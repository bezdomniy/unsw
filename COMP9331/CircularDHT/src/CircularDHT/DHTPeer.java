package CircularDHT;

import java.io.IOException;

public class DHTPeer {
	private int firstNeighbourPort;
	private int secondNeighbourPort;
	private Server pingServer;
	private Client pingClient;
	
	private PingResult pingResult;
	
	public DHTPeer(int port, int firstNeighbour, int secondNeighbour) throws IOException {
		setFirstNeighbourPort(firstNeighbour);
		setSecondNeighbourPort(secondNeighbour);
		
		this.pingResult = new PingResult(false,false);
		
		pingServer = new Server(port);
		pingServer.initialise();
		
		pingClient = new Client(port);
		pingClient.initiatePingSender(firstNeighbourPort, secondNeighbourPort, pingResult);
	}

	
	public void setFirstNeighbourPort(int firstNeighbourPort) {
		this.firstNeighbourPort = firstNeighbourPort;
	}

	public void setSecondNeighbourPort(int secondNeighbourPort) {
		this.secondNeighbourPort = secondNeighbourPort;
	}
	
	public PingResult getPingResult() {
		return pingResult;
	}

}
