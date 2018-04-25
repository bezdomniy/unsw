package CircularDHT;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketTimeoutException;

public class PingSender implements Runnable {
	private DatagramSocket socket;
	private InetAddress host;
	private Integer firstPort;
	private Integer secondPort;
	
	private PingResult result;
	
	public PingSender(InetAddress host, DatagramSocket socket, Integer firstPort, Integer secondPort, PingResult pingResult) {
		this.socket = socket;
		this.host = host;
		this.firstPort = firstPort;
		this.secondPort = secondPort;

		this.result = pingResult;
	}
	
	
	public boolean ping(int serverPort) throws IOException {
		String message = String.valueOf(this.socket.getLocalPort()-50256);
		//System.out.println(message);
		byte[] data = new byte[256];
		data = message.getBytes();
		
		DatagramPacket sendPacket = new DatagramPacket(data, data.length, this.host, 50000 + serverPort);
		DatagramPacket replyPacket = new DatagramPacket(new byte[256], 256);
		socket.send(sendPacket);
		//System.out.println("Server "+this.socket.getLocalPort()+": "+"A ping message has been sent to "+serverPort);
		
		try {
			this.socket.receive(replyPacket);
		} catch (SocketTimeoutException e) {
			//System.out.println("Server "+this.socket.getLocalPort()+": "+"A response message has not been received from "+serverPort);
			return false;
		} 
		
		String reply = new String(replyPacket.getData(), 0, replyPacket.getLength());
		System.out.println("Server "+(this.socket.getLocalPort()-50256)+": "+"A response message has been received from "+reply);
		//System.out.println(reply);
		return true;
		
	}


	@Override
	public void run() {
		try {
			result.setFirstActive(ping(firstPort));
			result.setSecondActive(ping(secondPort));
		} catch (IOException ignore) {}
		//System.out.println("Server "+this.socket.getLocalPort()+": "+result.toString());

	}
	
	
}