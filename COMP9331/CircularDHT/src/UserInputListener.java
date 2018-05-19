
import java.io.IOException;
import java.util.Scanner;

public class UserInputListener extends Thread {
	private DHTPeer peer;
	private Scanner reader;

	public UserInputListener(DHTPeer peer) {
		this.peer = peer;
	}

	@Override
	public void run() {
		while (!isInterrupted()) {
			System.out.print("");
			reader = new Scanner(System.in);
			String userInput = reader.nextLine();

			if (userInput.equals("quit")) {
				try {
					this.peer.quit();
				} catch (IOException ignore) {
				}
			} else if (userInput.length() > 8 && userInput.substring(0, 7).equals("request")) {
				String fileName = userInput.substring(8);
				if (fileName.length() > 4) {
					System.out.println("File name too long, must be 4 characters or fewer.");
					continue;
				}
				try {
					this.peer.requestFile(fileName);
				} catch (IOException ignore) {
				}
			}
		}
	}

	@Override
	public void interrupt() {
		reader.close();
		super.interrupt();
	}

}
