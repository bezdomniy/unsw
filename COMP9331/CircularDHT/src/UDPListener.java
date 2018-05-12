
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketAddress;
import java.net.SocketException;

public class UDPListener extends Thread {
	private DatagramSocket udpSocket;
	private DHTPeer peer;

	public UDPListener(DHTPeer peer, DatagramSocket udpSocket) throws SocketException {
		this.peer = peer;
		this.udpSocket = udpSocket;
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

				System.out.println("A request message has been received from " + receivedData);

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

		}
	}

}
