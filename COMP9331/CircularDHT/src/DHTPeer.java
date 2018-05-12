
import java.io.IOException;

public class DHTPeer {
	private Integer firstSuccessorPort;
	private Integer secondSuccessorPort;
	private Integer peerIdentity;

	private Integer[] predecessorPorts = { null, null };

	private Server server;
	private Client client;
	private UserInputListener inputListener;

	public static int hashFunction(String input) {
		return Integer.parseInt(input.trim()) % 256;
	}

	public Integer getFirstSuccessorPort() {
		return firstSuccessorPort;
	}

	public Integer getSecondSuccessorPort() {
		return secondSuccessorPort;
	}

	public Client getClient() {
		return client;
	}

	public DHTPeer(Integer port, Integer firstSuccessorPort, Integer secondSuccessorPort) throws IOException {
		setFirstSuccessorPort(firstSuccessorPort);
		setSecondSuccessorPort(secondSuccessorPort);

		this.peerIdentity = port;
		server = new Server(this, port);
		client = new Client(port);
		server.initialise();

		inputListener = new UserInputListener(this);
		inputListener.start();

		client.initialisePingSender(this);

	}

	public void sendRequest(String message, Integer targetPort) throws IOException {
		this.client.sendData(message, targetPort);
	}

	public void quit() throws IOException {
		String message = "quit" + padString(this.peerIdentity.toString(), 3)
				+ padString(this.firstSuccessorPort.toString(), 3) + padString(this.secondSuccessorPort.toString(), 3);
		sendRequest(message, this.predecessorPorts[0]);
		sendRequest(message, this.predecessorPorts[1]);

		this.inputListener.interrupt();
		this.server.terminate();
		this.client.terminatePingSender();
	}

	public void kill() {
		this.inputListener.interrupt();
		this.server.terminate();
		this.client.terminatePingSender();
	}

	public void requestFile(String fileName) throws IOException {
		System.out.println("File request message for " + fileName.trim() + " has been sent to my successor.");
		int fileHash = hashFunction(fileName);
		Pair checkFileInSuccessor = CheckFileInSuccessor(fileHash);
		String message = "request" + padString(String.valueOf(fileName), 4)
				+ padString(String.valueOf(this.peerIdentity), 3) + checkFileInSuccessor.getSecond();

		if (checkFileInSuccessor.getFirst() == 0) {
			System.out.println("I have file: " + fileName);
		} else if (checkFileInSuccessor.getFirst() == 1) {
			sendRequest(message, this.firstSuccessorPort);
		} else {
			sendRequest(message, this.secondSuccessorPort);
		}
	}

	public void forwardRequest(String fileName, String originPeerIdentity) throws IOException {
		System.out.println("File " + fileName.trim() + " is not stored here.");
		System.out.println("File request message has been forwarded to my successor.");
		int fileHash = hashFunction(fileName);
		Pair checkFileInSuccessor = CheckFileInSuccessor(fileHash);

		String message = "request" + padString(String.valueOf(fileName), 4) + originPeerIdentity
				+ checkFileInSuccessor.getSecond();

		if (checkFileInSuccessor.getFirst() == 1) {
			sendRequest(message, this.firstSuccessorPort);
		} else {
			sendRequest(message, this.secondSuccessorPort);
		}

	}

	public Integer getPeerIdentity() {
		return peerIdentity;
	}

	public void setFirstSuccessorPort(Integer firstSuccessorPort) {
		this.firstSuccessorPort = firstSuccessorPort;
	}

	public void setSecondSuccessorPort(Integer secondSuccessorPort) {
		this.secondSuccessorPort = secondSuccessorPort;
	}

	public Integer[] getPredecessorPorts() {
		return predecessorPorts;
	}

	public void setPredecessorPorts(Integer[] predecessorPorts) {
		this.predecessorPorts = predecessorPorts;
	}

	public static String padString(String s, int desiredLength) {
		StringBuilder ret;
		if (s.length() == desiredLength) {
			return s;
		} else if (s.length() < desiredLength) {
			int padLength = desiredLength - s.length();
			ret = new StringBuilder();
			for (int i = 0; i < padLength; i++) {
				ret.append(' ');
			}
			ret.append(s);
			return ret.toString();
		} else {
			return null;
		}
	}

	private Pair CheckFileInSuccessor(int target) {
		if (this.getPeerIdentity() < target) {
			if (this.getFirstSuccessorPort() >= target) {
				return new Pair(1, 1);
			}
			// if (this.getSecondSuccessorPort() >= target) {
			// return new Pair(2, 1);
			// }
			if (this.getFirstSuccessorPort() < this.getPeerIdentity()) {
				return new Pair(1, 1);
			}
			// if (this.getSecondSuccessorPort() < this.getPeerIdentity()) {
			// return new Pair(2, 1);
			// }
			return new Pair(1, 0);
		}
		if (this.getPeerIdentity() > target) {
			if (this.getFirstSuccessorPort() < this.getPeerIdentity() && this.getFirstSuccessorPort() >= target
					&& this.getSecondSuccessorPort() > target) {
				return new Pair(1, 1);
			}
			// if (this.getSecondSuccessorPort() < this.getPeerIdentity()) {
			// return new Pair(2, 1);
			// }
			return new Pair(1, 0);
		}

		return new Pair(0, 1);
	}

	private class Pair {
		private int first;
		private int second;

		public Pair(int first, int second) {
			this.first = first;
			this.second = second;
		}

		public int getFirst() {
			return first;
		}

		public int getSecond() {
			return second;
		}
	}

}
