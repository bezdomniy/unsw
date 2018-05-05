package CircularDHT;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketAddress;
import java.net.SocketException;
import java.util.Scanner;

public class ServerListener extends Thread {
	private DatagramSocket udpSocket;
	private ServerSocket tcpSocket;
	private DHTPeer peer;

	private static final int TCP_ACCEPT_TIMEOUT = 1000;

	public ServerListener(DHTPeer peer, DatagramSocket udpSocket, ServerSocket tcpSocket) throws SocketException {
		this.peer = peer;
		this.udpSocket = udpSocket;
		this.tcpSocket = tcpSocket;
		this.tcpSocket.setSoTimeout(TCP_ACCEPT_TIMEOUT);
	}

	@Override
	public void run() {
		while (!isInterrupted()) {
			// Receive ping requests
			DatagramPacket receivedPacket = new DatagramPacket(new byte[256], 256);
			try {
				this.udpSocket.receive(receivedPacket);

				String receivedData = new String(receivedPacket.getData(), 0, receivedPacket.getLength());

				RequestTrigger.updatePredecessors(this.peer, receivedData);

				System.out.println("Server " + (this.udpSocket.getLocalPort() - 50000) + ": "
						+ "A request message has been received from " + receivedData);

				SocketAddress requestServer = receivedPacket.getSocketAddress();

				byte[] response = new byte[256];
				String responseMessage = String.valueOf(this.udpSocket.getLocalPort() - 50000)
						+ String.valueOf(this.peer.getFirstSuccessorPort())
						+ String.valueOf(this.peer.getSecondSuccessorPort());
				response = responseMessage.getBytes();

				DatagramPacket responsePacket = new DatagramPacket(response, response.length, requestServer);

				this.udpSocket.send(responsePacket);

				// System.out.println("Server "+this.socket.getLocalPort()+": "+"A response
				// message has been sent");

			} catch (IOException ignore) {
			}

			// Receive TCP connection requests
			try {
				Socket tcpConnectionSocket = this.tcpSocket.accept();
				BufferedReader receivedData = new BufferedReader(
						new InputStreamReader(tcpConnectionSocket.getInputStream()));
				String requestBuffer = receivedData.readLine();

				
				if (requestBuffer.substring(0, 4).equals("quit")) {
					DataOutputStream outToClient = new DataOutputStream(tcpConnectionSocket.getOutputStream());
					int requesterPort = Integer.parseInt(requestBuffer.substring(4, 7).trim());
					Integer requesterFirstNeighbourPort = Integer.parseInt(requestBuffer.substring(7, 10).trim());
					Integer requesterSecondNeighbourPort = Integer.parseInt(requestBuffer.substring(10, 13).trim());
					RequestTrigger.updateSuccessor(this.peer, requesterPort, requesterFirstNeighbourPort, requesterSecondNeighbourPort);
					outToClient.writeBytes(
							"Server " + (this.udpSocket.getLocalPort() - 50000) + "Got it mate: " + requestBuffer + '\n');
				}
				
				
				else if (requestBuffer.substring(0, 4).equals("File")) {
					String fileName = requestBuffer.substring(4, 8).trim();
					String respondingPeer = requestBuffer.substring(8, 11).trim();
					System.out.println("Received a response message from peer "+ respondingPeer +", which has the file "+ fileName +".");
				}
				
				else if (requestBuffer.substring(0, 7).equals("request")) {
					System.out.println("message "+requestBuffer);
					String fileName = requestBuffer.substring(7, 11);
					int fileHash = DHTPeer.hashFunction(fileName.trim());
					if (fileHash == peer.getPeerIdentity()) {
						System.out.println("File "+fileName.trim()+" is here.");
						int originPeerPort = Integer.parseInt(requestBuffer.substring(11, 14).trim());
						System.out.println("A response message, destined for peer "+ originPeerPort +", has been sent.");
						peer.getClient().sendData("File"+fileName+fileName+DHTPeer.padString(String.valueOf(this.peer.getPeerIdentity()),3), originPeerPort);
					}
					else {
						System.out.println("File "+fileName.trim()+" is not stored here.");
						peer.forwardRequest(fileName, requestBuffer.substring(10, 13));
					}
				}

				

			} catch (IOException ignore) {
			}
			
			

		}
	}

}
