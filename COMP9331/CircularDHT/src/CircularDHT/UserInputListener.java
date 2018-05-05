package CircularDHT;

import java.util.Scanner;

public class UserInputListener extends Thread {
	public UserInputListener() {
		// TODO Auto-generated constructor stub
	}
	
	@Override
	public void run() {
		while (!isInterrupted()) {
			// Maintain input cursor at bottom of the screen
			Scanner reader = new Scanner(System.in);
			System.out.print(">>");
			String n = reader.nextLine();
			System.out.print("\r");
			reader.close();
		}
	}
}
