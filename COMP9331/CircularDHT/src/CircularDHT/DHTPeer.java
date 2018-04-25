package CircularDHT;

import java.io.IOException;

public class DHTPeer {
	private Integer firstSuccessorPort;
	private Integer secondSuccessorPort;
	
	private Integer[] predecessorPorts = {null,null};

	private Server server;
	private Client client;
	
	private PingResult pingResult;
	
	public DHTPeer(Integer port, Integer firstSuccessorPort, Integer secondSuccessorPort) throws IOException {
		setFirstSuccessorPort(firstSuccessorPort);
		setSecondSuccessorPort(secondSuccessorPort);
		
		this.pingResult = new PingResult(false,false);
		
		server = new Server(port, this.predecessorPorts);
		server.initialise();		
		client = new Client(port);
		client.initialisePingSender(this.firstSuccessorPort, this.secondSuccessorPort, pingResult);

	}
	
	public void sendRequest(String sentence) throws IOException {
		this.client.sendData(sentence);
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

}
