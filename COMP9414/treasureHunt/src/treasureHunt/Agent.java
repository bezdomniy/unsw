package treasureHunt;
/*********************************************
 *  Agent.java 
 *  Sample Agent for Text-Based Adventure Game
 *  COMP3411 Artificial Intelligence
 *  UNSW Session 1, 2017
*/

import java.util.*;
import java.util.Map.Entry;
import java.io.*;
import java.net.*;


public class Agent {
	int c=0;
	
   private boolean have_axe     = false;
   private boolean have_key     = false;
   private boolean have_treasure= false;
   private boolean on_water = false;
//   private boolean have_raft    = false;
   private int num_dynamites_held = 0;
	
   private Map<Character, Integer> symbolValues = new HashMap<Character, Integer>();
   private List<List<Character>> world = new ArrayList<List<Character>>();
   private int orientation = 0;
   private char[] orientationSymbol = {'^','>','v','<'};
   
   private int lastMoveSuccessful = 0;
//   private char lastMove = ' ';
   
   private Position objective = new Position(2,2);
   private List<Character> nextMoves = new ArrayList<Character>();
   Set<Character> obstacles = new HashSet<Character>();
   Set<Character> removable = new HashSet<Character>();
   
   private Position worldPosition = new Position(2,2);
   
   
   private int[] positionRelativeToStart = {0,0};
   private int[] expandedUpRightDownLeft = {0,0,0,0};
      
      
   private Map<Integer, Integer[]> forward = new HashMap<Integer,Integer[]>();
   
   
   public class Position {
	   private int row;
	   private int col;
	   
	   public Position(int row, int col) {
	       this.row = row;
	       this.col = col;
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
	   
	   public char getValueAt() {
		   return world.get(this.row).get(this.col);
	   }
	   
	   public Position north() {
		   if (this.getRow()-1 >= 0) {
			   return new Position(this.getRow()-1,this.getCol());
		   }
		   return null;
	   }
	   
	   public Position south() {
		   if (this.getRow()+1 < world.size()) {
			   return new Position(this.getRow()+1,this.getCol());
		   }
		   return null;
	   }
	   
	   public Position east() {
		   if (this.getCol()+1 < world.get(0).size()) {
			   return new Position(this.getRow(),this.getCol()+1);
		   }
		   return null;
	   }
	   
	   public Position west() {
		   if (this.getCol()-1 >= 0) {
			   return new Position(this.getRow(),this.getCol() - 1);
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
	   
	   
   }
   
   public class Node {
	   private Position position;
	   private int g;
	   private int h;
	   private int f;
	   private int orientation;
	   private Character symbol;
	   
	   private Character moveToReach = null;
	   
	   private List<Node> children = new ArrayList<Node>();
	
	
	   public Node() {}
	   
	   public Node(Position position, Character symbol, int orientation) {
		   this.position = position;
		   this.symbol = symbol;
		   this.orientation = orientation;
	   }
	   
	   @Override
	   public boolean equals(Object object)
	   {
	       if (object != null && object instanceof Node)
	       {
	           return this.position.equals(((Node) object).getPosition()) && this.orientation == ((Node) object).getOrientation();
	       }

	       return false;
	   }
	   
	   @Override
	    public int hashCode() {
	        return Objects.hash(this.position,this.orientation);
	    }
	   
	   @Override
	   public String toString() {
		   return String.format("Pos: "+this.position.getRow()+" "+this.position.getCol()+", Ori: "+this.orientation);
		   
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
	   
	   public int getH() {
		   return this.h;
	   }
	   
	   public int getG() {
		   return this.g;
	   }
	   
	   public int getF() {
		   return this.f;
	   }
	   
	   public char getSymbol() {
		   return this.symbol;
	   }
	   
	   public Position getPosition() {
		   return this.position;
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
	   
	   
   }
   
   public class fScoreComparator implements Comparator<Node> {  
	@Override
	public int compare(Node n1, Node n2){  
		   Integer f1=(Integer)n1.getF();  
		   Integer f2=(Integer)n2.getF();  
		     
		   return f1.compareTo(f2);  
	   }
   }
   
  
   
   public Agent() {
	   symbolValues.put('a', 2);
	   symbolValues.put('d', 2);
	   symbolValues.put('k', 2);
	   symbolValues.put('$', 2);
	   
	   symbolValues.put(' ', 0);
	   symbolValues.put('#', 0);
	   
	   symbolValues.put('T', -1);
	   symbolValues.put('*', -1);
	   symbolValues.put('-', -1);
	   symbolValues.put('~', -1);
	   symbolValues.put('.', -1);
	   
	   
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
	   
	   
//	   int[] t = {1,2};
//	   orientation = 2;
//	   System.out.println(manDistTo(t));
	   
	   for(int i = 0; i < 5; i++)  {
		   List<Character> nextRow = new ArrayList<Character>(5);
		   world.add(i, nextRow);
	   }
   }
   
   public char getValueAt(int[] position) {
	   return world.get(position[0]).get(position[1]);
   }
   
   public char getValueAt(Position position) {
	   return world.get(position.getRow()).get(position.getCol());
   }

   
   public List<Character> make_path(Map<Node,Node> origins, Node position) {
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
	   

	   
//	   System.out.println("Path: "+path);
	   return path;
   }
	   
   
   
   public List<Character> astar(Position position, Position goal) {
	   Node start = new Node(position,getValueAt(position),orientation);
	   Set<Node> closed = new HashSet<Node>();
	   Set<Node> open = new HashSet<Node>();
	   open.add(start);
	   
	   Map<Node,Node> origins = new HashMap<Node,Node>();
	   
	   start.setG(0);
	   start.setH(manDistTo(position, goal, start.getOrientation()));
	   start.updateF();
	   
	   while (!open.isEmpty()) {

		   Node current = Collections.min(open, new fScoreComparator());
//		   System.out.println("open: "+open);
//		   System.out.println("Fmin: "+current.toString());
//		   
//		   if (current.getSymbol() == getValueAt(Goal)) {
		   if (current.getPosition().equals(goal)) {
			   System.out.println("****************************** "+current.getSymbol());
			   return make_path(origins, current);
		   }
		   
		   
		   Position forwardPosition = new Position(current.getPosition().getRow()+forward.get(current.orientation)[0],current.getPosition().getCol()+forward.get(current.orientation)[1]);
		   if (forwardPosition.getRow() >= 0 && forwardPosition.getCol() >= 0 && forwardPosition.getRow() < world.size() && forwardPosition.getCol() < world.get(0).size()) {
			   
			   char forwardSymbol = getValueAt(forwardPosition) ;
			   Node forw = new Node(forwardPosition,forwardSymbol,current.getOrientation());
//			   System.out.println("symbol: "+forwardSymbol);
//			   if (forwardSymbol != '*' && forwardSymbol != '-' && forwardSymbol != 'T' && forwardSymbol != '~' && forwardSymbol != '#' ) {
			   if (!obstacles.contains(forwardSymbol)) {
//				   System.out.println("adding1");
				   forw.setMove('F');
				   current.addChild(forw);
			   }
			   else if (forwardSymbol == '-' && have_key) {
//				   System.out.println("adding2");
				   forw.setMove('U');
				   current.addChild(forw);
			   }
			   else if (forwardSymbol == 'T' && have_axe) {
//				   System.out.println("adding3");
				   forw.setMove('C');
				   current.addChild(forw);
			   }
			   else if ((forwardSymbol == '*' || forwardSymbol == 'T') && num_dynamites_held > 0 && manDistTo(forw.getPosition(),objective,orientation) == 0) {
//				   System.out.println("adding4");
				   forw.setMove('B');
				   current.addChild(forw);
			   }
		   }

		   Node left = new Node(current.getPosition(),current.getSymbol(),checkOrientation('L',current.getOrientation()));
		   Node right = new Node(current.getPosition(),current.getSymbol(),checkOrientation('R',current.getOrientation()));
		   left.setMove('L');
		   right.setMove('R');

		   current.addChild(left);
		   current.addChild(right);
		   
		   open.remove(current);
		   closed.add(current);
		   
		   for (Node child: current.getChildren()) {
			   if (closed.contains(child)) { 
				   continue;
			   }
			   
			   
//			   if (child.getSymbol() == '*' || child.getSymbol() == '-' || child.getSymbol() == 'T' || child.getSymbol() == '~') {
//				   System.out.println(child.getSymbol());
//				   obstacleCost = Integer.MAX_VALUE;
//			   }
//			   
//			   int temp_g = current.getG() + 1 + obstacleCost /*dist between current and child*/;
//			   obstacleCost = 0;
			   int temp_g = current.getG() + 1;
			   
			   if (temp_g < child.getG() || !open.contains(child) ) {
//				   System.out.println("adding: "+current.toString()+" to: "+child.toString()+" with move: "+child.getMove());
				   origins.put(child, current);
//				   System.out.println(origins);
				   child.setG(temp_g);
				   child.setH(manDistTo(child.getPosition(), goal, child.getOrientation()));
				   child.updateF();
				   if (!open.contains(child)) {
					   open.add(child);
				   }
			   }
		   }
		   
	   }

	   return null;
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
   
   public int distFromNearestEdge(Position position) {
//	   Set<Integer> distToEdge = new HashSet<Integer>();
//	   
//	   for (int i: new int[]{0,world.size()-1}) {
//		   for (int j = 0; j < world.get(0).size(); j++) {
//			   distToEdge.add(manDistTo(position,new int[]{i,j},9));
//		   }
//	   }
//	   for (int i: new int[]{0,world.get(0).size()-1}) {
//		   for (int j = 0; j < world.size(); j++) {
//			   distToEdge.add(manDistTo(position,new int[]{j,i},9));
//		   }
//	   }
//	   
//	   return Collections.min(distToEdge);
	   Position nearestEdge = findNearestReachableEdge(position);
	   return manDistTo(position,nearestEdge,9);
	   
	   
   }
   
   public Position findNearestReachableEdge(Position position) {
	   Position nearestEdge = new Position(worldPosition.getRow(),worldPosition.getCol());
	   
	   int edgeDist = Integer.MAX_VALUE;
	   
	   for (int i: new int[]{0,world.size()-1}) {
		   for (int j = 0; j < world.get(0).size(); j++) {
			   int old_i = i;
			   
			   if (i == 0) {
				   while (getValueAt(new int[]{i,j}) == '#') {
					   i++;
				   }
			   }
			   if (i == world.size()-1) {
				   while (getValueAt(new int[]{i,j}) == '#') {
					   i--;
				   }
			   }
			   
			   Position nextEdge = new Position(i,j);
			   int nextEdgeDist = manDistTo(worldPosition, nextEdge, orientation);
			   if (nextEdgeDist < edgeDist && !obstacles.contains(getValueAt(nextEdge)) && checkReachable(worldPosition,nextEdge)) {
				   edgeDist = nextEdgeDist;
				   nearestEdge.setRow(i);
				   nearestEdge.setCol(j);
			   }
			   
			   i = old_i;
		   }
	   }
	   for (int i: new int[]{0,world.get(0).size()-1}) {
		   for (int j = 0; j < world.size(); j++) {
			   int old_i = i;
			   
			   if (i == 0) {
				   while (getValueAt(new int[]{j,i}) == '#') {
					   i++;
				   }
			   }
			   if (i == world.get(0).size()-1) {
				   while (getValueAt(new int[]{j,i}) == '#') {
					   i--;
				   }
			   }
			   
			   Position nextEdge =	new Position(j,i);
			   int nextEdgeDist = manDistTo(worldPosition,nextEdge , orientation);
			   if (nextEdgeDist < edgeDist && !obstacles.contains(getValueAt(nextEdge)) && checkReachable(worldPosition,nextEdge)) {
				   edgeDist = nextEdgeDist;
				   nearestEdge.setRow(j);
				   nearestEdge.setCol(i);
			   }
			   
			   i = old_i;
		   }
	   }
		   
	   
	   
	   return nearestEdge;
	   
   }
   
   public void findObjective() {
	   List<Position> allCandidates = new ArrayList<Position>();
	   List<Position> reachableCandidates = new ArrayList<Position>();
	   List<Position> unreachableCandidates = new ArrayList<Position>();
	   
	   double highestValue = 0d;
	   double nextValue = 0d;
	   
	   
	   for (int i = 0; i < world.size(); i++) {
		   for (int j = 0; j < world.get(i).size(); j++) {
			   if (symbolValues.get(world.get(i).get(j)) != null) {
				   Position nextPosition = new Position(i,j);
				   nextValue =  ((double) symbolValues.get(world.get(i).get(j)))  ;
				   if (nextValue >= highestValue ) {
//					   if (nextValue > highestValue) {
					   if (highestValue == 0 && nextValue > highestValue) {
						   allCandidates = new ArrayList<Position>();
					   }
					   highestValue = nextValue;
					   allCandidates.add(nextPosition);
				   }
			   }
				   
		   }
	   }

	   if (highestValue > 0) {
		   System.out.println("here"+allCandidates);
//		   objective = allCandidates.get(0);

		   for (Position p: allCandidates) {
			   if (checkReachable(worldPosition,p)) {
				   reachableCandidates.add(p);
			   }
			   else {
				   unreachableCandidates.add(p);
			   }
		   }
		   
		   
		   
		   if (!reachableCandidates.isEmpty()) {
			   highestValue = 0d;
			   nextValue = 0d;
			   objective = reachableCandidates.get(0);
			   
			   for (Position p: reachableCandidates) {
				   nextValue =  ((double) symbolValues.get(p.getValueAt()) / manDistTo(worldPosition, p, orientation)) ;
				   if (nextValue >= highestValue) {
					   System.out.println("reachable"+p);
					   highestValue=nextValue;
					   objective = p;
				   }
			   }
			   return;
		   }
		   else {
			   highestValue = 0d;
			   nextValue = 0d;
			   objective = unreachableCandidates.get(0);
			   
			   for (Position p: unreachableCandidates) {
				   nextValue =  ((double) symbolValues.get(p.getValueAt()) / manDistTo(worldPosition, p, orientation)) ;
				   if (nextValue >= highestValue) {
					   System.out.println("unreachable"+p+" value: "+nextValue);
					   highestValue=nextValue;
					   objective = p;
				   }
			   }
			   
		   }
		   
	   }
	   else {
		   System.out.println("finding edge");
		   objective = findNearestReachableEdge(worldPosition);
	   }

   }
   
   public Position findNearestTo(Position objective) {
	   Map<Position,Integer> manDistPositionsMap = new HashMap<Position,Integer>();
	   
	   for (int i = 0; i < world.size(); i++) {
		   for (int j = 0; j < world.get(i).size(); j++) {
			   Position nextPos = new Position(i,j);
			   if (i != j && !obstacles.contains(getValueAt(nextPos)) && checkReachable(worldPosition,nextPos) && !nextPos.equals(worldPosition)) {
//				   System.out.println(nextPos[0]+" "+nextPos[1]+"Reachable: "+checkReachable(nextPos));
				   int nextManDist = manDistTo(nextPos, objective, 0);
				   int distFromEdge = distFromNearestEdge(nextPos);
				   int exploreScore = exploreScore(nextPos);
				   
//				   manDistPositionsMap.put(nextPos, nextManDist + distFromEdge);
				   manDistPositionsMap.put(nextPos, nextManDist + distFromEdge - exploreScore);
			   }
		   }
	   }
	   
//	   int minDist = Collections.min(manDistPositionsMap.values());
//	   Map<int[],Integer> candidates = new HashMap<int[],Integer>();
//	   
//
//	   for (Entry<int[], Integer> entry : manDistPositionsMap.entrySet()) {
//	       if (/*minDist == entry.getValue() &&*/ !Arrays.equals(entry.getKey(), worldPosition)) {
//	           candidates.put(entry.getKey(),distFromNearestEdge(entry.getKey()));
//	       }
//	   }
//	   
//	   for (int[] v: candidates.keySet()) {
//		   System.out.println("key "+v[0]+" "+v[1]);
//	   }
//	   
//	   for (Integer v: candidates.values()) {
//		   System.out.println("val "+v);
//	   }


	   return Collections.min(manDistPositionsMap.entrySet(),
               Comparator.comparingInt(Entry::getValue)).getKey();
   }
   
   public int exploreScore(Position position) {
	   int score = 0;
	   
	   try {
		   for (int i = position.getCol() - 2; i <= position.getCol() + 2; i++) {
			   if (getValueAt(new int[]{position.getRow() - 2,i}) == '#') {
				   score = 1;
				   if (getValueAt(new int[]{position.getRow() - 1,i}) == '#') {
					   return 2;
				   }
			   }
			   if (getValueAt(new int[]{position.getRow() + 2,i}) == '#') {
				   score = 1;
				   if (getValueAt(new int[]{position.getRow() + 1,i}) == '#') {
					   return 2;
				   }
			   }
		   }
		   
		   for (int i = position.getRow() - 2; i <= position.getRow() + 2; i++) {
			   if (getValueAt(new int[]{i,position.getCol() - 2}) == '#') {
				   score = 1;
				   if (getValueAt(new int[]{i,position.getCol() - 1}) == '#') {
					   return 2;
				   }
			   }
			   if (getValueAt(new int[]{i,position.getCol() + 2}) == '#') {
				   score = 1;
				   if (getValueAt(new int[]{i,position.getCol() + 1}) == '#') {
					   return 2;
				   }
			   }
		   }
	   } catch (IndexOutOfBoundsException e) {
		   
	   }
	   
	   
	   return score;
	
   }
   
   public boolean checkReachable(Position from, Position position) {
	   
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
		   
		   if (north != null && !obstacles.contains(getValueAt(north)) && !closed.contains(north)) {
			   open.add(north);
		   }

		   if (south != null && !obstacles.contains(getValueAt(south)) && !closed.contains(south)) {
			   open.add(south);
		   }
		   
		   if (east != null && !obstacles.contains(getValueAt(east)) && !closed.contains(east)) {
			   open.add(east);
		   }
		   
		   if (west != null && !obstacles.contains(getValueAt(west)) && !closed.contains(west)) {
			   open.add(west);
		   }

		   open.remove(current);
		   closed.add(current);
	   }
	   
	   return false;
   }
   
   public List<Position> findBorder(Position position) {
   
	   Set<Position> open = new HashSet<Position>();
	   Set<Position> closed = new HashSet<Position>();
	   
	   List<Position> border = new ArrayList<Position>();
	   
	   open.add(position);

	   while (!open.isEmpty()) {
//		   System.out.println("Open: "+open.toString());
//		   System.out.println("Closed: "+closed);
		   
		   Iterator<Position> o = open.iterator();
		   Position current = o.next();
		   
 		   Position north = current.north();
		   Position south = current.south();
		   Position east = current.east();
		   Position west = current.west();
		   
		   if (north != null && !obstacles.contains(getValueAt(north)) && !closed.contains(north)) {
			   open.add(north);
		   }
		   else if(north != null && obstacles.contains(getValueAt(north)) && getValueAt(north) != '.' && getValueAt(north) != '#') {
			   border.add(north);
		   }

		   if (south != null && !obstacles.contains(getValueAt(south)) && !closed.contains(south)) {
			   open.add(south);
		   }
		   else if(south != null && obstacles.contains(getValueAt(south)) && getValueAt(south) != '.' && getValueAt(south) != '#') {
			   border.add(south);
		   }

		   
		   if (east != null && !obstacles.contains(getValueAt(east)) && !closed.contains(east)) {
			   open.add(east);
		   }
		   else if(east != null && obstacles.contains(getValueAt(east)) && getValueAt(east) != '.' && getValueAt(east) != '#') {
			   border.add(east);
		   }

		   
		   if (west != null && !obstacles.contains(getValueAt(west)) && !closed.contains(west)) {
			   open.add(west);
		   }
		   else if(west != null && obstacles.contains(getValueAt(west)) && getValueAt(west) != '.' && getValueAt(west) != '#') {
			   border.add(west);
		   }
		   
//		   System.out.println("o: "+open);		   
//		   System.out.println("b: "+border);


		   open.remove(current);
		   closed.add(current);
	   }
	   
	   return border;
   }
   
   public Position removeOptimalWall(List<Position> border) {

	   Position candidate = null;
	   int candidateManDist = Integer.MAX_VALUE;
	   
	   for (Position b: border) {
//		   System.out.println(removable+" b: "+b);
		   if (removable.contains(b.getValueAt())) {
//			   System.out.println(b);
			   if (b.north() != null && b.south() != null) {
//				   System.out.println(b);
//				   System.out.println(b.north().getValueAt()+" "+b.south().getValueAt());
				   if(!obstacles.contains(b.north().getValueAt()) && !obstacles.contains(b.south().getValueAt())) {
//					   System.out.println("yes");
					   if (		(checkReachable(b.north(), objective) && manDistTo(worldPosition, b.south(),orientation) < candidateManDist) ||
							   	(checkReachable(b.south(), objective) && manDistTo(worldPosition, b.north(),orientation) < candidateManDist)) 
					   {
//						   System.out.println("yes2 wp "+worldPosition+" b "+b.south()+" "+manDistTo(worldPosition, b.south(),9));
						   candidate = b;
						   candidateManDist = manDistTo(worldPosition, b,orientation);
					   }
				   }
			   }
			   
			   if (b.east() != null && b.west() != null) {
//				   System.out.println(b);
//				   System.out.println(b.east().getValueAt()+" "+b.west().getValueAt());
				   if(!obstacles.contains(b.east().getValueAt()) && !obstacles.contains(b.west().getValueAt())) {
					   if (		(checkReachable(b.east(), objective) && manDistTo(worldPosition, b.west(),orientation) < candidateManDist) ||
							   	(checkReachable(b.west(), objective) && manDistTo(worldPosition, b.east(),orientation) < candidateManDist)) 
					   {
						   candidate = b;
						   candidateManDist = manDistTo(worldPosition, b,orientation);
					   }
				   }
			   }
		   }
		   
		   
		   
	   }
	   
//	   if (!candidate.equals(worldPosition)) {
//		   objective = candidate;
//	   }
	   return candidate;
   }
   
   public Position bestExploreScore() {
	   Position candidate = new Position(objective.getRow(),objective.getCol());
	   int exploreScore = 0;
	   
	   for (int i = 0; i< world.size(); i++) {
		   for (int j = 0; j < world.get(0).size(); j++) {
			   Position nextPosition = new Position(i,j);
			   int nextScore = exploreScore(nextPosition);
//			   if (exploreScore(nextPosition) > 0) {
//				   System.out.println(checkReachable(worldPosition,nextPosition)+" np "+nextPosition+" Explore Score: "+exploreScore(nextPosition));
//			   }
			   
			   if (nextScore > exploreScore && !obstacles.contains(nextPosition.getValueAt()) && checkReachable(worldPosition,nextPosition)) {
				   
				   exploreScore = nextScore;
				   candidate = nextPosition;
			   }
			   
		   }
	   }
	   
	   
	   return candidate;
	   
   }
   
   
   public char move(char[][] view) {
	   if (have_treasure) {
		   world.get(worldPosition.getRow()-positionRelativeToStart[0]).set(worldPosition.getCol()-positionRelativeToStart[1], 'S');
//		   symbolValues.put('S', Integer.MAX_VALUE);
		   symbolValues.put('S', 2);
//		   print_world();
//		   System.out.println(positionRelativeToStart[0]+" "+positionRelativeToStart[1]);
//		   System.exit(-1);
	   }
	   
	   
	   if (!nextMoves.isEmpty() && nextMoves.get(nextMoves.size()-1) == 'F' && obstacles.contains(getValueAt(new int[]{worldPosition.getRow() + forward.get(orientation)[0],worldPosition.getCol() + forward.get(orientation)[1]}))) {
		   nextMoves = null;
//		   move();
	   }
	   
	   
	   if (nextMoves == null || nextMoves.isEmpty()) {
		   
		   findObjective();
		   Position initialObjective = new Position(objective.getRow(),objective.getCol());
		   
		   System.out.println("new objective: "+objective.toString());
		   if (!checkReachable(worldPosition,objective)) {
//			   objective = findNearestTo(objective);
			   objective = findNearestReachableEdge(objective);
			   System.out.println("Nearest Edge: "+objective.toString());
			   
			   if (objective.equals(worldPosition)) {
				   objective = bestExploreScore();		
				   System.out.println("Best Explore Score: "+objective.toString());
				   
				   if (objective.equals(worldPosition)) {
					   System.out.println("Objective: "+initialObjective);
					   List<Position> border = findBorder(initialObjective);
					   System.out.println("Border: "+border);
					   objective = removeOptimalWall(border);
					   System.out.println("Best border: "+objective.toString());
				   }

			   }
			   
		   }

		   nextMoves = astar(worldPosition, objective);
//		   System.out.println(nextMoves);
//		   System.out.println(worldPosition[0]+" "+worldPosition[1]+" "+objective[0]+" "+objective[1]);
		   

	   }
	   System.out.println(nextMoves);
	   System.out.println("Next move: "+nextMoves.get(nextMoves.size()-1));
	   
	   char move = nextMoves.remove(nextMoves.size()-1);
	   
     updateOrientation(move);
     lastMoveSuccessful = successfulMove(move, view);
     System.out.println(on_water+" "+lastMoveSuccessful+" "+view[1][2]);
	   
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
			   removable.add('T');
//			   System.out.println("GOT AXE!");
			   symbolValues.put('a',0);
			   symbolValues.put('T',2);
			   
			   if (on_water == true) {
				   System.out.println("lost raft");
				   on_water = false;
				   symbolValues.put('~',-1);
				   obstacles.add('~');
				   symbolValues.put('T',2);
			   }
			   
			   return 3;
		   }
		   else if (view[1][2] == 'k') {
			   have_key = true;
			   symbolValues.put('k',0);
			   symbolValues.put('-',2);
			   if (on_water == true) {
				   System.out.println("lost raft");
				   on_water = false;
				   symbolValues.put('~',-1);
				   obstacles.add('~');
				   symbolValues.put('T',2);
			   }
			   
			   return 3;
		   }
		   else if (view[1][2] == '$') {
			   symbolValues.put('$',0);
			   have_treasure = true;
			   
			   if (on_water == true) {
				   System.out.println("lost raft");
				   on_water = false;
				   symbolValues.put('~',-1);
				   obstacles.add('~');
				   symbolValues.put('T',2);
			   }
			   return 3;
		   }
		   else if (view[1][2] == 'd') {
			   removable.add('*');
			   removable.add('T');
			   if (symbolValues.get('T') < 2) {
				   symbolValues.put('T',1);
			   }
			   
			   if (on_water == true) {
				   System.out.println("lost raft");
				   on_water = false;
				   symbolValues.put('~',-1);
				   obstacles.add('~');
				   symbolValues.put('T',2);
			   }
			   
//			   System.out.println("GOT DYNAMITE!");
			   num_dynamites_held++;
			   return 3;
		   }
//		   else if ((view[1][2] == ' ' || view[1][2] == '$' || view[1][2] == 'a' || view[1][2] == 'd' || view[1][2] == 'k') 
//				   && on_water == true) {
//			   System.out.println("lost raft");
//			   on_water = false;
//			   symbolValues.put('~',-1);
//			   obstacles.add('~');
//			   symbolValues.put('T',2);
//			   return 3;
//		   }
		   else if (view[1][2] == '~') {
//			   symbolValues.put('~',0);
//			   System.out.println("here4");
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
		   symbolValues.put('~',0);
		   obstacles.remove('~');
//		   have_raft = true;
		   symbolValues.put('T',0);
		   return 2;
	   }
	   else if ((view[1][2] == '*' || view[1][2] == 'T' || view[1][2] == '-') && num_dynamites_held > 0 && (move == 'b' || move == 'B')) {
		   num_dynamites_held--;
		   System.out.println("BOOM");
		   if (num_dynamites_held == 0) {
			   System.out.println("No more dynamite :(");
			   removable.remove('*');
			   if (!have_axe) {
				   removable.remove('T');
			   }
			   if (!have_key) {
				   removable.remove('-');
			   }
		   }
		   return 2;
	   }

	   return 0;		   
   }
   
   public void initialiseWorld(char view[][]) {
	// Create first view of world
//	   System.out.println("here");
	   for(int i = 0; i < 5; i++)  {
		   for(int j = 0; j < 5; j++)  {
			   world.get(i).add(j,view[i][j]);
		   }	   
	    }
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
		   List<Character> FOV = world.get(worldPosition.getRow() + forward.get(orientation)[0] * 2);

		   for (int i = 0; i < FOV.size(); i++) {
			   if (worldPosition.getCol()-2 <= i && i <= worldPosition.getCol()+2 && count < 5) {
				   FOV.set(i, view[0][count]);
				   count++;
			   }

		   }
	   }
	   else if (orientation == 2) {
		   List<Character> FOV = world.get(worldPosition.getRow() + forward.get(orientation)[0] * 2);

		   for (int i = FOV.size() - 1; i >= 0; i--) {
			   if (worldPosition.getCol()-2 <= i && i <= worldPosition.getCol()+2 && count < 5) {
				   FOV.set(i, view[0][count]);
				   count++;
			   }

		   }
		   
	   }
	   else if (orientation == 1) {
		   for (int i = 0; i < world.size(); i++) {
			   if (worldPosition.getRow()-2 <= i && i <= worldPosition.getRow()+2 && count < 5) {
				   world.get(i).set(worldPosition.getCol() + forward.get(orientation)[1] * 2, view[0][count]);
				   count++;
			   }
		   }
	   }
	   else if (orientation == 3) {
		   for (int i = world.size() - 1; i >=0; i--) {
			   if (worldPosition.getRow()-2 <= i && i <= worldPosition.getRow()+2 && count < 5) {
				   world.get(i).set(worldPosition.getCol() + forward.get(orientation)[1] * 2, view[0][count]);
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
			   world.get(worldPosition.getRow() + position[0]).set(worldPosition.getCol() + position[1], ' ');   
		   }
		   else {
			   int count = 0;
			   updateWorldPosition();

			   if (needExpansion()) {
				   expandedUpRightDownLeft[orientation]++;

				   if (orientation == 0 ) {
					   List<Character> newRow = new ArrayList<Character>(Collections.nCopies(world.get(0).size(), '#'));

					   for (int i = 0; i < newRow.size(); i++) {
						   if (worldPosition.getCol()-2 <= i && i <= worldPosition.getCol()+2 && count < 5) {
							   newRow.set(i, view[0][count]);
							   count++;
						   }
					   }
					   world.add(0, newRow);
				   }
				   else if (orientation == 2) {
					   List<Character> newRow = new ArrayList<Character>(Collections.nCopies(world.get(0).size(), '#'));

					   for (int i = newRow.size() -1 ; i >= 0 ; i--) {
						   if (worldPosition.getCol()-2 <= i && i <= worldPosition.getCol()+2 && count < 5) {
							   
							   newRow.set(i, view[0][count]);
							   count++;
						   }
					   }
					   world.add(newRow);
				   }
				   else if (orientation == 1) {
					   for (int i = 0; i < world.size(); i++) {
						   if (worldPosition.getRow()-2 <= i && i <= worldPosition.getRow()+2 && count < 5) {
							   world.get(i).add(view[0][count]);
							   count++;
						   }
						   else {
							   world.get(i).add('#');
						   }
					   }
				   }
				   else if (orientation == 3) {
					   for (int i = world.size() - 1; i >=0; i--) {
						   if (worldPosition.getRow()-2 <= i && i <= worldPosition.getRow()+2 && count < 5) {
							   world.get(i).add(0, view[0][count]);
							   count++;
						   }
						   else {
							   world.get(i).add(0,'#');
						   }
					   }
				   }
				   count = 0;
		   
			   }
			   updateFOV(view);
			   
			   if (successfulMove == 3) {
				   
				   world.get(worldPosition.getRow()).set(worldPosition.getCol(), ' ');
			   }
		   }
		   
	   }
   }
   
   public void updateWorldPosition() {
	   positionRelativeToStart[0] += forward.get(orientation)[0];
	   positionRelativeToStart[1] += forward.get(orientation)[1];
	   
	   if (!((worldPosition.getRow() <= 2 && orientation == 0) ||  (worldPosition.getCol() <= 2 && orientation == 3) )) {
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
      
      char[] dashArray = new char[world.get(0).size()];
      Arrays.fill(dashArray, '-');
      String dashes = new String(dashArray);
      
      System.out.println("\n+"+dashes+"+");
      for( i=0; i < world.size(); i++ ) {
         System.out.print("|");
         for( j=0; j < world.get(0).size(); j++ ) {
            if(( i == worldPosition.getRow() )&&( j == worldPosition.getCol() )) {
               System.out.print(orientationSymbol[orientation]);
            }
            else {
               System.out.print( world.get(i).get(j) );
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
            agent.print_view( view ); // COMMENT THIS OUT BEFORE SUBMISSION
            
            if (agent.world.get(0).isEmpty()) {
            	agent.initialiseWorld(view);
            }
            else {
            	agent.updateWorld(view, agent.lastMoveSuccessful);
            }
            
            agent.print_world();
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
