import java.io.*;
import java.util.*;



public class CompactCompressedSuffixTrie {

/** You need to define your data structures for the compressed trie */
/** Constructor */
	private Node root;
	protected String text;

  // Create a compact compressed suffix trie from file f
	public String readFile(String f) {
		Scanner input = null;
		String out = null;
		
		try {
			input = new Scanner(new File(f));
		}
		catch (FileNotFoundException e) {
			 System.err.println(f+" does not exist");
			 System.exit(0);
		}
		
		while (input.hasNextLine()) {
			out = input.nextLine();
			// System.out.println(text);
		}
		return out;
	}

	public CompactCompressedSuffixTrie(String f) {
		this.root = new Node();
		
		this.text = readFile(f);
	
		makeTree(0,this.text.length(),this.root);
		this.root.setTerminalNode(true);
	}
	
	public void makeTree(int start, int end, Node n) {
		while (!(start == end)) {
			// System.out.println(text.substring(start,end));
			makeBranch(start,end,n);
			start += 1;
		}
	}
	

	public void makeBranch(int start, int end, Node n) {
		
		if (start == end+1) {
//			n.setTerminalNode(true);
			// return;
		}
		
//		if (n == null) {
//			return;
//		}

		Node prefix = new Node(start,start+1);
//		System.out.println(n+" "+prefix);
		// System.out.println(prefix.getValue());
		// String suffix = s.substring(1);
		
		Node leaf = null;
//		Node leafPrefix = null;
		int offset=0;
		// String leafSuffix = null;
//		System.out.println(n+" "+n.children.values());
		for (Node l: n.children.values()) {
			// System.out.println(prefix+" "+l);
			// if (l.isTerminalNode()  ) {
			if (this.text.substring(l.startIndex,l.startIndex+1).equals(prefix.getValue()) ) {
				int i = l.startIndex+1;
				int j = start+1;
				while (this.text.substring(l.startIndex,i).equals(this.text.substring(start,j))) {
//					System.out.println(this.text.substring(l.startIndex,i)+" "+this.text.substring(start,j));

					offset = i - l.startIndex;
					if (i >= this.text.length() || j >= this.text.length()) {
						
						break;
					}
					i++;
					j++;
//					System.out.println(this.text.substring(l.startIndex,i));
				}
				// System.out.println("here");
				leaf = l;
//				leafPrefix = new Node(leaf.startIndex,leaf.startIndex+1);
//				leafPrefix = new Node(leaf.startIndex,j);
				break;
			}
		}
		
//		if (n.hasChild(leaf) && leaf.getValue().equals(text.substring(leaf.startIndex,leaf.startIndex+offset))) {
//			System.out.println(n.children.values()+" "+n);
//			System.out.println(leaf+"here");
//			makeBranch(start+offset,end,n.getChild(leaf));
//		}
//		else 
			if (leaf != null) {
			
			// System.out.println("split");
			System.out.println("off"+offset+" "+start+" trying: "+text.substring(start,end));
			Node next = splitNode(offset,leaf);
			
			System.out.println("here"+" "+this.text.substring(start+1, end));
			makeLeaf(start+offset,end,leaf);

//			makeTree(leaf.startIndex+1,leaf.endIndex,n.addChild(leafPrefix));
		}
		else {
//			System.out.println("here"+" "+this.text.substring(start, end));
			makeLeaf(start,end,n);
		}
	}
	
	public Node splitNode(int pos, Node n) {
		System.out.println("In: "+n+" at: "+pos);
		System.out.println(n.children.values());
		
		HashMap<Integer,Node> nChildren =  n.clearChildren();
		Node ret = n.addChild(new Node(n.startIndex+pos,n.endIndex));
		ret.setChildren(nChildren);
		
		System.out.println(n.children.values());
		
		n.endIndex = n.startIndex +pos;
		n.setTerminalNode(true);
		ret.setTerminalNode(true);
		System.out.println("Out: "+n+" "+ret);
//		ret.setTerminalNode(false);
		
		return ret;
	}
	
	public void makeLeaf(int start, int end, Node n) {
		if (start==end) {
			return;
		}
		Node suffix = new Node(start,end);
		n.addChild(suffix);
		suffix.setTerminalNode(true);
		
	}
	
	public void printTrie(Node pointer,int level) {
		level+=1;
		
		// String p = null;
		// if (pointer.parent == null) {
			// p = "null";
		// }
		// else {
			// p = pointer.parent.getValue().toString();
		// }
		
		ArrayList<Boolean> tValues = new ArrayList<Boolean>();
		for (Node nd:pointer.children.values()) {
			tValues.add(nd.isTerminalNode());
		}
		
		// System.out.println("P: "+p+" L: "+level+" "+pointer.children.values()+" S: "+tValues);
		System.out.println("L: "+level+" "+pointer.children.values()+" T:"+tValues);
		
		Collection<Node> c = pointer.children.values();
		Iterator<Node> iter = c.iterator();

		while (iter.hasNext()) {
			// System.out.println(iter.next());
			Node n = iter.next();
			
			printTrie(n,level);

		}
		
	}
	
	class Node {
		// Node parent = null;
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
			return text.substring(startIndex,endIndex);
		}
		
		public void setTerminalNode(boolean v) {
			this.terminalNode = v;
		} 
		
		public boolean isTerminalNode() {
			return this.terminalNode;
		} 
			
		public Node addChild(Node n) {
			// System.out.println(this);
			// n.setParent(this);
			children.put(n.hashCode(),n);
			
			return n;
		}
		
		// public void setParent(Node n) {
			// this.parent = n;
		// }
		
		public boolean hasChild(Node n) {
			return this.children.containsValue(n);
		}
		
		// public Node getChild(int i) {
			// return this.children.get(i);
		// }
		
		public boolean hasChild() {
			return !this.children.isEmpty();
		}
		
		public Node getChild(Node n) {
			return this.children.get(n.hashCode());
		}
		
		public HashMap<Integer,Node> getChildren() {
			return this.children;
		}
		
		public HashMap<Integer,Node> clearChildren() {
			HashMap<Integer,Node> ret = this.children;
			this.children = new HashMap<Integer,Node>();
			return ret;
		}
		
		public void setChildren(HashMap<Integer,Node> newChildren) {
			this.children = newChildren;
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
//		CompactCompressedSuffixTrie trie2 = new CompactCompressedSuffixTrie("file2.txt");
		trie1.printTrie(trie1.root,0);
//		System.out.println("==========");
//		trie1.printTrie(trie2.root,0);
		
		// System.out.println("FB".hashCode()+" "+"Ea".hashCode());
		
		// System.out.println("ACTTCGTAAG is at: " + trie1.findString("ACTTCGTAAG"));

		// System.out.println("AAAACAACTTCG is at: " + trie1.findString("AAAACAACTTCG"));
				
		// System.out.println("ACTTCGTAAGGTT : " + trie1.findString("ACTTCGTAAGGTT"));
				
		// System.out.println(CompactCompressedSuffixTrie.similarityAnalyser("file2.txt", "file3.txt", "file4.txt"));
	 }
 
 }
 
 