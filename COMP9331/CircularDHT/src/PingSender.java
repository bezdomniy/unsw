
import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketTimeoutException;

public class PingSender implements Runnable {
	private DatagramSocket socket;
	private InetAddress host;
	private int firstPeerSequenceNumber;
	private int secondPeerSequenceNumber;
	
	private Integer firstSuccessorReplySequence;
	private Integer secondSuccessorReplySequence;

	private DHTPeer peer;

	public PingSender(InetAddress host, DatagramSocket socket, DHTPeer peer) {
		this.socket = socket;
		this.host = host;

		this.firstPeerSequenceNumber = 0;
		this.secondPeerSequenceNumber = 0;
		
		this.firstSuccessorReplySequence = 0;
		this.secondSuccessorReplySequence = 0;
		
		this.peer = peer;
	}

	public Integer ping(int serverPort, int sequenceNumber, int replySequence) throws IOException {
		String message = DHTPeer.padString(String.valueOf(this.peer.getPeerIdentity()), 3)
				+ DHTPeer.padString(String.valueOf(sequenceNumber), 2);
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
			return replySequence;
		}

		String reply = new String(replyPacket.getData(), 0, replyPacket.getLength());
		System.out.println("A ping response message has been received from "
				+ reply.substring(0, 3).trim());
		// System.out.println(reply);
		return Integer.parseInt(reply.substring(3).trim());

	}

	@Override
	public void run() {
		try {
			this.firstSuccessorReplySequence = ping(this.peer.getFirstSuccessorPort(), this.firstPeerSequenceNumber, this.firstSuccessorReplySequence);
			this.secondSuccessorReplySequence = ping(this.peer.getSecondSuccessorPort(), this.secondPeerSequenceNumber,this.secondSuccessorReplySequence);

		} catch (IOException ignore) {
		}

		
		if (this.firstSuccessorReplySequence + 3 <= this.firstPeerSequenceNumber) {
			System.out.println("Peer " + this.peer.getFirstSuccessorPort() + " is no longer alive.");
			String replacementSuccessor = this.peer.sendRequest("successor1", this.peer.getSecondSuccessorPort());
			
			this.firstPeerSequenceNumber = 0;
			this.firstSuccessorReplySequence = 0;

			RequestTrigger.updateSuccessor(this.peer, this.peer.getFirstSuccessorPort(), 0,
					Integer.parseInt(replacementSuccessor));

		} else {
			this.firstPeerSequenceNumber++;
		}
		
		if (this.secondSuccessorReplySequence + 3 <= this.secondPeerSequenceNumber) {
			System.out.println("Peer " + peer.getSecondSuccessorPort() + " is no longer alive");
			String replacementSuccessor = this.peer.sendRequest("successor2", this.peer.getFirstSuccessorPort());
			
			this.secondPeerSequenceNumber = 0;
			this.secondSuccessorReplySequence = 0;

			RequestTrigger.updateSuccessor(this.peer, this.peer.getSecondSuccessorPort(),
					Integer.parseInt(replacementSuccessor), 0);

		} else {
			this.secondPeerSequenceNumber++;
		}

	}

}