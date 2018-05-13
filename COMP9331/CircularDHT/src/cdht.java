
import java.io.IOException;

public class cdht {
	// TO DO - Make sure peer tries to ping twice before assuming drop
	// TO DO - Update updatePredecessor function - it doesn't work properly

	public static void main(String[] args) throws IOException {
		DHTPeer peer = new DHTPeer(Integer.parseInt(args[0]), Integer.parseInt(args[1]), Integer.parseInt(args[2]));
	}
}
