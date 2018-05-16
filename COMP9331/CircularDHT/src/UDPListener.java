
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
				String pingSender = receivedData.substring(0, 3).trim();
				String sequenceNumber = receivedData.substring(3).trim();
				Integer PingSenderInteger = Integer.parseInt(pingSender);

				// Populate predecessor array. If you get a new ping, reset it to null and start
				// again.
				if (peer.getPredecessorPorts()[0] == null || peer.getPredecessorPorts()[1] == null) {
					this.peer.updatePredecessors(pingSender);
				} else if (!PingSenderInteger.equals(peer.getPredecessorPorts()[0])
						&& !PingSenderInteger.equals(peer.getPredecessorPorts()[1])) {
					Integer[] nullPredecessors = { null, null };
					peer.setPredecessorPorts(nullPredecessors);
					this.peer.updatePredecessors(pingSender);
				}

				System.out.println("A ping request message has been received from " + pingSender);

				SocketAddress requestServer = receivedPacket.getSocketAddress();

				byte[] response = new byte[256];
				String responseMessage = DHTPeer.padString(String.valueOf(this.peer.getPeerIdentity()), 3)
						+ sequenceNumber;
				response = responseMessage.getBytes();

				DatagramPacket responsePacket = new DatagramPacket(response, response.length, requestServer);

				this.udpSocket.send(responsePacket);

			} catch (IOException ignore) {
			}

		}
	}

}
