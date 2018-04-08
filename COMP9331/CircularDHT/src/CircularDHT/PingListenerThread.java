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
			DatagramPacket receivedPacket = new DatagramPacket(new byte[256], 256);
			
			//System.out.println("Init!");
	        try {
				this.socket.receive(receivedPacket);
				
				String receivedData = new String(receivedPacket.getData(), 0, receivedPacket.getLength());
				
				System.out.println("A request message has been received from "+receivedData);
				
				SocketAddress requestServer = receivedPacket.getSocketAddress();
				
				byte[] response = new byte[256];
				response = String.valueOf(socket.getLocalPort()-50000).getBytes();
				
				DatagramPacket responsePacket = new DatagramPacket(response, response.length, requestServer);
				
				socket.send(responsePacket);
				
			} catch (IOException e) {
				e.printStackTrace();
			} 

		}
	}
	
}
