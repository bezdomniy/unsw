package CircularDHT;

import java.io.IOException;

public class DHTPeer {
	private Integer firstSuccessorPort;
	private Integer secondSuccessorPort;
	
	private Integer[] predecessorPorts = {null,null};

	private Server server;
	private Client client;
	
	private PingResult pingResult;
	
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
		
		this.pingResult = new PingResult(false,false);
		
		server = new Server(this, port);
		server.initialise();		
		client = new Client(port);
		client.initialisePingSender(this.firstSuccessorPort, this.secondSuccessorPort, pingResult);

	}
	
	public void sendRequest(String sentence, Integer targetPort) throws IOException {
		this.client.sendData(sentence, targetPort);
	}
	
	public void quit() throws IOException {
		sendRequest("quit  "+this.firstSuccessorPort.toString(), this.predecessorPorts[0]);
		sendRequest("quit  "+this.firstSuccessorPort.toString(), this.predecessorPorts[1]);
		this.server.terminate();
		this.client.terminatePingSender();
	}


	public void setFirstSuccessorPort(Integer firstSuccessorPort) {
		this.firstSuccessorPort = firstSuccessorPort;
	}


	public void setSecondSuccessorPort(Integer secondSuccessorPort) {
		this.secondSuccessorPort = secondSuccessorPort;
	}


	public PingResult getPingResult() {
		return pingResult;
	}

	public Integer[] getPredecessorPorts() {
		return predecessorPorts;
	}

	public void setPredecessorPorts(Integer[] predecessorPorts) {
		this.predecessorPorts = predecessorPorts;
	}

}
