
import java.io.IOException;

public class cdht {
	public static void main(String[] args) throws IOException {
		new DHTPeer(Integer.parseInt(args[0]), Integer.parseInt(args[1]), Integer.parseInt(args[2]));
	}
}
