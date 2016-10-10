package linkedlist;

import java.util.Scanner;
import java.io.BufferedReader;
import java.io.FileReader;
import java.io.IOException;

public class MyDlist extends DList{
	public MyDlist() {
		super();
	}
	
	public MyDlist(String f) throws IOException {
		super();
		String s = new String();
		
		if (f.equals("stdin")) {
			Scanner in = new Scanner(System.in);
			
			while (in.hasNextLine() && (!(s = in.nextLine()).equals(""))) {
				DNode t = new DNode(s,null,null);				
				this.addLast(t);
			}
			in.close();
		}
		else {
			BufferedReader in = new BufferedReader(new FileReader(f));
			
			while ((s = in.readLine()) != null) {
				String[] tokens = s.split(" ");
				for (String token:tokens) {
					DNode t = new DNode(token,null,null);				
					this.addLast(t);
				}
			}
//			in.close();
		}
	}
	
	private MyDlist(MyDlist other) {
		super();
		DNode node = other.getFirst();
		while (other.hasNext(node)) {
			DNode t = new DNode(node.getElement(),null,null);
			this.addLast(t);
			node = node.getNext();
		}
	}
	
	public static MyDlist cloneList(MyDlist u) {
		MyDlist copy = new MyDlist(u);
		return copy;
	}
	
	public void printList() {
		DNode node = getFirst();
		while (hasNext(node)) {
			System.out.println(node.getElement());
			node = node.getNext();
		}
	}
	
	public boolean contains(DNode m) {
		DNode n = this.getFirst();
		
		while (hasNext(n)) {
			if (n.getElement() == m.getElement()) {
				return true;
			}
			n=n.getNext();
		}

		return false;
	}
	
	public static MyDlist union(MyDlist u, MyDlist v) {
		MyDlist union = new MyDlist();
		DNode u1 = u.getFirst();
		DNode v1 = v.getFirst();
		
		while (u.hasNext(u1)) {
			union.addLast(u1);
			if (u1.getElement() != null) {
				System.out.println(u1.next.getElement());
			}
			
			u1 = u1.getNext();
			if (u1.getElement() != null) {
				System.out.println(u1.getElement());
			}
			
		}
		
//		while (v.hasNext(v1)) {
//			union.addLast(v1);
//			v1 = v1.getNext();
//		}
//		
		
		return union ;
		
	}
	
	 public static void main(String[] args) throws Exception{

		   System.out.println("please type some strings, one string each line and an empty line for the end of input:");
		    /** Create the first doubly linked list
		    by reading all the strings from the standard input. */
		    MyDlist firstList = new MyDlist("stdin");
	    
		   /** Print all elements in firstList */
//		    firstList.printList();
		   
		   /** Create the second doubly linked list by reading all the strings from the file myfile that contains some strings. */
		  
		   /** Replace the argument by the full path name of the text file */  
		    MyDlist secondList=new MyDlist("D:/COMP9024/myfile.txt");

		   /** Print all elememts in secondList */                     
//		    secondList.printList();
//
//		   /** Clone firstList */
		    MyDlist thirdList = cloneList(firstList);
//
//		   /** Print all elements in thirdList. */
//		    thirdList.printList();
//
//		  /** Clone secondList */
		    MyDlist fourthList = cloneList(secondList);
//
//		   /** Print all elements in fourthList. */
//		    fourthList.printList();
//		    
//		   /** Compute the union of firstList and secondList */
		    MyDlist fifthList = union(firstList, secondList);
//
//		   /** Print all elements in thirdList. */ 
		    fifthList.printList(); 
//
//		   /** Compute the intersection of thirdList and fourthList */
//		    MyDlist sixthList = intersection(thirdList, fourthList);
//
//		   /** Print all elements in fourthList. */
//		    sixthList.printList();
		  }
}
