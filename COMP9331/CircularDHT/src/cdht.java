
import java.io.IOException;

public class cdht {
	// TO DO - Fix ordering issue for peers dropping and updating successors

	public static void main(String[] args) throws IOException {
		new DHTPeer(Integer.parseInt(args[0]), Integer.parseInt(args[1]), Integer.parseInt(args[2]));
	}
}
