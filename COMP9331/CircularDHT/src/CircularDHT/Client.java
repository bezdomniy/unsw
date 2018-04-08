package CircularDHT;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.SocketTimeoutException;
import java.net.UnknownHostException;

public class Client {
	private DatagramSocket socket;
	private InetAddress localhostIP;
	
	public Client(int port) throws SocketException, UnknownHostException {
		this.socket = new DatagramSocket(40000 + port);
		this.localhostIP = InetAddress.getLocalHost();
		this.socket.setSoTimeout(1000);
	}
	
	public boolean ping(int serverPort) throws IOException {
		String message = String.valueOf(this.socket.getLocalPort()-40000);
		//System.out.println(message);
		byte[] data = new byte[256];
		data = message.getBytes();
		
		DatagramPacket sendPacket = new DatagramPacket(data, data.length, this.localhostIP, 50000 + serverPort);
		DatagramPacket replyPacket = new DatagramPacket(new byte[256], 256);
		socket.send(sendPacket);
		
		try {
			this.socket.receive(replyPacket);
		} catch (SocketTimeoutException e) {
			return false;
		} 
		
		String reply = new String(replyPacket.getData(), 0, replyPacket.getLength());
		System.out.println("A response message has been received from "+reply);
		//System.out.println(reply);
		return true;
		
	}
}
