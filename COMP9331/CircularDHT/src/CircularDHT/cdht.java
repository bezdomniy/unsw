package CircularDHT;

import java.io.IOException;

public class cdht {
	
	// TO DO - Make peer close off last ping request if quitting between ping and response
	// TO DO - Make sure peer tries to ping twice before assuming drop
	
	public static void main(String[] args) throws IOException, InterruptedException {
		DHTPeer peer1 = new DHTPeer(1, 2, 3);
		//DHTPeer peer = new DHTPeer(Integer.parseInt(args[0]),Integer.parseInt(args[1]),Integer.parseInt(args[2]));
		DHTPeer peer2 = new DHTPeer(2, 3, 4);
		DHTPeer peer3 = new DHTPeer(3, 4, 1);
		DHTPeer peer4 = new DHTPeer(4, 1, 2);
		
		Thread.sleep(4000);
		peer1.request("260");
		//peer4.kill();
		//peer4.quit();
		//System.out.println("quitting 4"+"\r");
		//System.out.println("Success");
		
		//System.out.println(peer2.getPingResult());
		//System.exit(0);

	}
}
