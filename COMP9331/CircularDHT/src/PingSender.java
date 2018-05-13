
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
		// System.out.println("Restarting client with successors " +
		// peer.getFirstSuccessorPort() + " and "
		// + peer.getSecondSuccessorPort());
		this.socket = socket;
		this.host = host;

		this.peer = peer;
	}

	public boolean ping(int serverPort) throws IOException {
		String message = String.valueOf(this.peer.getPeerIdentity());
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
			return false;
		}

		String reply = new String(replyPacket.getData(), 0, replyPacket.getLength());
		System.out.println(
				"A ping response message has been received from " + reply.substring(0, 3).replaceFirst("^0+(?!$)", "").trim());
		// System.out.println(reply);
		return true;

	}

	@Override
	public void run() {
		boolean firstActive = false;
		boolean secondActive = false;

		try {
			firstActive = ping(this.peer.getFirstSuccessorPort());
			secondActive = ping(this.peer.getSecondSuccessorPort());

		} catch (IOException ignore) {
		}

		if (!firstActive && !secondActive) {
			System.out.println("Both peers dropped at once, no way to recover from this! Exiting...");
			System.exit(0);
		} else if (!firstActive) {
			// Remove this output
			System.out.println("Peer " + this.peer.getFirstSuccessorPort() + " is no longer alive.");
			String replacementSuccessor = this.peer.sendRequest("successor1", this.peer.getSecondSuccessorPort());

			RequestTrigger.updateSuccessor(this.peer, this.peer.getFirstSuccessorPort(), 0,
					Integer.parseInt(replacementSuccessor));

		} else if (!secondActive) {
			System.out.println("Peer " + peer.getSecondSuccessorPort() + " is no longer alive");
			String replacementSuccessor = this.peer.sendRequest("successor2", this.peer.getFirstSuccessorPort());

			RequestTrigger.updateSuccessor(this.peer, this.peer.getSecondSuccessorPort(),
					Integer.parseInt(replacementSuccessor), 0);

		}

		// System.out.println("Server "+this.socket.getLocalPort()+":
		// "+result.toString());

	}

}