
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketTimeoutException;

public class PingSender implements Runnable {
	private DatagramSocket socket;
	private InetAddress host;

	private DHTPeer peer;

	public PingSender(InetAddress host, DatagramSocket socket, DHTPeer peer) {
		// Remove this output
		//System.out.println("Restarting client with successors " + peer.getFirstSuccessorPort() + " and "
		//		+ peer.getSecondSuccessorPort());
		this.socket = socket;
		this.host = host;

		this.peer = peer;
	}

	public PingResult ping(int serverPort) throws IOException {
		String message = String.valueOf(this.socket.getLocalPort() - 50256);
		// System.out.println(message);
		byte[] data = new byte[256];
		data = message.getBytes();

		DatagramPacket sendPacket = new DatagramPacket(data, data.length, this.host, 50000 + serverPort);
		DatagramPacket replyPacket = new DatagramPacket(new byte[256], 256);
		socket.send(sendPacket);
		// System.out.println("***** Server " + (this.socket.getLocalPort() - 50256) +
		// ": A ping message has been sent to "
		// + serverPort+"\r");

		try {
			this.socket.receive(replyPacket);
		} catch (SocketTimeoutException e) {
			// System.out.println("Server "+this.socket.getLocalPort()+": "+"A response
			// message has not been received from "+serverPort);
			return new PingResult(false, null, null);
		}

		String reply = new String(replyPacket.getData(), 0, replyPacket.getLength());
		System.out.println("A response message has been received from " + reply.substring(0, 1));
		// System.out.println(reply);
		return new PingResult(true, Integer.parseInt(reply.substring(1, 2)), Integer.parseInt(reply.substring(2, 3)));

	}

	@Override
	public void run() {
		PingResult firstActive = new PingResult(false, null, null);
		PingResult secondActive = new PingResult(false, null, null);

		try {
			firstActive = ping(this.peer.getFirstSuccessorPort());
			secondActive = ping(this.peer.getSecondSuccessorPort());

		} catch (IOException ignore) {
		}

		if (!firstActive.isActive() && !secondActive.isActive()) {
			System.out.println("Both peers dropped at once, no way to recover from this! Exiting...");
			System.exit(0);
		} else if (!firstActive.isActive()) {
			// Remove this output
			System.out.println("Peer " + peer.getFirstSuccessorPort() + " is no longer alive.");
			RequestTrigger.updateDroppedSuccessor(this.peer, this.peer.getFirstSuccessorPort(),
					secondActive.getFirstNeighbour());
			System.out.println("My first successor is now " + peer.getFirstSuccessorPort() + ".");
			System.out.println("My second successor is now " + peer.getSecondSuccessorPort() + ".");
		} else if (!secondActive.isActive()) {
			System.out.println("Peer " + peer.getSecondSuccessorPort() + " is no longer alive");
			RequestTrigger.updateDroppedSuccessor(this.peer, this.peer.getSecondSuccessorPort(),
					firstActive.getSecondNeighbour());
			System.out.println("My second successor is now " + peer.getSecondSuccessorPort() + ".");
		}
		
		// System.out.println("Server "+this.socket.getLocalPort()+":
		// "+result.toString());

	}

}