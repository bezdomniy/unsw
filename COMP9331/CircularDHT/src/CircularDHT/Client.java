package CircularDHT;

import java.net.DatagramSocket;
import java.net.InetAddress;
import java.net.SocketException;
import java.net.UnknownHostException;
import java.util.concurrent.Executors;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledFuture;
import java.util.concurrent.TimeUnit;

public class Client {
	private DatagramSocket socket;
	private InetAddress localhostIP;
	private PingSender pingSender;
	
	private static final int PING_INTERVAL = 3;
	
	private final ScheduledExecutorService scheduler = Executors.newScheduledThreadPool(1);
		
	public Client(int port) throws SocketException, UnknownHostException {
		this.socket = new DatagramSocket(40000 + port);
		this.localhostIP = InetAddress.getLocalHost();
		this.socket.setSoTimeout(1000);
	}
	
	public void initiatePingSender(int firstNeighbourPort, int secondNeighbourPort, PingResult pingResult) {
		this.pingSender = new PingSender(this.localhostIP, this.socket, firstNeighbourPort, secondNeighbourPort, pingResult);
		
		final ScheduledFuture<?> pingHandler = scheduler.scheduleAtFixedRate(pingSender, 1, PING_INTERVAL, TimeUnit.SECONDS);

	}


}
