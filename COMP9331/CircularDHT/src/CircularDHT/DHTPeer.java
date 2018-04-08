package CircularDHT;

import java.io.IOException;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.ArrayList;
import java.util.Arrays;

public class DHTPeer {
	private int firstNeighbourPort;
	private int secondNeighbourPort;
	private Server pingServer;
	private Client pingClient;
	
	public DHTPeer(int port, int firstNeighbour, int secondNeighbour) throws IOException {
		setFirstNeighbourPort(firstNeighbour);
		setSecondNeighbourPort(secondNeighbour);
		
		pingServer = new Server(port);
		pingServer.initialise();
		
		pingClient = new Client(port);
	}
	

	public PingResult pingNeighbours() throws IOException {
		boolean firstActive = false;
		boolean secondActive = false;
		if (pingClient.ping(this.firstNeighbourPort)) {
			firstActive = true;
		}
		if (pingClient.ping(this.secondNeighbourPort)) {
			secondActive = true;
		}
		
		PingResult ret = new PingResult(firstActive, secondActive);
		
		return ret;
		
		
	}

	
	public void setFirstNeighbourPort(int firstNeighbourPort) {
		this.firstNeighbourPort = firstNeighbourPort;
	}

	public void setSecondNeighbourPort(int secondNeighbourPort) {
		this.secondNeighbourPort = secondNeighbourPort;
	}

}
