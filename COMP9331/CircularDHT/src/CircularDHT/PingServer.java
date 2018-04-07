package CircularDHT;

import java.io.*;
import java.net.*;

public class PingServer {
	private DatagramSocket socket;
	private PingListenerThread listener;
	
	public PingServer(int port) throws SocketException {
		this.socket = new DatagramSocket(50000+port);
		this.listener = new PingListenerThread(this.socket);
	}
	
	public void initialise() throws IOException {
		this.listener.start();

	}
	
	public void terminate() {
		this.listener.interrupt();
	}
	
	public static void main(String[] args) throws IOException, InterruptedException {
		PingServer serv = new PingServer(2000);
		serv.initialise();
		Thread.sleep(1000);

		serv.terminate();
		System.out.println("Great success!");
		System.exit(0);
	}
	
}


 