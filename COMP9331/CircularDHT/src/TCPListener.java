import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.ServerSocket;
import java.net.Socket;
import java.net.SocketException;

public class TCPListener extends Thread {
	private ServerSocket tcpSocket;
	private DHTPeer peer;

	private static final int TCP_ACCEPT_TIMEOUT = 2000;

	public TCPListener(DHTPeer peer, ServerSocket tcpSocket) throws SocketException {
		this.peer = peer;
		this.tcpSocket = tcpSocket;
		this.tcpSocket.setSoTimeout(TCP_ACCEPT_TIMEOUT);
	}

	@Override
	public void run() {
		while (!isInterrupted()) {
			// Receive TCP connection requests
			try {
				Socket tcpConnectionSocket = this.tcpSocket.accept();
				BufferedReader receivedData = new BufferedReader(
						new InputStreamReader(tcpConnectionSocket.getInputStream()));
				String requestBuffer = receivedData.readLine();

				if (requestBuffer.substring(0, 4).equals("quit")) {
					int requesterPort = Integer.parseInt(requestBuffer.substring(4, 7).trim());
					Integer requesterFirstNeighbourPort = Integer.parseInt(requestBuffer.substring(7, 10).trim());
					Integer requesterSecondNeighbourPort = Integer.parseInt(requestBuffer.substring(10, 13).trim());
					System.out.println("Peer " + requesterPort + " will depart from the network.");
					this.peer.updateSuccessor(requesterPort, requesterFirstNeighbourPort,
							requesterSecondNeighbourPort);
				}

				else if (requestBuffer.substring(0, 4).equals("File")) {
					String fileName = requestBuffer.substring(4, 8).trim();
					String respondingPeer = requestBuffer.substring(8, 11).trim();
					System.out.println("Received a response message from peer " + respondingPeer
							+ ", which has the file " + fileName + ".");
				}

				else if (requestBuffer.substring(0, 7).equals("request")) {
					String fileName = requestBuffer.substring(7, 11);
					boolean fileIsHere = requestBuffer.substring(14, 15).equals("1");
					if (fileIsHere) {
						int originPeerPort = Integer.parseInt(requestBuffer.substring(11, 14).trim());
						System.out.println("File " + fileName.trim() + " is stored here.");
						System.out.println(
								"A response message, destined for peer " + originPeerPort + ", has been sent.");
						peer.getClient().sendData(
								"File" + fileName + DHTPeer.padString(String.valueOf(this.peer.getPeerIdentity()), 3),
								originPeerPort);
					} else {
						peer.forwardRequest(fileName, requestBuffer.substring(11, 14));
					}
				}

				else if (requestBuffer.substring(0, 9).equals("successor")) {
					String message = "";

					DataOutputStream outToClient = new DataOutputStream(tcpConnectionSocket.getOutputStream());
					if (requestBuffer.substring(9).equals("1")) {
						message = String.valueOf(this.peer.getFirstSuccessorPort());
					} else if (requestBuffer.substring(9).equals("2")) {
						message = String.valueOf(this.peer.getSecondSuccessorPort());
					}

					outToClient.writeBytes(message + '\n');
				}

			} catch (IOException ignore) {
			}
		}
	}

}
