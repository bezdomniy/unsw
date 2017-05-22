package treasureHunt;
/*********************************************
 *  Agent.java 
 *  Sample Agent for Text-Based Adventure Game
 *  COMP3411 Artificial Intelligence
 *  UNSW Session 1, 2017
*/

import java.util.*;
import java.util.Map.Entry;
import java.util.stream.Collectors;
import java.io.*;
import java.net.*;


public class Agent {
	int c=0;
	
   private boolean have_axe     = false;
   private boolean have_key     = false;
   private boolean have_treasure= false;
   private boolean on_water = false;
   private boolean have_raft    = false;
   private int num_dynamites_held = 0;
   
   private boolean newItems = false;
   private boolean failedToFindPathToObjective = false;
   private boolean embarkingOrDisembarking = false;
   
   private int numberOfTrees = 0;
   private int numberOfAxes = 0;
   private int numberOfDynamite = 0;
   private int numberOfKeys = 0;
   private int numberOfTreasures = 0;
	
   private Map<Character, Integer> symbolValues = new HashMap<Character, Integer>();
//   private List<List<Position>> world = new ArrayList<List<Position>>();
   
   private WorldState world;
   
   private int orientation = 0;
   private char[] orientationSymbol = {'^','>','v','<'};
   
   private int lastMoveSuccessful = 0;
//   private char lastMove = ' ';
   
   private Position objective = new Position(2,2);
   List<Position> objectives = new ArrayList<Position>();
   
   private List<Character> nextMoves = new ArrayList<Character>();
   Set<Character> obstacles = new HashSet<Character>();
   
   // not used
   Set<Character> removable = new HashSet<Character>();
   
   private Position worldPosition = new Position(2,2);
   
   
   private int[] positionRelativeToStart = {0,0};
   private int[] expandedUpRightDownLeft = {0,0,0,0};
      
      
   private Map<Integer, Integer[]> forward = new HashMap<Integer,Integer[]>();
   
   public Agent() {
	   symbolValues.put('a', 0);
	   symbolValues.put('d', 0);
	   symbolValues.put('k', 0);
	   symbolValues.put('$', 2);
	   
	   symbolValues.put(' ', 0);
	   symbolValues.put('#', 0);
	   
//	   symbolValues.put('T', -1);
//	   symbolValues.put('*', -1);
//	   symbolValues.put('-', -1);
//	   symbolValues.put('~', -1);
//	   symbolValues.put('.', -1);
	   
	   
	   forward.put(0, new Integer[]{-1,0});
	   forward.put(1, new Integer[]{0,1});
	   forward.put(2, new Integer[]{1,0});
	   forward.put(3, new Integer[]{0,-1});
	   
	   obstacles.add('-');
	   obstacles.add('*');
	   obstacles.add('T');
	   obstacles.add('~');
//	   obstacles.add('#');
	   obstacles.add('.');
	   
	   List<List<Position>> w = new ArrayList<List<Position>>();
	   
	   for(int i = 0; i < 5; i++)  {
		   List<Position> nextRow = new ArrayList<Position>(5);
		   w.add(i, nextRow);
	   }
	   world = new WorldState(w);
	   
   }
   
   public class Position {
	   private int row;
	   private int col;
	   private Character value;
	   private boolean reachable = false;
	   private double exploreScore;
	   
	   public Position(int row, int col) {
	       this.row = row;
	       this.col = col;
	    }
	   
	   public Position(int row, int col,Character value) {
	       this.row = row;
	       this.col = col;
	       this.value = value;
	       
	    }
	   
	   public Position(Character value) {
	       this.value = value;
	    }
	   
	   public Position clone() {
		   Position ret  = new Position(this.row,this.col,this.value);
		   ret.setReachable(this.reachable);
		   return ret;
	   }
	   
	   @Override
	   public boolean equals(Object object) {
		   return this.row == ((Position) object).getRow() && this.col == ((Position) object).getCol();
		   
	   }
	   
	   @Override
	    public int hashCode() {
	        return Objects.hash(this.row,this.col);
	    }
	   
	   @Override
	   public String toString() {
		   return String.format("["+this.row+","+this.col+"]");
		   
	   }
	   

	   public boolean isReachable() {
		   return this.reachable;
	   }
	   
	   public void setReachable(boolean b) {
		   this.reachable = b;
	   }
	   
	   public char getValue() {
		   return this.value;
	   }

	   public void setValue(Character value) {
		   this.value = value;
//		   world.get(this.row).get(this.col).setValueAt(value);
	   }
	   
	   public Position north() {
		   if (this.getRow()-1 >= 0) {
//			   return new Position(this.getRow()-1,this.getCol());
			   return world.getCell(this.getRow()-1, this.getCol());
		   }
		   return null;
	   }
	   
	   public Position south() {
		   if (this.getRow()+1 < world.numRows()) {
//			   return new Position(this.getRow()+1,this.getCol());
			   return world.getCell(this.getRow()+1, this.getCol());
		   }
		   return null;
	   }
	   
	   public Position east() {
		   if (this.getCol()+1 < world.numCols()) {
//			   return new Position(this.getRow(),this.getCol()+1);
			   return world.getCell(this.getRow(), this.getCol()+1);
		   }
		   return null;
	   }
	   
	   public Position west() {
		   if (this.getCol()-1 >= 0) {
//			   return new Position(this.getRow(),this.getCol() - 1);
			   return world.getCell(this.getRow(), this.getCol()-1);
		   }
		   return null;
	   }
	   
	   
	   public void setRow(int row) {
	       this.row = row;
	   }

	   public void setCol(int col) {
	       this.col = col;
	   }
	   
	   public int getRow() {
	       return row;
	   }

	   public int getCol() {
	       return col;
	   }
	   
	   public double getExploreScore() {
		   return this.exploreScore;
	   }
	   
	   public double findExploreScore() {
//		   System.out.println(this);
		   this.exploreScore = 0d;
		   if (world != null) {
			   for (int i = this.getRow() - 2; i <= this.getRow() + 2; i++) {
				   for (int j = this.getCol() - 2; j <= this.getCol() + 2; j++) {
					   if (i == 0 || i == world.numRows() -1 || i == world.numRows() -2) {
						   long boundaryTiles = world.getRow(i).stream().map(Position::getValue).filter(c -> '.' == c).count();
						   if (boundaryTiles > 4d) {
							   i++;
							   break;
						   }
						   
					   }
					   
					   
					   if (i < 0 || i >= world.numRows()) {
//						   System.out.println("adding 5");
						   this.exploreScore+=5d;
						   break;
					   }
					   else {
						   
					   }

					   if (j < 0 || j >= world.numCols()) {
//						   System.out.println("adding 1 for col");
						   this.exploreScore+=1d;
					   }
					   else if (world.getCell(i, j).getValue() == '#') {
//						   System.out.println("adding 1 for hash");
						   this.exploreScore+=1d;
					   }
				   }
			   }
		   }
		   
		   if (this.value == 'd' || (this.value == 'a' && !have_axe) || (this.value == 'k' && !have_key)) {
			   System.out.println(this+" "+this.value+" marker");
			   this.exploreScore += 2;
		   }
		   
		   return this.exploreScore / (double) manDistTo(worldPosition,this,orientation);
	   }
	   
	   
   }
   
   public class WorldState {
	   private List<List<Position>> worldModel = new ArrayList<List<Position>>();
	   
	   
	   public WorldState(WorldState parentWorld) {
		   for (int i = 0; i < parentWorld.numRows(); i++) {
			   worldModel.add(new ArrayList<Position>());
			   
			   for (int j = 0 ; j < parentWorld.numCols(); j++) {
				   worldModel.get(i).add(parentWorld.worldModel.get(i).get(j).clone());
			   }
		   }
	   }
	   
	   public WorldState(List<List<Position>> parentWorld) {
		   for (int i = 0; i < parentWorld.size(); i++) {
//			   System.out.println(parentWorld.get(0).size());
			   worldModel.add(new ArrayList<Position>());
			   
			   for (int j = 0 ; j < parentWorld.get(0).size(); j++) {
//				   System.out.println(j);
//				   System.out.println(parentWorld.get(0).size());
				   this.worldModel.get(i).add(parentWorld.get(i).get(j).clone());
			   }
		   }
//		   System.out.println('f');
	   }
	   
	   public WorldState() {
		   
	   }
	   
	   public WorldState clone() {
		   return new WorldState(this);
	   }
	   
	   public boolean updateCell(int row, int col, Character symbol) {
		   try {
			   worldModel.get(row).get(col).setValue(symbol);
			   return true;
		   }
		   catch (IndexOutOfBoundsException e) {
			   return false;
		   }
		   
	   }
	   
	   @Override
	   public boolean equals(Object object)
	   {
		   try {
			   if (object != null && object instanceof WorldState) {
		    	   for (int i = 0; i < this.numRows(); i++) {
					   for (int j = 0 ; j < this.numCols(); j++) {
						   if (this.getCell(i, j).getValue() != ((WorldState) object).getCell(i, j).getValue()) {
							   return false;
						   }

					   }
				   }
		       }
		   }
		   catch (IndexOutOfBoundsException e) {
			   return false;
		   }

	       return true;
	   }
	   
	   @Override
	    public int hashCode() {
		   StringBuilder str = new StringBuilder();
		   
		   
		   for (int i = 0; i < this.numRows(); i++) {
			   for (int j = 0 ; j < this.numCols(); j++) {
				   str.append(this.getCell(i, j).getValue());

			   }
		   }
		   
		   str.toString();
		   
	        return Objects.hash(str.toString());
      
	    }
	   
	   public Position getCell(int row, int col) {
		   try {
			   
			   return worldModel.get(row).get(col);
		   }
		   catch (IndexOutOfBoundsException e) {
			   return (Position) null;
		   }
		   
	   }
	   
	   public List<Position> getRow(int row) {
		   return this.worldModel.get(row);
		   
	   }
	   
	   public void addPosition(int row, int col, Position pos) {
		   this.worldModel.get(row).add(col,pos);
	   }
	   
	   public void addPosition(int row, Position pos) {
		   this.worldModel.get(row).add(pos);
	   }
	   
	   public void addRow(char edge, List<Position> row) {
		   if (edge == 'T') {
			   this.worldModel.add(0,row);
		   }
		   else if (edge == 'B') {
			   this.worldModel.add(row);
		   }
		   
	   }
	   
	   
	   public int numRows() {
		   return this.worldModel.size();
		   
	   }
	   
	   public int numCols() {
		   return this.worldModel.get(0).size();
	   }
	   
	   public void print() {
		      int i,j;
		      
		      char[] dashArray = new char[this.numCols()];
		      Arrays.fill(dashArray, '-');
		      String dashes = new String(dashArray);
		      
		      System.out.println("\n+"+dashes+"+");
		      for( i=0; i < this.numRows(); i++ ) {
		         System.out.print("|");
		         for( j=0; j < this.numCols(); j++ ) {
		            if(( i == worldPosition.getRow() )&&( j == worldPosition.getCol() )) {
		               System.out.print(orientationSymbol[orientation]);
		            }
		            else {
		               System.out.print( getCell(i,j).getValue() );
		            }
		         }
		         System.out.println("|");
		      }
		      System.out.println("+"+dashes+"+");
	   }
	   
	   
   }
   
   public class Node {
	   private Position position;
	   private WorldState nodeWorld;
	   private Set<Character> nodeObstacles = new HashSet<Character>();
	   
	   private boolean haveAxe     = have_axe;
	   private boolean haveKey     = have_key;
	   private boolean haveTreasure= have_treasure;
	   private boolean onWater = on_water;
	   private boolean haveRaft    = have_raft;
	   private int numDynamitesHeld = num_dynamites_held;
	   

	   private int g;
	   private int h;
	   private float f;
	   private int orientation;
	   private Character symbol;
	   
	   private Character moveToReach = null;
	   
	   private List<Node> children = new ArrayList<Node>();
	   
	   public Node(Position position, Character symbol, int orientation, Node parent) {
		   this.position = position;
		   this.symbol = symbol;
		   this.orientation = orientation;
//		   this.value = position.getValueAt();
		   
		   if (parent != null) {
			   this.haveAxe = parent.haveAxe;
			   this.haveKey = parent.haveKey;
			   this.haveTreasure = parent.haveTreasure;
			   this.onWater = parent.onWater;
			   this.haveRaft = parent.haveRaft;
			   this.numDynamitesHeld = parent.numDynamitesHeld;
			   this.nodeWorld = parent.nodeWorld.clone();
			   
			   for (Character c: parent.nodeObstacles) {
				   this.nodeObstacles.add(c);
			   }
//			   this.nodeObstacles.add('#');
		   }
		   else {
			   this.nodeWorld = new WorldState(world);
			   
			   for (Character c: obstacles) {
				   this.nodeObstacles.add(c);
			   }
//			   this.nodeObstacles.add('#');
		   }

	   }
	   
	   @Override
	   public boolean equals(Object object)
	   {
	       if (object != null && object instanceof Node)
	       {
	    	   if (this.getForwardPosition() != null) {
	    		   
//	    		   System.out.println(this.getForwardPosition() +" "+ ((Node) object).getForwardPosition());
//	    		   System.out.println(this.getForwardPosition().getValueAt() +" "+ ((Node) object).getForwardPosition().getValueAt());
//	    		   System.out.println(this.getMove() +" "+ ((Node) object).getMove());
//	    		   System.out.println(this.nodeWorld.getCell(2, 4) +" "+ ((Node) object).nodeWorld.getCell(2, 4));
	    		   
	    		   return this.position.equals(((Node) object).getPosition()) && this.orientation == ((Node) object).getOrientation() 
	    				   && this.nodeWorld.equals(((Node) object).nodeWorld);
	    				   
	    				   
//		        		   && this.nodeWorld.getCell(this.getForwardPosition().getRow(),this.getForwardPosition().getCol()) 
//		        		   == ((Node) object).nodeWorld.getCell(this.getForwardPosition().getRow(),this.getForwardPosition().getCol());
	    		   
	    		   
	    	   }
	           return this.position.equals(((Node) object).getPosition()) && this.orientation == ((Node) object).getOrientation();
	       }

	       return false;
	   }
	   
	   @Override
	    public int hashCode() {
		   if (this.getForwardPosition() != null) {
//			   return Objects.hash(this.position,this.orientation,this.nodeWorld.getCell(this.getForwardPosition().getRow(),this.getForwardPosition().getCol()));
			   return Objects.hash(this.position,this.orientation,this.nodeWorld);
		   }
	        return Objects.hash(this.position,this.orientation);
       
	    }
	   
	   @Override
	   public String toString() {
		   return String.format("Pos: "+this.position.getRow()+" "+this.position.getCol()+", Ori: "+this.orientation);
		   
	   }
	   
	   public Position getForwardPosition() {
		   Position forwardPosition = new Position(this.position.getRow()+forward.get(this.orientation)[0],this.position.getCol()+forward.get(this.orientation)[1]);
		   if (forwardPosition.getRow() >= 0 && forwardPosition.getCol() >= 0 && forwardPosition.getRow() < this.nodeWorld.numRows() && forwardPosition.getCol() < this.nodeWorld.numCols()) {
			   return forwardPosition;
		   }
		   return null;
	   }
	   
	   public Position getBackwardPosition() {
		   Position backwardPosition = new Position(this.position.getRow()-forward.get(this.orientation)[0],this.position.getCol()-forward.get(this.orientation)[1]);
		   if (backwardPosition.getRow() >= 0 && backwardPosition.getCol() >= 0 && backwardPosition.getRow() < this.nodeWorld.numRows() && backwardPosition.getCol() < this.nodeWorld.numCols()) {
			   return backwardPosition;
		   }
		   return null;
	   }
	   
//	   public void setValue(Character c) {
//		   this.value = c;
//	   }
	   
	   public void setHaveAxe(boolean bool) {
		   this.haveAxe = bool;
	   }
	   
	   public void setHaveKey(boolean bool) {
		   this.haveKey = bool;
	   }
	   
	   public void setHaveRaft(boolean bool) {
		   this.haveRaft = bool;
	   }
	   
	   public void setOnWater(boolean bool) {
		   this.onWater = bool;
	   }
	   
	   public void setHaveTreasure(boolean bool) {
		   this.haveTreasure = bool;
	   }
	   
	   public void addDynamite() {
		   this.numDynamitesHeld++;
	   }
	   
	   public void removeDynamite() {
		   this.numDynamitesHeld--;
	   }
	   
	   
	   public void setMove(Character c) {
		   this.moveToReach = c;
	   }
	   
	   public Character getMove() {
		   return this.moveToReach;
	   }
	   
	   public void setH(int h) {
		   this.h = h;
	   }
	   
	   public void setG(int g) {
		   this.g = g;
	   }
	   
	   public void updateF() {
		   this.f = this.g + this.h;
	   }
	   
	   public void updateF(float w) {
		   this.f = (2 - w) * this.g +  w * this.h;
	   }
	   
  
	   public int getH() {
		   return this.h;
	   }
	   
	   public int getG() {
		   return this.g;
	   }
	   
	   public float getF() {
		   return this.f;
	   }
	   
	   public char getSymbol() {
		   return this.symbol;
	   }
	   
	   public Position getPosition() {
		   return this.position;
	   }
	   
	   public void setPosition(Position position) {
		   this.position = position;
	   }
	   
	   public void setPosition(int row, int col) {
		   this.position = new Position(row,col);
	   }
	   	   
	   public int getOrientation() {
		   return this.orientation;
	   }
	   
	   public void setOrientation(int o) {
		   this.orientation = o;;
	   }
	   
	   public void addChild(Node node) {
		   this.children.add(node);
	   }
	   
	   public List<Node> getChildren() {
		   return this.children;
	   }
	   
	   public void removeObstacle(Character o) {
		   this.nodeObstacles.remove(o);
	   }
	   
	   public void addObstacle(Character o) {
		   this.nodeObstacles.add(o);
	   }
	   
	   public void printWorld() {
		      int i,j;
		      
		      char[] dashArray = new char[this.nodeWorld.numCols()];
		      Arrays.fill(dashArray, '-');
		      String dashes = new String(dashArray);
		      
		      System.out.println("\n+"+dashes+"+");
		      for( i=0; i < this.nodeWorld.numRows(); i++ ) {
		         System.out.print("|");
		         for( j=0; j < this.nodeWorld.numCols(); j++ ) {
		            if(( i == this.getPosition().getRow() )&&( j == this.getPosition().getCol() )) {
//		            	System.out.print("X");
		               System.out.print(orientationSymbol[this.orientation]);
		            }
		            else {
		               System.out.print( this.nodeWorld.getCell(i,j).getValue() );
		            }
		         }
		         System.out.println("|");
		      }
		      System.out.println("+"+dashes+"+");
	   }
	   
	   
	   
   }
   
   public class fScoreComparator implements Comparator<Node> {  
	@Override
	public int compare(Node n1, Node n2){  
		   Float f1=(Float)n1.getF();  
		   Float f2=(Float)n2.getF();  
		     
		   return f1.compareTo(f2);  
	   }
   }
   
  
   

   
   public char getValueAt(int[] position) {
	   return world.getCell(position[0], position[1]).getValue();
   }
   
   public char getValueAt(Position position) {
	   return world.getCell(position.getRow(), position.getCol()).getValue();
   }

   
   public List<Character> make_path(Map<Node,Node> origins, Node position) {
	   
	   int goalOrientation = position.getOrientation();
	   
	   List<Character> path = new ArrayList<Character>();
	   path.add(position.getMove());
	   
	   while (origins.containsKey(position)) {
		   position = origins.get(position);
//		   System.out.println(position.getMove());
		   
		   if (position.getMove() == null) {
//			   path.add(0, lastM);
			   break;
		   }
		   
		   path.add(position.getMove());
	   }
	   

	   path.add((char) goalOrientation);
	   System.out.println("Path: "+path);
	   return path;
   }
   
   
   
   
   
   public List<Character> astar(Position position, Position goal, int startOrientation, boolean exploringAstar, float w, int timeCutoff, boolean showMaps) {

	   if (goal.getValue() == '$') {
		   timeCutoff *= 2;
	   }

	   long startTime = System.currentTimeMillis();
	   long endTime = startTime + timeCutoff*1000; 

		   
	   
	   Node start = new Node(position,getValueAt(position),startOrientation,null);
	   
	   start.removeObstacle(' ');
	   
	   if (!exploringAstar) {
		   start.addObstacle('#');
	   }
//	   start.removeObstacle('~');
	   
	   Set<Node> closed = new HashSet<Node>();
	   Set<Node> open = new HashSet<Node>();
	   open.add(start);
	   
	   Map<Node,Node> origins = new HashMap<Node,Node>();
	   
	   start.setG(0);
	   start.setH(manDistTo(position, goal, start.getOrientation()));
	   start.updateF();
	   
	   while (!open.isEmpty() && System.currentTimeMillis() < endTime) {
//	   while (!open.isEmpty() ) {

		   Node current = Collections.min(open, new fScoreComparator());
//		   System.out.println("open: "+open);
//		   System.out.println("Fmin: "+current.toString());
//		   
//		   if (current.getSymbol() == getValueAt(Goal)) {
		   if (current.getPosition().equals(goal)) {
//			   System.out.println("****************************** "+current.getPosition());
			   return make_path(origins, current);
		   }
		   
//		   System.out.println("\n*************** current *************** ");
//		   System.out.println(current.getMove());
//		   current.printWorld();
//		   System.out.println("*************** end current *************** ");
		   
		   Position forwardPosition = current.getForwardPosition();
		   if (forwardPosition != null) {
			   char forwardSymbol = current.nodeWorld.getCell(forwardPosition.getRow(), forwardPosition.getCol()).getValue() ;
			   
			   Node forw = new Node(forwardPosition,forwardSymbol,current.getOrientation(),current);

			   if (!current.nodeObstacles.contains(forwardSymbol) ||
					   (forwardSymbol == '~' && current.haveRaft) || 
					   (forwardPosition.equals(goal) && goal.getValue() == '#')) {
				   forw.setMove('F');
				   
//				   if (forwardSymbol == '~' && current.haveRaft) {
//					   
//					   System.out.println("adding2");
//					   System.out.println("******** Current: "+current.haveRaft);
//					   current.printWorld();
//					   System.out.println("******** Forward: ");
//					   forw.printWorld();
//					   System.out.println("****************");
//				   }
//				   
				   
				   if (forwardSymbol == 'd') {
					   forw.addDynamite();
					   forw.nodeWorld.updateCell(forwardPosition.getRow(), forwardPosition.getCol(), ' ');
				   }
				   else if (forwardSymbol == 'k') {
					   forw.setHaveKey(true);
				   }
				   else if (forwardSymbol == 'a') {
					   forw.setHaveAxe(true);
				   }
				   else if (forwardSymbol == '$') {
					   forw.setHaveTreasure(true);
				   }
				   else if (current.getSymbol() == '~' && (forwardSymbol == ' ' || forwardSymbol == '$' ||
						   forwardSymbol == 'a' || forwardSymbol == 'd' || forwardSymbol == 'k' /*|| forwardSymbol == '#'*/ )) {
					   forw.addObstacle('~');
					   
					   forw.setHaveRaft(false);
				   }
				   
				   if (forw.nodeWorld.getCell(forwardPosition.getRow(), forwardPosition.getCol()).getValue() != '~') {
					   forw.nodeWorld.updateCell(forwardPosition.getRow(), forwardPosition.getCol(), ' ');
				   }

				   current.addChild(forw);

			   }
			   else if (forwardSymbol == '-' && current.haveKey) {
//				   System.out.println("adding2");
				   forw.nodeWorld.updateCell(forw.getPosition().getRow(), forw.getPosition().getCol(), ' ');
				   forw.setPosition(current.getPosition());
				   
				   forw.setMove('U');
				   current.addChild(forw);
			   }
			   else if (forwardSymbol == 'T' && current.haveAxe) {
		   
				   
//				   System.out.println("adding3");
				   forw.nodeWorld.updateCell(forw.getPosition().getRow(), forw.getPosition().getCol(), ' ');
				   forw.setPosition(current.getPosition());

//				   forw.removeObstacle('~');
				   forw.setHaveRaft(true);
				   
				   forw.setMove('C');
				   current.addChild(forw);
			   }
			   else if (!exploringAstar && (forwardSymbol == '*' || forwardSymbol == 'T' || forwardSymbol == '-') && forw.numDynamitesHeld > 0 /*&& manDistTo(forw.getPosition(),objective,orientation) == 0*/) {
//				   System.out.println("adding4");
				   forw.removeDynamite();
				   forw.nodeWorld.updateCell(forw.getPosition().getRow(), forw.getPosition().getCol(), ' ');
				   forw.setPosition(current.getPosition());

				   forw.setMove('B');
				   current.addChild(forw);
			   }
			   
//			   System.out.println("\n*************** adding *************** ");
//			   System.out.println(forw.getMove());
//			   current.printWorld();
//			   System.out.println("*************** end adding *************** ");
		   }

		   Node left = new Node(current.getPosition(),current.getSymbol(),checkOrientation('L',current.getOrientation()),current);
		   Node right = new Node(current.getPosition(),current.getSymbol(),checkOrientation('R',current.getOrientation()),current);
		   left.setMove('L');
		   right.setMove('R');

		   current.addChild(left);
		   current.addChild(right);
		   
		   open.remove(current);
		   closed.add(current);
		   
//		   System.out.println("\n*************** current *************** ");
//		   System.out.println(current.getMove());
//		   current.printWorld();
//		   System.out.println("*************** end current *************** ");
		   
		   for (Node child: current.getChildren()) {
			   if (closed.contains(child)) { 
				   
//				   if (child.getSymbol() == 'T' && child.haveAxe && child.getMove() == 'B') {
//					   System.out.println("\n*************** closed *************** ");
//					   System.out.println(child.getMove());
//					   child.printWorld();
//					   System.out.println("*************** end closed *************** ");
//				   }
//				   System.out.println("\n*************** closed *************** ");
//				   System.out.println(child.getMove());
//				   child.printWorld();
//				   System.out.println("*************** end closed *************** ");
				   continue;
			   }
			   
			   int moveCost = 1;
			   if (exploringAstar) {
				   if (child.getMove() == 'B') {
					   moveCost = 10;
					   if (current.getSymbol() == '~') {
						   moveCost = 100;
					   }

				   }
				   else if (child.getMove() == 'C' && child.haveRaft) {
					   moveCost = 5;
					   
					   if (current.getSymbol() == '~') {
						   moveCost += 10;
					   }
				   }
				   
				   
				   if (current.getSymbol() == '~' && child.getSymbol() == ' ') {
					   moveCost += 10;
				   }
				   if (current.getSymbol() == ' ' && child.getSymbol() == '~') {
					   moveCost += 10;
				   }
			   }
			   else {
				   if (child.getMove() == 'B') {
					   moveCost = 3;
					   if (current.getSymbol() == '~') {
						   moveCost = 100;
					   }
				   }
			   }

			   

			   
			   int temp_g = current.getG() + moveCost;
			   
//			   System.out.println("\n*************** update *************** ");
//			   System.out.println(child.getMove());
//			   child.printWorld();
//			   System.out.println("*************** end update *************** ");
			   
			   if (temp_g < child.getG() || !open.contains(child) ) {
//				   System.out.println("adding: "+current.toString()+" to: "+child.toString()+" with move: "+child.getMove());
				   origins.put(child, current);
				   
				   if (showMaps) {
					   System.out.println("******** Current: "+current.numDynamitesHeld);
					   current.printWorld();
					   System.out.println("******** child: "+child.numDynamitesHeld);
					   child.printWorld();
					   System.out.println("****************");
				   }
				   

				   
				   
//				   System.out.println(origins);
				   child.setG(temp_g);
//				   child.setH(manDistTo(child.getPosition(), goal, child.getOrientation()));
				   child.setH(manDistTo(child.getPosition(), goal, 9));
				   child.updateF(w);
//				   child.updateF();
				   if (!open.contains(child)) {
					   open.add(child);
				   }
			   }
		   }
		   
	   }
	   
	   return null;
	}

   public boolean checkForNewUsefulItems() {
	   int numberOfTreesUpdate = 0;
	   int numberOfAxesUpdate = 0;
	   int numberOfDynamiteUpdate = 0;
	   int numberOfKeysUpdate = 0;
	   int numberOfTreasuresUpdate = 0;
	   
	   boolean newItems = false;
	   
	   for(int i=0; i < world.numRows(); i++ ) {
	         for(int j=0; j < world.numCols(); j++ ) {
	        	 char nextSymbol = world.getCell(i, j).getValue();
	        	 
	        	 if (nextSymbol == 'T') {
	        		 numberOfTreesUpdate++;
	        	 }
	        	 if (nextSymbol == 'a') {
	        		 numberOfAxesUpdate++;
	        	 }
	        	 if (nextSymbol == 'd') {
	        		 numberOfDynamiteUpdate++;
	        	 }
	        	 if (nextSymbol == 'k') {
	        		 numberOfKeysUpdate++;
	        	 }
	        	 if (nextSymbol == '$') {
	        		 numberOfTreasuresUpdate++;
	        	 }
	         }
	   }
	   
	 if (numberOfTreesUpdate > numberOfTrees) {
		 numberOfTrees = numberOfTreesUpdate;
//		 if (numberOfTrees < 5) {
//			 newItems = true;
//		 }
		 
  	 }
  	 if (numberOfAxesUpdate > numberOfAxes) {
  		numberOfAxes = numberOfAxesUpdate;
  		if (!have_axe) {
  			newItems = true;
  		}
  		
  	 }
  	 if (numberOfDynamiteUpdate > numberOfDynamite) {
  		numberOfDynamite = numberOfDynamiteUpdate;
  		newItems = true;
  	 }
  	 if (numberOfKeysUpdate > numberOfKeys) {
  		numberOfKeys = numberOfKeysUpdate;
  		if (!have_key) {
  			newItems = true;
  		}
  	 }
  	if (numberOfTreasuresUpdate > numberOfTreasures) {
  		numberOfTreasures = numberOfTreasuresUpdate;
  		if (!have_treasure) {
  			newItems = true;
  		}
  		
  	 }
	   
	   
	return newItems;
	   
   }
  
   
   
   public int manDistTo(Position position, Position objective, int orientation) {
	   int manDist = 0;
	   int turnCost = 0;
	   
  
	   manDist = Math.abs(position.getRow() - objective.getRow()) + Math.abs(position.getCol() - objective.getCol());
	   
	   if (manDist == 0 || orientation == 9) return manDist;
	   
	   if (position.getCol() == objective.getCol() && orientation == 0 && position.getRow() > objective.getRow()) {
		   turnCost = 0;
	   }
	   else if (position.getCol() == objective.getCol() && orientation == 2 && position.getRow() < objective.getRow()) {
		   turnCost = 0;
	   }
	   else if (position.getRow() == objective.getRow() && orientation == 3 && position.getCol() > objective.getCol()) {
		   turnCost = 0;
	   }
	   else if (position.getRow() == objective.getRow() && orientation == 1 && position.getCol() < objective.getCol()) {
		   turnCost = 0;
	   }
	   else if (position.getRow() >= objective.getRow() && (orientation == 0 )) {
		   turnCost = 1;
	   }
	   else if (position.getRow() <= objective.getRow() && (orientation == 2)) {
		   turnCost = 1;
	   }
	   else if (position.getCol() <= objective.getCol() && (orientation == 1 )) {
		   turnCost = 1;
	   }
	   else if (position.getCol() >= objective.getCol() && (orientation == 3)) {
		   turnCost = 1;
	   }
	   else {
		   turnCost = 2;
	   }
	   
	   return manDist + turnCost;
   
   }
   
     
   public List<Position> findObjective() {
//	   List<Position> output = new ArrayList<Position>();
	   
	   Map<Position,Double> allCandidates = new HashMap<Position,Double>();
//	   List<Position> reachableCandidates = new ArrayList<Position>();
//	   List<Position> unreachableCandidates = new ArrayList<Position>();
	   
	   double highestValue = 0d;
	   double nextValue = 0d;
	   
	   
	   for (int i = 0; i < world.numRows(); i++) {
		   for (int j = 0; j < world.numCols(); j++) {
			   if (symbolValues.get(world.getCell(i, j).getValue()) != null) {
				   Position nextPosition = world.getCell(i, j);
				   nextValue =  (double) symbolValues.get(nextPosition.getValue()) / (double) manDistTo(worldPosition, nextPosition, orientation)  ;
//				   System.out.println("position: "+nextPosition+" value: "+nextValue);
				   if (nextValue > 0d ) {
					   highestValue = nextValue;
//					   allCandidates.add(nextPosition);
					   allCandidates.put(nextPosition, highestValue);
				   }
			   }
				   
		   }
	   }
	   
	   List<Position> output = allCandidates.entrySet()
	              .stream()
	              .sorted(Entry.comparingByValue(Collections.reverseOrder()))
	              .map(Entry::getKey)
	              .collect(Collectors.toList());

	   if (highestValue > 0d) {
		   System.out.println(output);
		   return output;
		   
	   }
	   return new ArrayList<Position>();

   }
   
   public List<Position> findReachableTiles(Position position, int iteration) {
	   
//	   Iteration:
//	   0: No chop, blow, or move to water
//	   1: Allow chop
//	   2: Allow move to water or from water
//	   3: Allow blow
	   
	   Map<Integer,Set<Character>> boundaries = new HashMap<Integer,Set<Character>>();
	   
	   boundaries.put(0, new HashSet<Character>());
	   for (Character c: obstacles) {
		   boundaries.get(0).add(c);
		   
	   }
	   boundaries.get(0).add('#');
	   if (have_key) {
		   boundaries.get(0).remove('-');
	   }
	   
	   if (!on_water) {
		   boundaries.get(0).add('~');
	   }
	   else {
		   boundaries.get(0).remove('~');
		   boundaries.get(0).add(' ');
		   boundaries.get(0).add('T');
		   boundaries.get(0).add('a');
		   boundaries.get(0).add('k');
		   boundaries.get(0).add('d');
		   boundaries.get(0).add('$');
		   boundaries.get(0).add('-');
		   
	   }

	   
	   boundaries.put(1, new HashSet<Character>());
	   for (Character c: boundaries.get(0)) {
		   boundaries.get(1).add(c);
	   }
	   if (!on_water ) {
		   boundaries.get(1).remove('T');
	   }
	   
	   boundaries.put(2, new HashSet<Character>());
	   
	   for (Character c: boundaries.get(1)) {
		   boundaries.get(2).add(c);
	   }
	   if (on_water) {
		   boundaries.get(2).remove(' ');
		   boundaries.get(2).remove('T');
	   }
	   else {
		   boundaries.get(2).remove('~');
	   }
	   
//	   boundaries.put(3, new HashSet<Character>());
//	   
//	   for (Character c: boundaries.get(1)) {
//		   boundaries.get(2).add(c);
//	   }
//	   boundaries.get(2).remove('*');
	   
//	   System.out.println(boundaries.get(0));
//	   System.out.println(boundaries.get(1));
//	   System.out.println(boundaries.get(2));
	   
	   
//	   boundaries.put(3, new HashSet<Character>());
//	   for (Character c: obstacles) {
//		   if (c != 'T') {
//			   boundaries.get(1).add(c);
//		   }
//	   }
	   
	   
	   if (boundaries.get(iteration).contains(position) ) {
		   return null;
	   }
	   
	   Set<Position> open = new HashSet<Position>();
	   Set<Position> closed = new HashSet<Position>();
	   Map<Position,Double> reachable = new HashMap<Position,Double>();
	   
	   open.add(position);
	   Double score = null;

	   while (!open.isEmpty()) {
//		   System.out.println("Open: "+open.toString());
//		   System.out.println("Closed: "+closed);
		   
		   Iterator<Position> o = open.iterator();

		   Position current = o.next();
		   
//		   if (current.equals(from)) {
////			   System.out.println("Reachable path");
//			   return output;
//		   }
		   
		   Position north = current.north();
		   Position south = current.south();
		   Position east = current.east();
		   Position west = current.west();
		   
//		   if (current != null) {
//			   System.out.println(getValueAt(current)+" "+current);
//		   }
		   
		   if (iteration != 2 || (on_water && getValueAt(current) == '~') || (!on_water && getValueAt(current) != '~')) {
			   if (north != null && (!boundaries.get(iteration).contains(getValueAt(north)) ) && 
					   !closed.contains(north)) {
				   open.add(north);
			   }

			   if (south != null && (!boundaries.get(iteration).contains(getValueAt(south)) ) && 
					   !closed.contains(south)) {
				   open.add(south);
			   }
			   
			   if (east != null && (!boundaries.get(iteration).contains(getValueAt(east)) ) && 
					   !closed.contains(east)) {
				   open.add(east);
			   }
			   
			   if (west != null && (!boundaries.get(iteration).contains(getValueAt(west)) ) && 
					   !closed.contains(west)) {
				   open.add(west);

			   }
		   }
		   

		   if (!position.equals(current)) {
			   score = current.findExploreScore();
		   }
		   
		   if (score != null && score > 0d) {
			   reachable.put(current,score);
		   }

		   open.remove(current);
		   closed.add(current);
	   }
	   
	   
	   
	   List<Position> output = reachable.entrySet()
	              .stream()
	              .sorted(Entry.comparingByValue(/*Collections.reverseOrder()*/))
	              .map(Entry::getKey)
	              .collect(Collectors.toList());
	   
	   return output;
   }
   
 
   
   public boolean checkReachable(Position from, Position position) {
	   if (obstacles.contains(position) &&  !(getValueAt(position) == '-' && have_key)) {
		   return false;
	   }
	   
	   Set<Position> open = new HashSet<Position>();
	   Set<Position> closed = new HashSet<Position>();
	   
	   open.add(position);

	   while (!open.isEmpty()) {
//		   System.out.println("Open: "+open.toString());
//		   System.out.println("Closed: "+closed);
		   
		   Iterator<Position> o = open.iterator();

		   Position current = o.next();
		   
		   if (current.equals(from)) {
//			   System.out.println("Reachable path");
			   return true;
		   }
		   
		   Position north = current.north();
		   Position south = current.south();
		   Position east = current.east();
		   Position west = current.west();
		   
		   if (north != null && (!obstacles.contains(getValueAt(north)) || (getValueAt(north) == '-' && have_key)) && 
				   !closed.contains(north)) {
			   open.add(north);
		   }

		   if (south != null && (!obstacles.contains(getValueAt(south)) || (getValueAt(south) == '-' && have_key)) && 
				   !closed.contains(south)) {
			   open.add(south);
		   }
		   
		   if (east != null && (!obstacles.contains(getValueAt(east)) || (getValueAt(east) == '-' && have_key)) && 
				   !closed.contains(east)) {
			   open.add(east);
		   }
		   
		   if (west != null && (!obstacles.contains(getValueAt(west)) || (getValueAt(west) == '-' && have_key)) && 
				   !closed.contains(west)) {
			   open.add(west);
		   }

		   open.remove(current);
		   closed.add(current);
	   }
	   
	   return false;
   }
   
     
   public Map<Position,List<Character>> bestExploreScore() {
//	   Position candidate = new Position(objective.getRow(),objective.getCol());
	   Map<Position,Double> allCandidates = new HashMap<Position,Double>();
	   List<Position> reachableCandidates = new ArrayList<Position>();
	   List<Position> unreachableCandidates = new ArrayList<Position>();

	   double nextScore = 0d;
	   
	   for (int i = 0; i< world.numRows(); i++) {
		   for (int j = 0; j < world.numCols(); j++) {
			   Position nextPosition = new Position(i,j);
			   
			   if ((getValueAt(worldPosition) == '~' && getValueAt(nextPosition) != '~') ||
					   (getValueAt(worldPosition) != '~' && getValueAt(nextPosition) == '~')) 
			   {
				   nextScore = 0.001d;
			   }
			   else {
//				   nextScore = (double) exploreScore(nextPosition) / manDistTo(worldPosition,nextPosition,orientation);
				   
				   nextScore = (double) nextPosition.getExploreScore() / manDistTo(worldPosition,nextPosition,orientation);
			   }
			   
			   
//			   System.out.println("candidate: "+nextPosition+" score: "+nextScore);
			   if (nextScore > 0d ) {
//				   System.out.println(nextPosition+" score: "+nextScore);
				   allCandidates.put(nextPosition,nextScore);
			   }
		   }
	   }
	   
	   allCandidates = allCandidates.entrySet()
	              .stream()
	              .sorted(Map.Entry.comparingByValue(/*Collections.reverseOrder()*/))
	              .collect(Collectors.toMap(
	                Map.Entry::getKey, 
	                Map.Entry::getValue, 
	                (e1, e2) -> e1, 
	                LinkedHashMap::new
	              ));

	   
	   for (Position p: allCandidates.keySet()) {
		   if (checkReachable(p,worldPosition)) {
			   
			   reachableCandidates.add(p);
		   }
		   else {
			   unreachableCandidates.add(p);
		   }
	   }
//	   System.out.println("all candidates: "+allCandidates.values());
//	   System.out.println("reachable: "+reachableCandidates);
   
	   if (!reachableCandidates.isEmpty()) {
		   Position objective = reachableCandidates.get(reachableCandidates.size()-1);
		   objective.setReachable(true);
		   List<Character> search = astar(worldPosition, objective,orientation, true, 1f,20,false);
		   Map<Position,List<Character>> ret = new HashMap<Position,List<Character>>();
		   ret.put(objective, search);
		   return ret;
//		   return objective;
	   }
	   else if (!unreachableCandidates.isEmpty()) {
//		   System.out.println("unreachable: "+unreachableCandidates);
//		   bring in astar check for unreachable candidates before outputting
		   
		   for (int i = unreachableCandidates.size() - 1; i >= 0; i--) {
			   Position nextUnreachable = unreachableCandidates.get(i);
			   System.out.println("trying: "+nextUnreachable);
			   List<Character> search = astar(worldPosition, nextUnreachable,orientation, true, 1.5f,1,false);
			   if (search != null) {
//				   System.out.println("success!");
				   Map<Position,List<Character>> ret = new HashMap<Position,List<Character>>();
				   ret.put(nextUnreachable, search);
				   return ret;
//				   return nextUnreachable;
			   }
		   }
	   }

	   return null;
				   
	   
   }
   
   public Position findNearestReachable(Position goal) {
	   int dist = Integer.MAX_VALUE;
	   Position ret = null;
	   for (int i = 0; i< world.numRows(); i++) {
		   for (int j = 0; j < world.numCols(); j++) {
			   Position temp = world.getCell(i, j);
			   int tempDist = manDistTo(temp,goal,9);
			   
			   if (tempDist < dist && checkReachable(temp, worldPosition)) {
				   ret = temp;
				   dist = tempDist;
			   }
		   }
	   }
	   return ret;
		   
   }
   
   
   public Character move(char[][] view) {
//	   System.out.println(obstacles);
	   
//	   List<Position> objectives = new ArrayList<Position>();
	   Character move = null; 
	   
	   
	   if (have_treasure) {
		   world.getCell(worldPosition.getRow()-positionRelativeToStart[0], worldPosition.getCol()-positionRelativeToStart[1]).setValue('S');
//		   symbolValues.put('S', Integer.MAX_VALUE);
		   symbolValues.put('S', 2);
//		   print_world();
//		   System.out.println(positionRelativeToStart[0]+" "+positionRelativeToStart[1]);
//		   System.exit(-1);
	   }
	   
	   Position forwardPosition = world.getCell(worldPosition.getRow() + forward.get(orientation)[0], worldPosition.getCol() + forward.get(orientation)[1]);
	   char forwardSymbol = forwardPosition.getValue();
//	   char forwardSymbol = getValueAt(new int[]{worldPosition.getRow() + forward.get(orientation)[0],worldPosition.getCol() + forward.get(orientation)[1]});
	   System.out.println(nextMoves);
	   if (!nextMoves.isEmpty() && nextMoves.get(nextMoves.size()-1) == 'F') {
		   if (obstacles.contains(forwardSymbol) && 
				   !((on_water && forwardSymbol == '~') || (forwardSymbol == '~' && have_raft))) {
			   nextMoves = null;
			   System.out.println("finding new move set: obstacle ");
		   }
		   
		   if (!(embarkingOrDisembarking && forwardPosition.equals(objective)) && failedToFindPathToObjective && ((getValueAt(worldPosition) == '~' && forwardSymbol != '~') ||
				   (getValueAt(worldPosition) != '~' && forwardSymbol == '~'))) {
			   nextMoves = null;
			   System.out.println("finding new move set: changing between land and water ");
		   }
	   }
			   

	   
	   newItems = checkForNewUsefulItems();
	   System.out.println("new items: "+newItems);
	   
	   
	   if (nextMoves == null || nextMoves.isEmpty() || newItems) {
		   
		   embarkingOrDisembarking = false;
		   
		   
		   if (!failedToFindPathToObjective || (failedToFindPathToObjective && newItems)) {
			   nextMoves = null;
			   objective = null;
			   System.out.println("finding objective...");
			   objectives = findObjective();
			   for (Position o: objectives) {
				   System.out.println("o: "+o);
				   
//				   System.out.println("obstacles: "+obstacles);
				   
				   nextMoves = astar(worldPosition, o,orientation, false, 1.2f,1,false);
				   
				   System.out.println(nextMoves);
				   if (!(nextMoves == null) && !nextMoves.isEmpty()) {
					   
					   nextMoves.remove(nextMoves.size()-1);
					   
					   objective = o;
					   move = nextMoves.remove(nextMoves.size()-1);
					   failedToFindPathToObjective = false;
					   
					   System.out.println("new objective: "+objective.toString());
					   System.out.println("next move: "+move);
					   System.out.println("remaining moves: "+nextMoves);
					   
					   break;

				   }
				   
			   }
		   }
		   if (nextMoves == null || (nextMoves.isEmpty()  && move == null )) {
			   
			   
			   if (!objectives.isEmpty()) {
				   failedToFindPathToObjective = true;
			   }
			   
			   List<Position> reachables = findReachableTiles(worldPosition,0);
			   System.out.println("iter 0");
			   if (reachables.isEmpty() && have_axe) {
				   reachables = findReachableTiles(worldPosition,1);
				   System.out.println("iter 1");
			   }
			   if (reachables.isEmpty() && have_axe) {
				   reachables = findReachableTiles(worldPosition,2);
				   embarkingOrDisembarking = true;
				   System.out.println("iter 2");
			   }
			   
			   System.out.println(reachables);
			   
			   if (reachables.isEmpty()) {
				   failedToFindPathToObjective = false;
				   symbolValues.put('d', 1);
				   objectives = findObjective();
				   
				   for (Position o: objectives) {
					   System.out.println("checking "+o);
					   objective = o;
					   Position nearestReachable = findNearestReachable(objective);
					   System.out.println(nearestReachable);
//					   nextMoves = astar(worldPosition, objective, false, 1.2f,15,true);
					   
					   List<Character> positionToNearest = astar(worldPosition, nearestReachable,orientation, true, 1.2f,2,false);
					   int startOrientation = (int) positionToNearest.remove(positionToNearest.size()-1);
//					   System.out.println(startOrientation);
					   nextMoves = astar(nearestReachable, objective,startOrientation, false, 1.2f,15,false);
					   nextMoves.remove(nextMoves.size()-1);
					   
					   if (nextMoves.get(0) != null) {
						   nextMoves.addAll(positionToNearest);
					   }
					   else {
						   nextMoves = positionToNearest;
					   }
					   
					   
					   if (!(nextMoves == null) && !nextMoves.isEmpty()) {
						   break;
					   }
				   }
				   
				   
				   
			   }
			   else {
				   objective = reachables.get(reachables.size()-1);
				   nextMoves = astar(worldPosition, objective,orientation, true, 1.5f,1,false);
				   nextMoves.remove(nextMoves.size()-1);
			   }
			   
   
			   System.out.println("objective: "+objective+" score "+objective.getExploreScore()+" symbol "+objective.getValue());
			   
			   
			   move = nextMoves.remove(nextMoves.size()-1);

			   System.out.println(obstacles);
			   System.out.println("explore objective: "+objective.toString()+" "+objective.getExploreScore());
			   System.out.println("reachable: "+objective.isReachable()+" have raft "+have_raft);   

//			   System.out.println("obstacles: "+obstacles);
			   
			   System.out.println("moves: "+nextMoves);
			   
		   }

	   }
	   else {
		   move = nextMoves.remove(nextMoves.size()-1);
	   }

     updateOrientation(move);
     lastMoveSuccessful = successfulMove(move, view);
//     System.out.println(on_water+" "+obstacles+" "+view[1][2]);
	   System.out.println("move: "+move);
	   return move;
   }
   
   
//   public char move() {
//	   int r = -1;
//		try {
//			r = (char) System.in.read();
//		} catch (IOException e) {
//
//		}
//		return (char) r;
//
//   }
   
   
   
   public int successfulMove(char move, char view[][]) {
	   /*	
	     	0 - failed move or turn
	   		1 - normal move forward
	   		2 - open, blowup or chop
	   		3 - move forward over key, dynamite or axe
			4 - got the treasure, let's go home
	   	*/
	   
	   if (view[1][2] != '*' && view[1][2] != '-' && view[1][2] != 'T' && (move == 'f' || move == 'F')) {
		   if (view[1][2] == 'a') {
			   have_axe = true;
			   numberOfAxes--;
//			   removable.add('T');
//			   System.out.println("GOT AXE!");
			   symbolValues.put('a',0);
			   
//			   if (have_raft == false) {
//				   symbolValues.put('T',2);
//			   }
//			   
			   
			   if (on_water == true) {
				   System.out.println("lost raft");
				   on_water = false;
				   have_raft = false;
				   obstacles.add('~');
//				   symbolValues.put('T',2);
			   }
			   
			   return 3;
		   }
		   else if (view[1][2] == 'k') {
			   numberOfKeys--;
			   have_key = true;
//			   obstacles.remove('-');
			   symbolValues.put('k',0);
			   if (on_water == true) {
				   System.out.println("lost raft");
				   on_water = false;
				   have_raft = false;
				   obstacles.add('~');
//				   symbolValues.put('T',2);
			   }
			   
			   return 3;
		   }
		   else if (view[1][2] == '$') {
			   numberOfTreasures--;
			   symbolValues.put('$',0);
			   have_treasure = true;
			   
			   if (on_water == true) {
				   System.out.println("lost raft");
				   on_water = false;
				   have_raft = false;
				   obstacles.add('~');
//				   symbolValues.put('T',2);
			   }
			   return 3;
		   }
		   else if (view[1][2] == 'd') {
			   
			   
			   
			   numberOfDynamite--;
			   
//			   removable.add('*');
//			   removable.add('T');
//			   if (symbolValues.get('T') < 2) {
//				   symbolValues.put('T',1);
//			   }
			   
			   if (on_water == true) {
				   System.out.println("lost raft");
				   on_water = false;
				   have_raft = false;
//				   symbolValues.put('~',-1);
				   obstacles.add('~');
//				   symbolValues.put('T',2);
			   }
			   
//			   System.out.println("GOT DYNAMITE!");
			   num_dynamites_held++;
			   return 3;
		   }
		   else if (view[1][2] == ' ' && on_water == true) {
			   System.out.println("lost raft");
			   on_water = false;
			   have_raft = false;
			   obstacles.add('~');
//			   symbolValues.put('T',2);
			   return 3;
		   }
		   else if (view[1][2] == '~') {
			   obstacles.remove('~');
			   on_water = true;
			   return 1;
		   }

		   
//		   ADD THIS: If the current tile is water, and you move to land, then symbolValues.put('~',-1) and obstacles.add('~');
		   
		   return 1;
	   }
	   else if (view[1][2] == '-' && have_key && (move == 'u' || move == 'U')) {
		   return 2;
	   }
	   else if (view[1][2] == 'T' && have_axe && (move == 'c' || move == 'C')) {
		   numberOfTrees--;
		   symbolValues.put('~',0);
//		   obstacles.remove('~');
		   have_raft = true;
		   symbolValues.put('T',0);
		   return 2;
	   }
	   else if ((view[1][2] == '*' || view[1][2] == 'T' || view[1][2] == '-') && num_dynamites_held > 0 && (move == 'b' || move == 'B')) {
		   num_dynamites_held--;
		   System.out.println("BOOM");
//		   if (num_dynamites_held == 0) {
//			   System.out.println("No more dynamite :(");
//			   removable.remove('*');
//			   if (!have_axe) {
//				   removable.remove('T');
//			   }
//			   if (!have_key) {
//				   removable.remove('-');
//			   }
//		   }
		   return 2;
	   }

	   return 0;		   
   }
   
   public void initialiseWorld(char view[][]) {
	// Create first view of world
//	   System.out.println("here");
	   for(int i = 0; i < 5; i++)  {
		   for(int j = 0; j < 5; j++)  {
			   Position p = new Position(i,j);
			   p.setValue(view[i][j]);
			   world.addPosition(i, j, p);
			   
		   }	   
	    }
	   
//	   worldPosition = world.get(2).get(2);
//	   world.get(2).set(2,'S');
   }
   
   public boolean needExpansion() {

//	   System.out.println("pos: "+positionRelativeToStart[0]);
	      
	   if (positionRelativeToStart[0] < 0 && Math.abs(positionRelativeToStart[0]) > expandedUpRightDownLeft[0]) {
		   return true;
	   }
	   else if (positionRelativeToStart[0] > 0 && Math.abs(positionRelativeToStart[0]) > expandedUpRightDownLeft[2]) {
		   return true;
	   }
	   else if (positionRelativeToStart[1] > 0 && Math.abs(positionRelativeToStart[1]) > expandedUpRightDownLeft[1]) {
		   return true;
	   }
	   else if (positionRelativeToStart[1] < 0 && Math.abs(positionRelativeToStart[1]) > expandedUpRightDownLeft[3]) {
		   return true;
	   }
	   
	   return false;
   }
   
   public void updateFOV(char view[][]) {
	   int count = 0;
	   if (orientation == 0) {
		   List<Position> FOV = world.getRow(worldPosition.getRow() + forward.get(orientation)[0] * 2);

		   for (int i = 0; i < FOV.size(); i++) {
			   if (worldPosition.getCol()-2 <= i && i <= worldPosition.getCol()+2 && count < 5) {
				   FOV.get(i).setValue(view[0][count]);
				   count++;
			   }

		   }
	   }
	   else if (orientation == 2) {
		   List<Position> FOV = world.getRow(worldPosition.getRow() + forward.get(orientation)[0] * 2);

		   for (int i = FOV.size() - 1; i >= 0; i--) {
			   if (worldPosition.getCol()-2 <= i && i <= worldPosition.getCol()+2 && count < 5) {
				   FOV.get(i).setValue(view[0][count]);
				   count++;
			   }

		   }
		   
	   }
	   else if (orientation == 1) {
		   for (int i = 0; i < world.numRows(); i++) {
			   if (worldPosition.getRow()-2 <= i && i <= worldPosition.getRow()+2 && count < 5) {
				   world.getCell(i, worldPosition.getCol() + forward.get(orientation)[1] * 2).setValue(view[0][count]);
				   count++;
			   }
		   }
	   }
	   else if (orientation == 3) {
		   for (int i = world.numRows() - 1; i >=0; i--) {
			   if (worldPosition.getRow()-2 <= i && i <= worldPosition.getRow()+2 && count < 5) {
				   world.getCell(i, worldPosition.getCol() + forward.get(orientation)[1] * 2).setValue(view[0][count]);
				   count++;
			   }
		   }
	   }
   }
	   

   public void updateWorld(char view[][], int successfulMove) {

	   if (successfulMove != 0) {
		   if (successfulMove == 2) {
			   Integer[] position = forward.get(orientation);
//			   System.out.println((worldPosition.getRow() + position[0])+" "+(worldPosition.getRow() + position[1]));
			   world.getCell(worldPosition.getRow() + position[0],worldPosition.getCol() + position[1]).setValue(' '); 
		   }
		   else {
			   int count = 0;
			   updateWorldPosition();

			   if (needExpansion()) {
				   expandedUpRightDownLeft[orientation]++;

				   if (orientation == 0 ) {
					   List<Position> newRow = new ArrayList<Position>();
					   for (int i = 0; i < world.numCols(); i++) {
						   newRow.add(new Position('#'));
					   }

					   for (int i = 0; i < newRow.size(); i++) {
						   if (worldPosition.getCol()-2 <= i && i <= worldPosition.getCol()+2 && count < 5) {
//							   System.out.print(view[0][count]);
							   newRow.get(i).setValue(view[0][count]);
//							   for (Position x: newRow) {
//								   System.out.print(x.getValue());
//							   }
							   count++;
						   }
					   }

					   world.addRow('T', newRow);
				   }
				   else if (orientation == 2) {
					   List<Position> newRow = new ArrayList<Position>();
					   for (int i = 0; i < world.numCols(); i++) {
						   newRow.add(new Position('#'));
					   }

					   for (int i = newRow.size() -1 ; i >= 0 ; i--) {
						   if (worldPosition.getCol()-2 <= i && i <= worldPosition.getCol()+2 && count < 5) {
							   newRow.get(i).setValue(view[0][count]);
							   count++;
						   }
					   }
					   world.addRow('B',newRow);
				   }
				   else if (orientation == 1) {
					   for (int i = 0; i < world.numRows(); i++) {
						   Position p = new Position('#');
						   if (worldPosition.getRow()-2 <= i && i <= worldPosition.getRow()+2 && count < 5) {
							   p.setValue(view[0][count]);
							   
							   count++;
						   }
						   
						   world.addPosition(i, p);

					   }
				   }
				   else if (orientation == 3) {
					   for (int i = world.numRows() - 1; i >=0; i--) {
						   Position p = new Position('#');
						   if (worldPosition.getRow()-2 <= i && i <= worldPosition.getRow()+2 && count < 5) {
							   p.setValue(view[0][count]);
							   System.out.print(view[0][count]);
//							   p.setValue('t');
							   
							   count++;
						   }

						   world.addPosition(i, 0, p);
					   }
					   
//					   for (int i = 0; i < world.size(); i++) {
//						   for (int j = 0; j < world.get(0).size(); j++) {
//							   if (j != 0) {
//								   world.get(i).get(j).setRow(world.get(i).get(j).getCol()+1);
//							   }
//							   
//						   }
//					   }
				   }
				   count = 0;
		   
			   }
			   
			   for (int i = 0; i < world.numRows(); i++) {
				   for (int j = 0; j < world.numCols(); j++) {
					   world.getCell(i, j).setRow(i);
					   world.getCell(i, j).setCol(j);
				   }
			   }
			   
//			   print_world();
			   updateFOV(view);
			   
			   if (successfulMove == 3) {
				   
				   world.getCell(worldPosition.getRow(), worldPosition.getCol()).setValue(' ');
			   }
		   }
		   
	   }
	   
	   
   }
   
   public void updateWorldPosition() {
	   positionRelativeToStart[0] += forward.get(orientation)[0];
	   positionRelativeToStart[1] += forward.get(orientation)[1];
	   
	   if (!((worldPosition.getRow() <= 2 && orientation == 0) ||  (worldPosition.getCol() <= 2 && orientation == 3) )) {
		   
//		   worldPosition = world.get(worldPosition.getRow() + forward.get(orientation)[0]).get(worldPosition.getCol() + forward.get(orientation)[1]);
		   
		   worldPosition.setRow(worldPosition.getRow() + forward.get(orientation)[0]);
		   worldPosition.setCol(worldPosition.getCol() + forward.get(orientation)[1]);

	   }

   }
   
   public void updateOrientation(char turn) {
	   if (turn == 'L' || turn == 'l') {
		   orientation = (orientation - 1) % 4;
		   if (orientation<0) {
			   orientation += 4;
			   
		   }
	   }
	   else if (turn == 'R' || turn == 'r'){
		   orientation = (orientation + 1) % 4;
	   }
   }
   
   public int checkOrientation(char turn, int start) {
	   if (turn == 'L' || turn == 'l') {
		   start = (start - 1) % 4;
		   if (start<0) {
			   return start += 4;
			   
		   }
		   return start;
	   }
	   else if (turn == 'R' || turn == 'r'){
		   return (start + 1) % 4;
	   }
	   return start;
   }
   
	
   

   public char get_action( char view[][] ) {
	   
      // REPLACE THIS CODE WITH AI TO CHOOSE ACTION

      int ch = 0;

      System.out.print("Enter Action(s): ");

//      try {
         while ( ch != -1 ) {
            // read character from keyboard
//            ch  = System.in.read();
        	 
        	 ch = move(view);

            
//            updateOrientation((char) ch);
//            lastMoveSuccessful = successfulMove((char) ch, view);
//            System.out.println(lastMoveSuccessful);
            if (lastMoveSuccessful != 0 || ch == 'L' || ch == 'l' || ch == 'R' || ch == 'r') {
            	return((char) ch );          
            }
         }
//      }
//      catch (IOException e) {
//         System.out.println ("IO error:" + e );
//      }
//
      return 0;
   }
   


   void print_view( char view[][] )
   {
      int i,j;

      System.out.println("\n+-----+");
      for( i=0; i < 5; i++ ) {
         System.out.print("|");
         for( j=0; j < 5; j++ ) {
            if(( i == 2 )&&( j == 2 )) {
               System.out.print('^');
            }
            else {
               System.out.print( view[i][j] );
            }
         }
         System.out.println("|");
      }
      System.out.println("+-----+");

   }
   
   void print_world()
   {
      int i,j;
      
      char[] dashArray = new char[world.numCols()];
      Arrays.fill(dashArray, '-');
      String dashes = new String(dashArray);
      
      System.out.println("\n+"+dashes+"+");
      for( i=0; i < world.numRows(); i++ ) {
         System.out.print("|");
         for( j=0; j < world.numCols(); j++ ) {
            if(( i == worldPosition.getRow() )&&( j == worldPosition.getCol() )) {
               System.out.print(orientationSymbol[orientation]);
            }
            else {
               System.out.print( world.getCell(i, j).getValue() );
            }
         }
         System.out.println("|");
      }
      System.out.println("+"+dashes+"+");

   }

   public static void main( String[] args )
   {

	   
      InputStream in  = null;
      OutputStream out= null;
      Socket socket   = null;
      Agent  agent    = new Agent();
      char   view[][] = new char[5][5];
      char   action   = 'F';
      int port;
      int ch;
      int i,j;

      if( args.length < 2 ) {
         System.out.println("Usage: java Agent -p <port>\n");
         System.exit(-1);
      }

      port = Integer.parseInt( args[1] );

      try { // open socket to Game Engine
         socket = new Socket( "localhost", port );
         in  = socket.getInputStream();
         out = socket.getOutputStream();
      }
      catch( IOException e ) {
         System.out.println("Could not bind to port: "+port);
         System.exit(-1);
      }

      try { // scan 5-by-5 window around current location
         while( true ) {
            for( i=0; i < 5; i++ ) {
               for( j=0; j < 5; j++ ) {
                  if( !(( i == 2 )&&( j == 2 ))) {
                     ch = in.read();
                     if( ch == -1 ) {
                        System.exit(-1);
                     }
                     view[i][j] = (char) ch;
                  }
               }
            }
//            agent.print_view( view ); // COMMENT THIS OUT BEFORE SUBMISSION
            
            if (agent.world.getRow(0).isEmpty()) {
            	agent.initialiseWorld(view);
            }
            else {
            	agent.updateWorld(view, agent.lastMoveSuccessful);
            }
            
            agent.world.print();
//            agent.print_world();
            action = agent.get_action( view );
            out.write( action );
            
            
         }

      }
      catch( IOException e ) {
         System.out.println("Lost connection to port: "+ port );
         System.exit(-1);
      }
      finally {
         try {
            socket.close();
         }
         catch( IOException e ) {}
      }
   }
}
