package CircularDHT;

import java.io.IOException;

public class cdht {
	public static void main(String[] args) throws IOException, InterruptedException {
		DHTPeer peer1 = new DHTPeer(1, 2, 3);
		DHTPeer peer2 = new DHTPeer(2, 3, 4);
		DHTPeer peer3 = new DHTPeer(3, 4, 1);
		DHTPeer peer4 = new DHTPeer(4, 1, 2);
		
		Thread.sleep(1000);
		
		peer4.quit();
		//System.out.println("Success");
		
		//System.out.println(peer2.getPingResult());
		//System.exit(0);

	}
}
