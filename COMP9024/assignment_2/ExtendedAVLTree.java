package net.datastructures;

import java.util.ArrayList;
import java.util.Comparator;
import java.util.LinkedList;

import javax.swing.*;

import java.awt.Graphics;


public class ExtendedAVLTree<K,V> extends AVLTree<K,V> implements Dictionary<K,V>{

	public static class TreeView<K, V> extends JFrame {
		private final AVLTree<K, V> tree;
		private int width;
		private int height;
		public TreeView(AVLTree<K, V> tree) {
			this.tree = tree;
			Position<Entry<K,V>> root = this.tree.root();
			height=tree.height(root);
			width = 2^tree.height(root);
			//
			this.setDefaultCloseOperation( JFrame.EXIT_ON_CLOSE );
			this.setSize(1600, 800);
			this.setVisible(true);
		}
		
		private void drawTree(Graphics g,Position<Entry<K,V>> node,int x, int y,int level) {
			
			int fix=0;
			if (height>3) {
				if (height>4) {
					if (level==-2) {
						fix=190;
					}
					if (level==-1) {
						fix=35;
					}
					if (level==0 ) {
						fix=-18;
					}
				}
				else {
					if (level==-2) {
						fix=105;
					}
					if (level==-1) {
						fix=-5;
					}
					if (level==0 ) {
						fix=-18;
					}
				}
			}

			int x_shift = 2^(fix+25*height-level*50);
			int y_shift = 75;
			
			if (node.element() != null) {
				g.drawOval(x,y, 50, 50);
				
				g.drawString(node.element().getKey().toString(), x+20, y+30);
			}
			else {
				g.drawRect(x, y, 50, 50);
			}
			
	
			if (tree.hasLeft(node)) {
				g.drawLine(x+25, y+50, x-x_shift+25, y+y_shift);

				drawTree(g,tree.left(node),x-x_shift,y+y_shift,level+1);
			}
			
			if (tree.hasRight(node)) {
				g.drawLine(x+25, y+50, x+x_shift+25, y+y_shift);

				drawTree(g,tree.right(node),x+x_shift,y+y_shift,level+1);
			}
			
			
		}

		/* (non-Javadoc)
		 * @see javax.swing.JComponent#paint(java.awt.Graphics)
		 */
		@Override
		public void paint(Graphics g) {
			super.paint(g);
			this.drawTree(g,tree.root(),625,50,-2);
			// use tree
			
		}
	}

	public ExtendedAVLTree() {super();}
	public ExtendedAVLTree(Comparator<K> c)  { super(c); }
	
	
	public static <K, V> AVLTree<K, V> clone(AVLTree<K,V> sourceTree) {
		ExtendedAVLTree<K,V> resultTree = new ExtendedAVLTree<K,V>();
		Position<Entry<K,V>> sourceRoot = sourceTree.root();
		Position<Entry<K,V>> resultRoot = resultTree.addRoot(sourceRoot.element());
		clone(sourceTree, resultTree, sourceRoot, resultRoot);
		resultTree.setHeight(resultRoot);
		return resultTree;
		
// 		This function creates a new AVL from the root of the sourceTree The private clone algorithm 
//		then uses the private clone helper function to copy the left node and insert it as the left
//		child of the current node (constant time) then calls itself on this new node, it does the same
//		for the right node (constant time). It recurses on itself n times, terminating each branch
//		when it has reached a null child node.
//		It visits each node in the sourceTree once, so this is O(n)
	}
	
	private static <K, V> void clone(
			AVLTree<K,V> sourceTree, ExtendedAVLTree<K,V> resultTree,
	        Position<Entry<K,V>> sourceNode, Position<Entry<K,V>> resultNode) {

		// left
		if (sourceTree.hasLeft(sourceNode)) {
			Position<Entry<K,V>> leftSourceNode = sourceTree.left(sourceNode);
			Position<Entry<K,V>> leftResultNode = resultTree.insertLeft(resultNode, leftSourceNode.element());
			clone(sourceTree, resultTree, leftSourceNode, leftResultNode);
		}
		// right
		if (sourceTree.hasRight(sourceNode)) {
			Position<Entry<K,V>> rightSourceNode = sourceTree.right(sourceNode);
			Position<Entry<K,V>> rightResultNode = resultTree.insertRight(resultNode, rightSourceNode.element());
			clone(sourceTree, resultTree, rightSourceNode, rightResultNode);
		}

		if (resultTree.hasRight(resultNode) && resultTree.hasLeft(resultNode)){
			resultTree.setHeight(resultNode);
		}

	}
	
	protected void inorderArray(Position<Entry<K,V>> v, LinkedList<Entry<K,V>> pos) 
		    throws InvalidPositionException {
		    if (hasLeft(v))
		    	inorderArray(left(v), pos); 
		    
		    if (v.element() != null) {
		    	pos.add(v.element());
		    }
		    
		    if (hasRight(v))
		    	inorderArray(right(v), pos); 
		  }


	public static <K, V> AVLTree<Integer, V>  merge(AVLTree<Integer,V> tree1, AVLTree<Integer,V> tree2 ) {
		ExtendedAVLTree<Integer,V> tree = new ExtendedAVLTree<Integer,V>();
		AVLTree<Integer,V> resultTree = new AVLTree<Integer,V>();
		
		LinkedList<Entry<Integer,V>> list1 = new LinkedList<Entry<Integer,V>>();
		LinkedList<Entry<Integer,V>> list2 = new LinkedList<Entry<Integer,V>>();
	

		tree.inorderArray(tree1.root, list1);
		tree.inorderArray(tree2.root, list2);

		ArrayList<Entry<Integer,V>> mergedList = mergeLists(list1,list2);
		
		int middle = mergedList.size()/2;
		
		Position<Entry<Integer, V>> resultRoot = resultTree.addRoot(mergedList.get(middle));
		buildAVL(resultTree,resultRoot, mergedList);
		resultTree.setHeight(resultRoot);		
		
		return resultTree;
		
//		1. This function first uses the inorderArray function to traverse each node of the 2 trees and input 
//		its entry, in order into a linked list. O(m+n)
//		2. It then uses the mergeList function to combine the 2 linked lists, into a new sorted list in constant time
//		by comparing the next element in each linked list and adding the smaller one. O(m+n)
//		3. It then uses the buildAVL method to build a new AVLTree from the elements in the linked list
//		in a similar manner to the one used in the clone method. O(m+n)
	}
	
	public static <K, V> void buildAVL(AVLTree<Integer,V> resultTree,Position<Entry<Integer, V>> node, ArrayList<Entry<Integer,V>> input) {
		
		int middle = input.size()/2;
		
		ArrayList<Entry<Integer,V>> leftList = new ArrayList<Entry<Integer,V>>(input.subList(0, middle)) ;
		ArrayList<Entry<Integer,V>> rightList = new ArrayList<Entry<Integer,V>>(input.subList(middle+1,input.size())) ;


		int left = leftList.size()/2;
		int right = rightList.size()/2;
		

		if (!leftList.isEmpty()) {
			Position<Entry<Integer, V>> leftNode = resultTree.insertLeft(node, input.get(left));
			buildAVL(resultTree,leftNode, leftList);

			}
			

		if (!rightList.isEmpty()) {
			Position<Entry<Integer, V>> rightNode = resultTree.insertRight(node, rightList.get(right));
			buildAVL(resultTree,rightNode, rightList);

		}

		if (resultTree.hasRight(node) && resultTree.hasLeft(node)){
			resultTree.setHeight(node);
		}
		

		
	}
		

	
	public static <K,V> ArrayList<Entry<Integer,V>> mergeLists(LinkedList<Entry<Integer,V>> list1, LinkedList<Entry<Integer,V>> list2) {
		ArrayList<Entry<Integer,V>> result = new ArrayList<Entry<Integer,V>>();
		

		while (list1 != null || list2 != null) {
			if (!list1.isEmpty() && !list2.isEmpty()) {
				
				if (list1.getFirst().getKey() < list2.getFirst().getKey()) {
					result.add(list1.removeFirst());
				}
				else {
					result.add(list2.removeFirst());
				}
			}
			else if (list1.isEmpty() && list2.isEmpty()) {
				break;
			}
			else if (list1.isEmpty()) {
				result.add(list2.removeFirst());
			}
			else if (list2.isEmpty()) {
				result.add(list1.removeFirst());
			}
		}

		return result;
		
	}
	
	
	
	
	public static <K, V> void print(AVLTree<K, V> tree) {
		new TreeView<K, V>(tree);
	}
	
	
	public static void main(String[] args)
    { 
      String values1[]={"Sydney", "Beijing","Shanghai", "New York", "Tokyo", "Berlin",
     "Athens", "Paris", "London", "Cairo"}; 
      int keys1[]={20, 8, 5, 30, 22, 40, 12, 10, 3, 5};
      String values2[]={"Fox", "Lion", "Dog", "Sheep", "Rabbit", "Fish"}; 
      int keys2[]={40, 7, 5, 32, 20, 30};
         
      /* Create the first AVL tree with an external node as the root and the
     default comparator */ 
         
        AVLTree<Integer, String> tree1=new AVLTree<Integer, String>();

      // Insert 10 nodes into the first tree
         
        for ( int i=0; i<10; i++)
            tree1.insert(keys1[i], values1[i]);
       
      /* Create the second AVL tree with an external node as the root and the
     default comparator */
         
        AVLTree<Integer, String> tree2=new AVLTree<Integer, String>();
       
      // Insert 6 nodes into the tree
         
        for ( int i=0; i<6; i++)
            tree2.insert(keys2[i], values2[i]);
         
        ExtendedAVLTree.print(tree1);
        ExtendedAVLTree.print(tree2); 
        ExtendedAVLTree.print(ExtendedAVLTree.clone(tree1));
        ExtendedAVLTree.print(ExtendedAVLTree.clone(tree2));
        
        ExtendedAVLTree.print(ExtendedAVLTree.merge(ExtendedAVLTree.clone(tree1), 
        ExtendedAVLTree.clone(tree2)));
      }








}
