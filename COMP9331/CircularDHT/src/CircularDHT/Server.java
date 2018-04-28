package CircularDHT;

import java.io.*;
import java.net.*;

public class Server {
	private DatagramSocket udpSocket;
	private ServerSocket tcpSocket;
	private Listener listener;

	public Server(DHTPeer peer, Integer port) throws IOException {
		this.udpSocket = new DatagramSocket(50000+port);
		this.tcpSocket = new ServerSocket(50256+port);
		this.listener = new Listener(peer, this.udpSocket, this.tcpSocket);
	}


	public void initialise() throws IOException {
		this.listener.start();

	}
	
	public void terminate() {
		this.listener.interrupt();
	}

}


 