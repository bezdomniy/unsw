package CircularDHT;

import java.io.IOException;
import java.net.DatagramPacket;
import java.net.DatagramSocket;
import java.net.SocketAddress;

public class PingListener extends Thread {
	private DatagramSocket socket;
	private Integer[] predecessorPorts;

		
	public PingListener(DatagramSocket socket, Integer[] predecessorPorts) {
		this.socket = socket;
		this.predecessorPorts = predecessorPorts;
	}
	
	private void updatePredecessors(Integer receivedInteger) {
		if (this.predecessorPorts[0] == null) {
			this.predecessorPorts[0] = receivedInteger;
		} else if (!this.predecessorPorts[0].equals(receivedInteger)) {
			this.predecessorPorts[1] = receivedInteger;
		}

	}
	
	@Override
	public void run() {
		while(!isInterrupted()) {
			DatagramPacket receivedPacket = new DatagramPacket(new byte[256], 256);

	        try {
				this.socket.receive(receivedPacket);
				
				String receivedData = new String(receivedPacket.getData(), 0, receivedPacket.getLength());
				
				updatePredecessors(Integer.parseInt(receivedData));

				System.out.println("Server "+this.socket.getLocalPort()+": "+"A request message has been received from "+receivedData);
				
				System.out.println("Peer: "+this.socket.getLocalPort()+" First "+this.predecessorPorts[0]+", "+"Second "+this.predecessorPorts[1]);
				
				SocketAddress requestServer = receivedPacket.getSocketAddress();
				
				byte[] response = new byte[256];
				response = String.valueOf(socket.getLocalPort()-50000).getBytes();
				
				DatagramPacket responsePacket = new DatagramPacket(response, response.length, requestServer);
				
				socket.send(responsePacket);
				
				//System.out.println("Server "+this.socket.getLocalPort()+": "+"A response message has been sent");
				
			} catch (IOException ignore) {} 

		}
	}

}
