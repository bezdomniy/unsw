package CircularDHT;

import java.io.IOException;

public class DHTPeer {
	private Integer firstSuccessorPort;
	private Integer secondSuccessorPort;
	private Integer peerIdentity;

	private Integer[] predecessorPorts = { null, null };

	private Server server;
	private Client client;

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

		client.initialisePingSender(this);

	}

	public void sendRequest(String message, Integer targetPort) throws IOException {
		this.client.sendData(message, targetPort);
	}

	public void quit() throws IOException {
		String message = "quit  " + this.peerIdentity.toString() + "  " + this.firstSuccessorPort.toString() + "  "
				+ this.secondSuccessorPort.toString();
		sendRequest(message, this.predecessorPorts[0]);
		sendRequest(message, this.predecessorPorts[1]);
		this.server.terminate();
		this.client.terminatePingSender();
	}

	public void kill() {
		this.server.terminate();
		this.client.terminatePingSender();
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

}
