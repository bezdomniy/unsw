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

	private static final int TCP_ACCEPT_TIMEOUT = 1000;

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
					DataOutputStream outToClient = new DataOutputStream(tcpConnectionSocket.getOutputStream());
					int requesterPort = Integer.parseInt(requestBuffer.substring(4, 7).trim());
					Integer requesterFirstNeighbourPort = Integer.parseInt(requestBuffer.substring(7, 10).trim());
					Integer requesterSecondNeighbourPort = Integer.parseInt(requestBuffer.substring(10, 13).trim());
					RequestTrigger.updateSuccessor(this.peer, requesterPort, requesterFirstNeighbourPort,
							requesterSecondNeighbourPort);
					outToClient.writeBytes("Got it mate: " + requestBuffer + '\n');
				}

				else if (requestBuffer.substring(0, 4).equals("File")) {
					String fileName = requestBuffer.substring(4, 8).trim();
					String respondingPeer = requestBuffer.substring(8, 11).trim();
					System.out.println("Received a response message from peer " + respondingPeer
							+ ", which has the file " + fileName + ".");
				}

				else if (requestBuffer.substring(0, 7).equals("request")) {
					String fileName = requestBuffer.substring(7, 11);
					// int fileHash = DHTPeer.hashFunction(fileName.trim());
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
			} catch (IOException ignore) {
			}
		}
	}

}
