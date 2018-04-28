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

public class Listener extends Thread {
	private DatagramSocket udpSocket;
	private ServerSocket tcpSocket;
	private DHTPeer peer;

	private static final int TCP_ACCEPT_TIMEOUT = 1000;

	public Listener(DHTPeer peer, DatagramSocket udpSocket, ServerSocket tcpSocket) throws SocketException {
		this.peer = peer;
		this.udpSocket = udpSocket;
		this.tcpSocket = tcpSocket;
		this.tcpSocket.setSoTimeout(TCP_ACCEPT_TIMEOUT);
	}

	@Override
	public void run() {
		//System.out.println(this.peer.getClient().getPeerIdentity()+" running");
		while (!isInterrupted()) {
			DatagramPacket receivedPacket = new DatagramPacket(new byte[256], 256);

			try {
				this.udpSocket.receive(receivedPacket);

				String receivedData = new String(receivedPacket.getData(), 0, receivedPacket.getLength());

				RequestTrigger.updatePredecessors(this.peer, receivedData);

				System.out.println("Server " + (this.udpSocket.getLocalPort() - 50000) + ": "
						+ "A request message has been received from " + receivedData);

				SocketAddress requestServer = receivedPacket.getSocketAddress();

				byte[] response = new byte[256];
				// add first and second peer to message
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

			try {
				Socket tcpConnectionSocket = this.tcpSocket.accept();
				BufferedReader receivedData = new BufferedReader(
						new InputStreamReader(tcpConnectionSocket.getInputStream()));

				DataOutputStream outToClient = new DataOutputStream(tcpConnectionSocket.getOutputStream());
				String requestBuffer = receivedData.readLine();

				int requesterPort = Integer.parseInt(requestBuffer.substring(4, 7).trim());
				if (requestBuffer.substring(0, 4).equals("quit")) {
					Integer requesterFirstNeighbourPort = Integer.parseInt(requestBuffer.substring(7, 10).trim());
					Integer requesterSecondNeighbourPort = Integer.parseInt(requestBuffer.substring(10, 13).trim());
					RequestTrigger.updateSuccessor(this.peer, requesterPort, requesterFirstNeighbourPort, requesterSecondNeighbourPort);
				}

				outToClient.writeBytes(
						"Server " + (this.udpSocket.getLocalPort() - 50000) + "Got it mate: " + requestBuffer + '\n');

			} catch (IOException ignore) {
			}

		}
	}

}