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
			if (out == null) {
				out = input.nextLine();
			}
			else {
				out += input.nextLine();
			}
			
			// System.out.println(text);
		}
		System.out.println(out);
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
			makeBranch(start,end,n);
			start += 1;
		}
	}
	
	public class Pair<A,B> {
	    public final A a;
	    public final B b;

	    public Pair(A a, B b) {
	        this.a = a;
	        this.b = b;
	    }
	};
	
	public Pair<Node, Integer> findChildAndOffset(int start, Node n) {
		Node leaf = null;
		int offset=0;
		for (Node l: n.children.values()) {
			if (this.text.substring(l.startIndex,l.startIndex+1).equals(this.text.substring(start, start+1)) ) {
				int i = l.startIndex+1;
				int j = start+1;
				while (this.text.substring(l.startIndex,i).equals(this.text.substring(start,j))) {
					offset = i - l.startIndex;
					if (i >= this.text.length() || j >= this.text.length()) {
						break;
					}
					i++;
					j++;
				}
				if (leaf == null || l.toString().length() >= leaf.toString().length()) {
					leaf = l;
				}
				
//				System.out.println("found: "+l);
				
				break;
			}
		}
		return new Pair<Node, Integer>(leaf,offset);
	}

	public void makeBranch(int start, int end, Node n) {
		int offset=0;
		Pair<Node,Integer> child = findChildAndOffset(start,n);
		Node leaf = child.a;
		offset = child.b;
//		System.out.println(this.text.substring(start, end)+" leaf "+leaf);
		

		if (leaf != null) {
			System.out.println("adding: "+this.text.substring(start, end)+" to "+leaf.toString());
			
			int textRun = start+1;
			int leafRun = 1;
			
//			System.out.println("STRING"+leaf.toString());
			
			while (textRun < this.text.length() && leafRun < leaf.toString().length()+1  && this.text.substring(start, textRun).equals(leaf.toString().substring(0, leafRun))) {
//				System.out.println(this.text.substring(start, textRun)+" "+leaf.toString().substring(0, leafRun));
				
				leafRun++;
				textRun++;
			}
//			System.out.println("adding: "+this.text.substring(start, end)+" to "+leaf.toString());

			boolean moveCondition = leaf.toString().length() == leafRun-1 && 
									this.text.substring(start, end).length() != 1;	
			if (moveCondition ) {
				Node nextLeaf = leaf;
				int nextOffset=0;
				int nextSubstringOffset = 1;
				while (true) {
//					System.out.println(n.children.values());
					System.out.println("========here========="+nextLeaf+" s: "+start+nextSubstringOffset);
					
					Pair<Node,Integer> nextChild=null;
					try {
						nextChild = findChildAndOffset(start+nextSubstringOffset,nextLeaf);
						System.out.println("here!!"+nextLeaf);
					}
					catch (StringIndexOutOfBoundsException e) {
						nextChild = findChildAndOffset(nextSubstringOffset,nextLeaf);
					}
					
					System.out.println("====="+nextSubstringOffset);
					
					if (nextChild.a == null) {
						leaf = nextLeaf;
						offset = nextOffset;
						nextSubstringOffset = 1;
						break;
					}
					nextLeaf = nextChild.a;
					nextOffset = nextChild.b;
							
					nextSubstringOffset+=nextLeaf.toString().length();
				}
//				System.out.println("out "+leaf+" leafrun "+leafRun);
				if (!(leaf.toString().length() == leafRun-1)) {
					System.out.println("leaf: "+leaf);
					splitNode(offset,leaf);
					
				}
				else {
					System.out.println(this.text.substring(start+nextSubstringOffset,end));
//					leaf.addChild(new Node(start+nextSubstringOffset,end));
					if (!leaf.children.values().contains(new Node(start+nextSubstringOffset,end))) {
						makeLeaf(start+nextSubstringOffset,end,leaf);
					}
					
//					System.out.println("==================================="+leaf);
				}
			}
			else {
				System.out.println("here-"+leaf+" "+this.text.substring(start+offset,end));
				Node nextNode = null;
				if (!(leaf.toString().length() == 1)) {
					nextNode=splitNode(offset,leaf);
				}
				
				makeLeaf(start+offset,end,nextNode);
//				System.out.println("adding: "+this.text.substring(start, end)+" to "+leaf.toString());
//				System.out.println(leaf.children.values());
			}
			

		}
		else {
			makeLeaf(start,end,n);
		}
//		printTrie(this.root,0);
	}
	
	public Node splitNode(int pos, Node n) {
		HashMap<Integer,Node> nChildren =  n.clearChildren();
		int start = n.startIndex;
		int end = n.endIndex;
		Node parent = n.parent;
		
	
		n.parent.removeChild(n);
		n = new Node(start,start +pos);		
		n.setParent(parent);
		
		n.parent.addChild(n);
//		System.out.println(pos+" splitting: "+n);
		
		Node addNode = new Node(start+pos,end);
//		System.out.println("===========added node: "+addNode+" to: "+n);
		Node ret = n.addChild(addNode);
//		System.out.println("done");
		ret.setChildren(nChildren);
		ret.updateChildren();
		
//		n.endIndex = n.startIndex +pos;
		n.setTerminalNode(true);
		ret.setTerminalNode(true);
		
		return n;
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

		ArrayList<Node> parents = new ArrayList<Node>();
		for (Node nd:pointer.children.values()) {
			parents.add(nd.parent);
		}
		
		System.out.println("L: "+level+" "+pointer.children.values()+" T:"+parents);
		
		Collection<Node> c = pointer.children.values();
		Iterator<Node> iter = c.iterator();

		while (iter.hasNext()) {
			Node n = iter.next();
			printTrie(n,level);
		}
		
	}
	
	class Node {
		boolean terminalNode;
		Node parent = null;
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

			return text.substring(startIndex,endIndex);
		}
		
		public void setTerminalNode(boolean v) {
			this.terminalNode = v;
		} 
		
		public boolean isTerminalNode() {
			return this.terminalNode;
		} 
			
		public Node addChild(Node n) {
			children.put(n.hashCode(),n);
			n.parent=this;
			return n;
		}
		
		public Node removeChild(Node n) {
			return this.children.remove(n.hashCode());
		}

		public boolean hasChild(Node n) {
			return this.children.containsValue(n);
		}
		
		public boolean hasChild() {
			return !this.children.isEmpty();
		}
		
		public Node getChild(Node n) {
//			System.out.println("getChild: "+n.hashCode());
//			for (Node x: this.children.values()) {
//				 System.out.println(x+" "+x.hashCode());
//			}
			return this.children.get(n.hashCode());
		}
		
		public Node getChild(String s) {
//			System.out.println("getChild: "+s.hashCode());
//			for (int x: this.children.keySet()) {
//				 System.out.println(x+" "+x);
//			}
			return this.children.get(s.hashCode());
		}
		
		public HashMap<Integer,Node> getChildren() {
			return this.children;
		}
		
		public HashMap<Integer,Node> clearChildren() {
			HashMap<Integer,Node> ret = this.children;
			this.children = new HashMap<Integer,Node>();
			return ret;
		}
		
		public void setParent(Node n) {
			this.parent = n;
		}
		
		public void updateChildren() {
			for (Node c:this.children.values()) {
				c.setParent(this);
			}
		}
		
		public void setChildren(HashMap<Integer,Node> newChildren) {
			this.children = newChildren;
		}
		

	}
	

 /** Method for finding the first occurrence of a pattern s in the DNA sequence */

	 public int findString(String s) {
		 int stringPos = 0;
		 int stringSearch = 1;
		 Integer origin = null;
		 Node treePos = this.root;
		 Node nextPos = this.root;
		 
		 
		 while (stringPos != s.length()) {
			 while (stringSearch-stringPos != s.length() ) {
				 System.out.println(treePos.children.values()+" "+s.substring(stringPos, stringSearch)+stringPos+" "+stringSearch);
				 
				 nextPos = treePos.getChild(s.substring(stringPos, stringSearch));

				 if (nextPos != null) {
					 
					 stringPos = stringSearch;
					 stringSearch=stringPos+1;
					 treePos = nextPos;
					 System.out.println(stringPos+" "+stringSearch);
					 
					 if (origin == null) {
						 origin=nextPos.startIndex;
					 }
					 System.out.println("here");
					 break;
				 }
				 
				 
				 stringSearch++;
			 }
			 
			 if (stringSearch-stringPos==s.length()) {
				 return -1;
			 }
			 
		 }
		 return origin;
	 } 
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
//		System.out.println("ANANA is at: " + trie1.findString("ANANA"));

		
		// System.out.println("FB".hashCode()+" "+"Ea".hashCode());
		
//		 System.out.println("ACTTCGTAAG is at: " + trie1.findString("ACTTCGTAAG"));

		// System.out.println("AAAACAACTTCG is at: " + trie1.findString("AAAACAACTTCG"));
				
		// System.out.println("ACTTCGTAAGGTT : " + trie1.findString("ACTTCGTAAGGTT"));
				
		// System.out.println(CompactCompressedSuffixTrie.similarityAnalyser("file2.txt", "file3.txt", "file4.txt"));
	 }
 
 }
 
 