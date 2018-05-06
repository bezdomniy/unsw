package CircularDHT;

import java.util.Scanner;

public class UserInputListener extends Thread {
	public UserInputListener() {
		// TODO Auto-generated constructor stub
	}
	
	@Override
	public void run() {
		while (!isInterrupted()) {
			Scanner reader = new Scanner(System.in);
			String n = reader.nextLine();
		}
	}
}
