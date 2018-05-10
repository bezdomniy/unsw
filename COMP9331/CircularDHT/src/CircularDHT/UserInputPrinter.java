package CircularDHT;

import java.util.Scanner;

public class UserInputPrinter extends Thread {
	public UserInputPrinter() {
		// TODO Auto-generated constructor stub
	}
	
	@Override
	public void run() {
		UserInputListener inputLister = new UserInputListener();
		while (!isInterrupted()) {
			System.out.print("");

			try {
				Thread.sleep(100);
			} catch (InterruptedException e) {
				// TODO Auto-generated catch block
				e.printStackTrace();
			}
			//System.out.print("\r");
		}
	}
	
	public static void main(String[] args) {
		UserInputPrinter u = new UserInputPrinter();
		u.run();
	}
}
