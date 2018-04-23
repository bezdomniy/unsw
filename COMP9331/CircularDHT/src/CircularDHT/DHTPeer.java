package CircularDHT;

import java.io.IOException;

public class DHTPeer {
	private Integer firstSuccessorPort;
	private Integer secondSuccessorPort;
	
	private Integer[] predecessorPorts = {null,null};

	private PingServer pingServer;
	private PingClient pingClient;
	
	private PingResult pingResult;
	
	public DHTPeer(Integer port, Integer firstSuccessorPort, Integer secondSuccessorPort) throws IOException {
		setFirstSuccessorPort(firstSuccessorPort);
		setSecondSuccessorPort(secondSuccessorPort);
		
		this.pingResult = new PingResult(false,false);
		
		pingServer = new PingServer(port, this.predecessorPorts);
		pingServer.initialise();		
		pingClient = new PingClient(port);
		pingClient.initialisePingSender(this.firstSuccessorPort, this.secondSuccessorPort, pingResult);

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
