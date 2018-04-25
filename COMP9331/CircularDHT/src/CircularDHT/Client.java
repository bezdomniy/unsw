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
	
	private static final int PING_INTERVAL = 5;
	private static final int UDP_RESPONSE_TIMEOUT = 1000;
	
	private final ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(1);
		
	public Client(Integer peerIdentity) throws SocketException, UnknownHostException {
		this.peerIdentity = peerIdentity;
		this.udpSocket = new DatagramSocket(50256 + this.peerIdentity);
		
		this.localhostIP = InetAddress.getLocalHost();
		
		
		this.udpSocket.setSoTimeout(UDP_RESPONSE_TIMEOUT);
	}
	
	public void initialisePingSender(Integer firstNeighbourPort, Integer secondNeighbourPort, PingResult pingResult) {
		this.pingSender = new PingSender(this.localhostIP, this.udpSocket, firstNeighbourPort, secondNeighbourPort, pingResult);	
		final ScheduledFuture<?> pingHandler = scheduler.scheduleAtFixedRate(pingSender, 0, PING_INTERVAL, TimeUnit.SECONDS);
	}
	
	public void sendData(String dataToSend) throws IOException {
		Socket tcpSocket = new Socket(this.localhostIP, 50256 + this.peerIdentity);
		DataOutputStream request = new DataOutputStream(tcpSocket.getOutputStream());
		request.writeBytes(dataToSend + "\n");
		
		BufferedReader responseBuffer = new BufferedReader(new InputStreamReader(tcpSocket.getInputStream()));
		String response = responseBuffer.readLine();
		System.out.println(response);
		
		tcpSocket.close();
	}


}
