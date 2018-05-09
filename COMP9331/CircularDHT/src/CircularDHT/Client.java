package CircularDHT;

import java.io.BufferedReader;
import java.io.DataOutputStream;
import java.io.IOException;
import java.io.InputStreamReader;
import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.Socket;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

public class Client {
	private DatagramSocket udpSocket;
	private InetAddress localhostIP;
	private PingSender pingSender;
	private Integer peerIdentity;
	
	private static final int PING_INTERVAL = 10;
	private static final int UDP_RESPONSE_TIMEOUT = 3000;
	
	private final ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(1);
		
	public Client(Integer peerIdentity) throws SocketException, UnknownHostException {
		this.peerIdentity = peerIdentity;
		this.udpSocket = new DatagramSocket(50256 + this.peerIdentity);
		
		this.localhostIP = InetAddress.getLocalHost();
		
		this.udpSocket.setSoTimeout(UDP_RESPONSE_TIMEOUT);
	}
	
	public Integer getPeerIdentity() {
		return peerIdentity;
	}

	public void initialisePingSender(DHTPeer peer) {
		this.pingSender = new PingSender(this.localhostIP, this.udpSocket, peer);	
		scheduler.scheduleAtFixedRate(pingSender, 2, PING_INTERVAL, TimeUnit.SECONDS);
	}
	
	public void terminatePingSender() {
		//this.scheduler.shutdown();
		this.scheduler.shutdownNow();
	}
	
	public void sendData(String dataToSend, Integer targetPort) throws IOException {
		Socket tcpSocket = new Socket(this.localhostIP, targetPort + 50256);
		DataOutputStream request = new DataOutputStream(tcpSocket.getOutputStream());
		request.writeBytes(dataToSend + "\n");
		
		//BufferedReader responseBuffer = new BufferedReader(new InputStreamReader(tcpSocket.getInputStream()));
		//String response = responseBuffer.readLine();
		//System.out.println(response);
		tcpSocket.close();
	}

	


}
