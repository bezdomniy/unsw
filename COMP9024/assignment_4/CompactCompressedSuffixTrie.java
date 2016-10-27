import java.io.*;
import java.util.*;

public class CompactCompressedSuffixTrie {

/** You need to define your data structures for the compressed trie */
/** Constructor */
	private Node root;
	protected static String text;

  // Create a compact compressed suffix trie from file f
	public CompactCompressedSuffixTrie(String f) {
		this.root = new Node();
		this.text = readFile(f);
	
		makeTree(0,this.text.length()-1,this.root);

	}
	
	public String readFile(String f) {
		Scanner input = null;
		String out = null;
		
		try {
			input = new Scanner(new File(f));
		}
		catch (FileNotFoundException e) {
			 System.err.println(f+" does not exist");
		}
		
		while (input.hasNextLine()) {
			out = input.nextLine();
			// System.out.println(text);
		}
		return out;
	}
	
	public void makeTree(int start, int end, Node n) {
		while (!(start == end+1)) {
			makeBranch(start,end,n);
			start += 1;
		}
	}
	
	// public void makeSubtree(String s, Node n) {
		// String substr = s;
		
		// while (!substr.equals("")) {
			// System.out.println(substr);
			// makeBranch(substr,n);
			// substr=substr.substring(1);
		// }
	// }
	
	public void makeBranch(int start, int end, Node n) {
		// To fix - index out of range when using end+1
		if (start == end) {
			n.setTerminalNode(true);
			return;
		}

		Node prefix = new Node(start,start+1);
		// System.out.println(prefix.getValue());
		// String suffix = s.substring(1);
		

		Node leaf = null;
		Node leafPrefix = null;
		// String leafSuffix = null;
		for (Node l: n.children.values()) {
			if (l.isTerminalNode()) {
				leaf = l;
				leafPrefix = new Node(leaf.startIndex,leaf.startIndex+1);
				// leafSuffix = leaf.value.substring(1,leaf.value.length() - 1);
				break;
			}
		}
		
		if (n.hasChild(prefix)) {
			makeBranch(start+1,end,n.getChild(prefix));
		}
		else if (leaf != null && leafPrefix.getValue().equals(prefix.getValue())) {
			leaf.startIndex += 1;
			// System.out.println(n);

			makeTree(leaf.startIndex+1,leaf.endIndex,n.addChild(leafPrefix));
		}
		else {
			// System.out.println(n);
			makeLeaf(start+1,end,n.addChild(prefix));
		}
	}
	
	public void makeLeaf(int start, int end, Node n) {
		Node suffix = new Node(start,end);
		n.addChild(suffix);
		
		n.setTerminalNode(true);
	}
	
	public void printTrie(Node pointer,int level) {
		level+=1;
		
		String p = null;
		if (pointer.parent == null) {
			p = "null";
		}
		else {
			p = pointer.parent.getValue().toString();
		}
		
		// ArrayList tValues = new ArrayList();
		// for (Node nd:pointer.children.values()) {
			// tValues.add(nd.getTerminalValue());
		// }
		
		// System.out.println("P: "+p+" L: "+level+" "+pointer.children.values()+" S: "+tValues);
		System.out.println("P: "+p+" L: "+level+" "+pointer.children.values());
		
		Collection c = pointer.children.values();
		Iterator<Node> iter = c.iterator();

		while (iter.hasNext()) {
			// System.out.println(iter.next());
			Node n = iter.next();
			
			printTrie(n,level);

		}
		
	}
	

 /** Method for finding the first occurrence of a pattern s in the DNA sequence */

	// public int findString(String s) {
	 
	// } 
 /** Method for computing the degree of similarity of two DNA sequences stored
 in the text files f1 and f2 */
	 // public static float similarityAnalyser(String f1, String f2, String f3) {
		 
	 // }
	 
	 public static void main(String[] args) {
		/** Construct a compact compressed suffix trie named trie1
		 */       
		CompactCompressedSuffixTrie trie1 = new CompactCompressedSuffixTrie("file1.txt");
		// CompactCompressedSuffixTrie trie2 = new CompactCompressedSuffixTrie("file2.txt");
		trie1.printTrie(trie1.root,0);
		
		// System.out.println("FB".hashCode()+" "+"Ea".hashCode());
		
		// System.out.println("ACTTCGTAAG is at: " + trie1.findString("ACTTCGTAAG"));

		// System.out.println("AAAACAACTTCG is at: " + trie1.findString("AAAACAACTTCG"));
				
		// System.out.println("ACTTCGTAAGGTT : " + trie1.findString("ACTTCGTAAGGTT"));
				
		// System.out.println(CompactCompressedSuffixTrie.similarityAnalyser("file2.txt", "file3.txt", "file4.txt"));
	 }
 
 }
 
 // class Edge {
	// Node child = null;
	// String value = null;
		
	// public Edge() {
			
	// }
	// public Edge(String v) {
		// this.value = v;
		// this.child = new Node();
	// }
		
	// @Override
	// public boolean equals(Object o) {
		// Edge n = (Edge) o;
		// return this.value.hashCode() == n.value.hashCode();
	// }
	
	// @Override	
	// public int hashCode() {
		// return this.value.hashCode();
	// }
		
	// public String toString() {
		// return this.value;
	// }

 // }
 
 class Node {
	Node parent = null;
	// String value = null;
	boolean terminalNode;
	int startIndex ;
	int endIndex ;
	
	public Node() {}
	
	public Node(int start, int end) {
		this.startIndex = start;
		this.endIndex = end;
		// this.child = new Node();
	}
	
	@Override
	public boolean equals(Object o) {
		Node n = (Node) o;
		return this.getValue().hashCode() == n.getValue().hashCode();
	}
	
	@Override	
	public int hashCode() {
		return this.getValue().hashCode();
	}
	
	@Override
	public String toString() {
		return this.getValue();
	}
	
	HashMap<Integer,Node> children = new HashMap<Integer,Node>();
	
	public String getValue() {
		// System.out.println(String.valueOf(startIndex)+" "+String.valueOf(endIndex)); 
		// System.out.println(CompactCompressedSuffixTrie.text.substring(startIndex,endIndex));
		return CompactCompressedSuffixTrie.text.substring(startIndex,endIndex);
	}
	
	public void setTerminalNode(boolean v) {
		this.terminalNode = v;
	} 
	
	public boolean isTerminalNode() {
		return this.terminalNode;
	} 
		
	public Node addChild(Node n) {
		// System.out.println(this);
		n.setParent(this);
		children.put(n.hashCode(),n);
		
		return n;
	}
	
	public void setParent(Node n) {
		this.parent = n;
	}
	
	public boolean hasChild(Node n) {
		return this.children.containsValue(n);
	}
	
	public Node getChild(int i) {
		return this.children.get(i);
	}
	
	public boolean hasChild() {
		return !this.children.isEmpty();
	}
	
	public Node getChild(Node n) {
		return this.children.get(n.hashCode());

	}
}