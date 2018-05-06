package CircularDHT;

import java.io.*;
import java.net.*;

public class Server {
	private DatagramSocket udpSocket;
	private ServerSocket tcpSocket;
	private UDPListener udpListener;
	private TCPListener tcpListener;

	public Server(DHTPeer peer, Integer port) throws IOException {
		this.udpSocket = new DatagramSocket(50000+port);
		this.tcpSocket = new ServerSocket(50256+port);
		this.udpListener = new UDPListener(peer, this.udpSocket);
		this.tcpListener = new TCPListener(peer, this.tcpSocket);
	}


	public void initialise() throws IOException {
		this.udpListener.start();
		this.tcpListener.start();

	}
	
	public void terminate() {
		this.udpListener.interrupt();
	}

}


 