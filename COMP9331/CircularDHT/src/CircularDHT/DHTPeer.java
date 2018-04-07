package CircularDHT;

import java.io.IOException;
import java.net.SocketException;

public class DHTPeer {
	private int firstNeighbourPort;
	private int secondNeighbourPort;
	private PingServer pingServer;
	private PingClient pingClient;
	
	public DHTPeer(int port, int firstNeighbour, int secondNeighbour) throws IOException {
		setFirstNeighbourPort(firstNeighbour);
		setSecondNeighbourPort(secondNeighbour);
		
		pingServer = new PingServer(port);
		pingServer.initialise();
		
		pingClient = new PingClient(port);
	}
	

	public int pingNeighbours() throws IOException {
		if (pingClient.ping(this.firstNeighbourPort)) {
			return 0;
		}
		else if (pingClient.ping(this.secondNeighbourPort)) {
			return 1;
		}
		return 2;		
	}

	
	public void setFirstNeighbourPort(int firstNeighbourPort) {
		this.firstNeighbourPort = firstNeighbourPort;
	}

	public void setSecondNeighbourPort(int secondNeighbourPort) {
		this.secondNeighbourPort = secondNeighbourPort;
	}

}
