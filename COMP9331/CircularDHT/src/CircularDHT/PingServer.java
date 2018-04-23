package CircularDHT;

import java.io.*;
import java.net.*;

public class PingServer {
	private DatagramSocket socket;
	private PingListener listener;

	public PingServer(Integer port, Integer[] predecessorPorts) throws SocketException {
		this.socket = new DatagramSocket(50000+port);
		this.listener = new PingListener(this.socket, predecessorPorts);
	}


	public void initialise() throws IOException {
		this.listener.start();

	}
	
	public void terminate() {
		this.listener.interrupt();
	}

}


 