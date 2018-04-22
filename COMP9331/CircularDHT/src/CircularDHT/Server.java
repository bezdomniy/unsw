package CircularDHT;

import java.io.*;
import java.net.*;

public class Server {
	private DatagramSocket socket;
	private PingListener listener;
	
	public Server(int port) throws SocketException {
		this.socket = new DatagramSocket(50000+port);
		this.listener = new PingListener(this.socket);
	}
	
	public void initialise() throws IOException {
		this.listener.start();

	}
	
	public void terminate() {
		this.listener.interrupt();
	}
	
}


 