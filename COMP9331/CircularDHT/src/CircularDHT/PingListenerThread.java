package CircularDHT;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketAddress;

public class PingListenerThread extends Thread {
	private DatagramSocket socket;
	
	public PingListenerThread(DatagramSocket socket) {
		this.socket = socket;
	}
	
	
	@Override
	public void run() {
		while(!isInterrupted()) {
			DatagramPacket request = new DatagramPacket(new byte[1024], 1024);
			//System.out.println("Init!");
	        try {
				this.socket.receive(request);
				SocketAddress requestServer = request.getSocketAddress();
				String message = "## TO-DO";
				byte[] data = new byte[1024];
				data = message.getBytes();
				
				DatagramPacket sendPacket = new DatagramPacket(data, data.length, requestServer);
				socket.send(sendPacket);
				
			} catch (IOException e) {
				e.printStackTrace();
			} 

		}
	}
	
}
