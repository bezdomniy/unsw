
/*********************************************
 *  Agent.java 
 *  Agent for Text-Based Adventure Game
 *  COMP9414 Artificial Intelligence
 *  UNSW Session 1, 2017
 *  
 *  My algorithm works by exploring the map until the treasure is sighted, at which point the agent tries to find a path to the objective. 
 *  Exploration works by scanning the map at every turn and assigning an exploreScore to each Position. This score is based on the number
 *  of tiles that are expected to be revealed by moving the agent to this position. It then divides each score by the Manhattan distance 
 *  between the agent and the position in order to priorities the exploration of nearby positions. It then uses a breadth-first search algorithm
 *  to find the set of positions which are reachable and sets the maximum value of these positions as the objective and uses A* search to define 
 *  the path. In order to avoid wasteful tree chopping and unnecessary switching between land and water the findReachableTiles method can take 4
 *  forms: 0: not using or removing any finite items, 1: with tree chopping, 2: switching between land and water, 3: allowing use of bombs.
 *  
 *  Once the treasure is sighted an A* search is initiated to find a path to the objective. The A* algorithm places each Position into a Node
 *  which holds the Position, the WorldState at that node, and the agent's inventory (raft, key, etc.) at that point. For the purpose of the A*
 *  algorithm a Node is equal to another if its Position, agent orientation, and WorldState are equal. Each valid action in the game is a possible
 *  new Node in the graph, with the exception of placing a bomb if the method is called with the argument exploringAstar = true . The move cost (g)
 *  is set to 1 for each move, but in order to discourage unnecessary use of finite items, costs for certain types of actions are higher. 
 *  
 *  If no path is found then the agent does not attempt to reach the treasure again until a new useful item (key, axe, dynamite etc.) is found, 
 *  or it has exhausted all possible Positions for exploration. If no path to the treasure is found at this stage, the algorithm sets other
 *  useful items as goals. For example more dynamite, or an axe if the agent does not yet have one. After this it again tries to find the path
 *  to the treasure.
 *  
 *  This algorithm is able to solve all sample maps in under 2 minutes (all except s7.in in under 1 minute) on my Intel i5, 8gb RAM computer.
 *  Before submitting, I tested it on the CSE machines and found they are significantly slower at running my A* algorithm, so I have found that
 *  it may not work for all maps. This is due to the algorithm trying to find paths to the treasure by first picking up objects (like dynamite) 
 *  required to attain the path. This leads to the expansion of a couple of thousand nodes for some searches which seems to be too much for the
 *  CSE machines. I could not thing of another way to implement this, which works for maps such as s8 and s9 which require walls to be bombed in
 *  a correct order.
*/

import java.util.*;
import java.util.Map.Entry;
import java.util.stream.Collectors;
import java.io.*;
import java.net.*;

public class Agent {
	private boolean have_axe = false;
	private boolean have_key = false;
	private boolean have_treasure = false;
	private boolean on_water = false;
	private boolean have_raft = false;
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

	private WorldState world;

	private int orientation = 0;
	private char[] orientationSymbol = { '^', '>', 'v', '<' };

	private int lastMoveSuccessful = 0;
	// private char lastMove = ' ';

	private Position objective = new Position(2, 2);
	private List<Position> objectives = new ArrayList<Position>();

	private List<Character> nextMoves = new ArrayList<Character>();
	Set<Character> obstacles = new HashSet<Character>();

	// not used
	Set<Character> removable = new HashSet<Character>();

	private Position worldPosition = new Position(2, 2);

	private int[] positionRelativeToStart = { 0, 0 };
	private int[] expandedUpRightDownLeft = { 0, 0, 0, 0 };

	private Map<Integer, Integer[]> forward = new HashMap<Integer, Integer[]>();

	public Agent() {
		symbolValues.put('a', 0);
		symbolValues.put('d', 0);
		symbolValues.put('k', 0);
		symbolValues.put('T', 0);
		symbolValues.put('$', 2);

		symbolValues.put(' ', 0);
		symbolValues.put('#', 0);

		forward.put(0, new Integer[] { -1, 0 });
		forward.put(1, new Integer[] { 0, 1 });
		forward.put(2, new Integer[] { 1, 0 });
		forward.put(3, new Integer[] { 0, -1 });

		obstacles.add('-');
		obstacles.add('*');
		obstacles.add('T');
		obstacles.add('~');
		// obstacles.add('#');
		obstacles.add('.');

		List<List<Position>> w = new ArrayList<List<Position>>();

		for (int i = 0; i < 5; i++) {
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

		public Position(int row, int col, Character value) {
			this.row = row;
			this.col = col;
			this.value = value;

		}

		public Position(Character value) {
			this.value = value;
		}

		public Position clone() {
			Position ret = new Position(this.row, this.col, this.value);
			ret.setReachable(this.reachable);
			return ret;
		}

		@Override
		public boolean equals(Object object) {
			return this.row == ((Position) object).getRow() && this.col == ((Position) object).getCol();

		}

		@Override
		public int hashCode() {
			return Objects.hash(this.row, this.col);
		}

		@Override
		public String toString() {
			return String.format("[" + this.row + "," + this.col + "]");

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
		}

		public Position north() {
			if (this.getRow() - 1 >= 0) {
				return world.getCell(this.getRow() - 1, this.getCol());
			}
			return null;
		}

		public Position south() {
			if (this.getRow() + 1 < world.numRows()) {
				return world.getCell(this.getRow() + 1, this.getCol());
			}
			return null;
		}

		public Position east() {
			if (this.getCol() + 1 < world.numCols()) {
				return world.getCell(this.getRow(), this.getCol() + 1);
			}
			return null;
		}

		public Position west() {
			if (this.getCol() - 1 >= 0) {
				return world.getCell(this.getRow(), this.getCol() - 1);
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
			this.exploreScore = 0d;
			if (world != null) {
				for (int i = this.getRow() - 2; i <= this.getRow() + 2; i++) {
					for (int j = this.getCol() - 2; j <= this.getCol() + 2; j++) {
						if (i == 0 || i == world.numRows() - 1 || i == world.numRows() - 2) {
							long boundaryTiles = world.getRow(i).stream().map(Position::getValue).filter(c -> '.' == c)
									.count();
							if (boundaryTiles > 4d) {
								i++;
								break;
							}
						}

						if (i < 0 || i >= world.numRows()) {
							this.exploreScore += 5d;
							break;
						} else {

						}

						if (j < 0 || j >= world.numCols()) {
							this.exploreScore += 1d;
						} else if (world.getCell(i, j).getValue() == '#') {
							this.exploreScore += 1d;
						}
					}
				}
			}

			if (this.value == 'd' || (this.value == 'a' && !have_axe) || (this.value == 'k' && !have_key)) {
				this.exploreScore += 2;
			}

			return this.exploreScore / (double) manDistTo(worldPosition, this, orientation);
		}

	}

	public class WorldState {
		private List<List<Position>> worldModel = new ArrayList<List<Position>>();

		public WorldState(WorldState parentWorld) {
			for (int i = 0; i < parentWorld.numRows(); i++) {
				worldModel.add(new ArrayList<Position>());

				for (int j = 0; j < parentWorld.numCols(); j++) {
					worldModel.get(i).add(parentWorld.worldModel.get(i).get(j).clone());
				}
			}
		}

		public WorldState(List<List<Position>> parentWorld) {
			for (int i = 0; i < parentWorld.size(); i++) {
				worldModel.add(new ArrayList<Position>());

				for (int j = 0; j < parentWorld.get(0).size(); j++) {
					this.worldModel.get(i).add(parentWorld.get(i).get(j).clone());
				}
			}
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
			} catch (IndexOutOfBoundsException e) {
				return false;
			}

		}

		@Override
		public boolean equals(Object object) {
			try {
				if (object != null && object instanceof WorldState) {
					for (int i = 0; i < this.numRows(); i++) {
						for (int j = 0; j < this.numCols(); j++) {
							if (this.getCell(i, j).getValue() != ((WorldState) object).getCell(i, j).getValue()) {
								return false;
							}

						}
					}
				}
			} catch (IndexOutOfBoundsException e) {
				return false;
			}

			return true;
		}

		@Override
		public int hashCode() {
			StringBuilder str = new StringBuilder();

			for (int i = 0; i < this.numRows(); i++) {
				for (int j = 0; j < this.numCols(); j++) {
					str.append(this.getCell(i, j).getValue());

				}
			}

			str.toString();

			return Objects.hash(str.toString());

		}

		public Position getCell(int row, int col) {
			try {

				return worldModel.get(row).get(col);
			} catch (IndexOutOfBoundsException e) {
				return (Position) null;
			}

		}

		public List<Position> getRow(int row) {
			return this.worldModel.get(row);

		}

		public void addPosition(int row, int col, Position pos) {
			this.worldModel.get(row).add(col, pos);
		}

		public void addPosition(int row, Position pos) {
			this.worldModel.get(row).add(pos);
		}

		public void addRow(char edge, List<Position> row) {
			if (edge == 'T') {
				this.worldModel.add(0, row);
			} else if (edge == 'B') {
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
			int i, j;

			char[] dashArray = new char[this.numCols()];
			Arrays.fill(dashArray, '-');
			String dashes = new String(dashArray);

			System.out.println("\n+" + dashes + "+");
			for (i = 0; i < this.numRows(); i++) {
				System.out.print("|");
				for (j = 0; j < this.numCols(); j++) {
					if ((i == worldPosition.getRow()) && (j == worldPosition.getCol())) {
						System.out.print(orientationSymbol[orientation]);
					} else {
						System.out.print(getCell(i, j).getValue());
					}
				}
				System.out.println("|");
			}
			System.out.println("+" + dashes + "+");
		}

	}

	public class Node {
		private Position position;
		private WorldState nodeWorld;
		private Set<Character> nodeObstacles = new HashSet<Character>();

		private boolean haveAxe = have_axe;
		private boolean haveKey = have_key;
		private boolean haveTreasure = have_treasure;
		private boolean onWater = on_water;
		private boolean haveRaft = have_raft;
		private int numDynamitesHeld = num_dynamites_held;

		private double g;
		private double h;
		private double f;
		private int orientation;
		private Character symbol;

		private Character moveToReach = null;

		private List<Node> children = new ArrayList<Node>();

		public Node(Position position, Character symbol, int orientation, Node parent) {
			this.position = position;
			this.symbol = symbol;
			this.orientation = orientation;
			// this.value = position.getValueAt();

			if (parent != null) {
				this.haveAxe = parent.haveAxe;
				this.haveKey = parent.haveKey;
				this.haveTreasure = parent.haveTreasure;
				this.onWater = parent.onWater;
				this.haveRaft = parent.haveRaft;
				this.numDynamitesHeld = parent.numDynamitesHeld;
				this.nodeWorld = parent.nodeWorld.clone();

				for (Character c : parent.nodeObstacles) {
					this.nodeObstacles.add(c);
				}
			} else {
				this.nodeWorld = new WorldState(world);

				for (Character c : obstacles) {
					this.nodeObstacles.add(c);
				}
			}

		}

		@Override
		public boolean equals(Object object) {
			if (object != null && object instanceof Node) {
				if (this.getForwardPosition() != null) {

					return this.position.equals(((Node) object).getPosition())
							&& this.orientation == ((Node) object).getOrientation()
							&& this.nodeWorld.equals(((Node) object).nodeWorld);
				}
				return this.position.equals(((Node) object).getPosition())
						&& this.orientation == ((Node) object).getOrientation();
			}

			return false;
		}

		@Override
		public int hashCode() {
			if (this.getForwardPosition() != null) {
				return Objects.hash(this.position, this.orientation, this.nodeWorld);
			}
			return Objects.hash(this.position, this.orientation);

		}

		@Override
		public String toString() {
			return String.format(
					"Pos: " + this.position.getRow() + " " + this.position.getCol() + ", Ori: " + this.orientation);

		}

		public Position getForwardPosition() {
			Position forwardPosition = new Position(this.position.getRow() + forward.get(this.orientation)[0],
					this.position.getCol() + forward.get(this.orientation)[1]);
			if (forwardPosition.getRow() >= 0 && forwardPosition.getCol() >= 0
					&& forwardPosition.getRow() < this.nodeWorld.numRows()
					&& forwardPosition.getCol() < this.nodeWorld.numCols()) {
				return forwardPosition;
			}
			return null;
		}

		public Position getBackwardPosition() {
			Position backwardPosition = new Position(this.position.getRow() - forward.get(this.orientation)[0],
					this.position.getCol() - forward.get(this.orientation)[1]);
			if (backwardPosition.getRow() >= 0 && backwardPosition.getCol() >= 0
					&& backwardPosition.getRow() < this.nodeWorld.numRows()
					&& backwardPosition.getCol() < this.nodeWorld.numCols()) {
				return backwardPosition;
			}
			return null;
		}

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

		public void setH(double d) {
			this.h = d * (1.001d);
		}

		public void setG(double temp_g) {
			this.g = temp_g;
		}

		public void updateF() {
			this.f = this.g + this.h;
		}

		public void updateF(double w) {
			this.f = (2 - w) * this.g + w * this.h;
		}

		public double getH() {
			return this.h;
		}

		public double getG() {
			return this.g;
		}

		public double getF() {
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
			this.position = new Position(row, col);
		}

		public int getOrientation() {
			return this.orientation;
		}

		public void setOrientation(int o) {
			this.orientation = o;
			;
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
			int i, j;

			char[] dashArray = new char[this.nodeWorld.numCols()];
			Arrays.fill(dashArray, '-');
			String dashes = new String(dashArray);

			System.out.println("\n+" + dashes + "+");
			for (i = 0; i < this.nodeWorld.numRows(); i++) {
				System.out.print("|");
				for (j = 0; j < this.nodeWorld.numCols(); j++) {
					if ((i == this.getPosition().getRow()) && (j == this.getPosition().getCol())) {
						System.out.print(orientationSymbol[this.orientation]);
					} else {
						System.out.print(this.nodeWorld.getCell(i, j).getValue());
					}
				}
				System.out.println("|");
			}
			System.out.println("+" + dashes + "+");
		}

	}

	public class fScoreComparator implements Comparator<Node> {
		@Override
		public int compare(Node n1, Node n2) {
			Double f1 = (Double) n1.getF();
			Double f2 = (Double) n2.getF();

			return f1.compareTo(f2);
		}
	}

	public char getValueAt(int[] position) {
		return world.getCell(position[0], position[1]).getValue();
	}

	public char getValueAt(Position position) {
		return world.getCell(position.getRow(), position.getCol()).getValue();
	}

	public List<Character> make_path(Map<Node, Node> origins, Node position) {

		int goalOrientation = position.getOrientation();

		List<Character> path = new ArrayList<Character>();
		path.add(position.getMove());

		while (origins.containsKey(position)) {
			position = origins.get(position);

			if (position.getMove() == null) {
				break;
			}

			path.add(position.getMove());
		}

		path.add((char) goalOrientation);
		return path;
	}

	public List<Character> astar(Position position, Position goal, int startOrientation, boolean exploringAstar,
			boolean findPartial, float w, double d, boolean showMaps) {

		if (goal.getValue() == '$') {
			d *= 2;
		}

		double startTime = System.currentTimeMillis();
		double endTime = startTime + d * 1000d;

		Node start = new Node(position, getValueAt(position), startOrientation, null);

		start.removeObstacle(' ');

		if (!exploringAstar) {
			start.addObstacle('#');
		}

		Set<Node> closed = new HashSet<Node>();
		Set<Node> open = new HashSet<Node>();
		open.add(start);

		Map<Node, Node> origins = new HashMap<Node, Node>();

		start.setG(0d);
		start.setH(manDistTo(position, goal, start.getOrientation()));
		start.updateF();

		Node current = null;

		while (!open.isEmpty() && System.currentTimeMillis() < endTime) {
			current = Collections.min(open, new fScoreComparator());
			if (current.getPosition().equals(goal)) {
				// System.out.println("nodes opened " + co);
				return make_path(origins, current);
			}

			Position forwardPosition = current.getForwardPosition();
			if (forwardPosition != null) {
				char forwardSymbol = current.nodeWorld.getCell(forwardPosition.getRow(), forwardPosition.getCol())
						.getValue();

				Node forw = new Node(forwardPosition, forwardSymbol, current.getOrientation(), current);

				if (!current.nodeObstacles.contains(forwardSymbol) || (forwardSymbol == '~' && current.haveRaft)
						|| (forwardPosition.equals(goal) && goal.getValue() == '#')) {
					forw.setMove('F');

					if (forwardSymbol == 'd') {
						forw.addDynamite();
						forw.nodeWorld.updateCell(forwardPosition.getRow(), forwardPosition.getCol(), ' ');
					} else if (forwardSymbol == 'k') {
						forw.setHaveKey(true);
					} else if (forwardSymbol == 'a') {
						forw.setHaveAxe(true);
					} else if (forwardSymbol == '$') {
						forw.setHaveTreasure(true);
					} else if (current.getSymbol() == '~' && (forwardSymbol == ' ' || forwardSymbol == '$'
							|| forwardSymbol == 'a' || forwardSymbol == 'd'
							|| forwardSymbol == 'k' /*
													 * || forwardSymbol == '#'
													 */ )) {
						forw.addObstacle('~');

						forw.setHaveRaft(false);
					}

					if (forw.nodeWorld.getCell(forwardPosition.getRow(), forwardPosition.getCol()).getValue() != '~') {
						forw.nodeWorld.updateCell(forwardPosition.getRow(), forwardPosition.getCol(), ' ');
					}

					current.addChild(forw);

				} else if (forwardSymbol == '-' && current.haveKey) {
					forw.nodeWorld.updateCell(forw.getPosition().getRow(), forw.getPosition().getCol(), ' ');
					forw.setPosition(current.getPosition());

					forw.setMove('U');
					current.addChild(forw);
				} else if (forwardSymbol == 'T' && current.haveAxe) {
					forw.nodeWorld.updateCell(forw.getPosition().getRow(), forw.getPosition().getCol(), ' ');
					forw.setPosition(current.getPosition());

					forw.setHaveRaft(true);

					forw.setMove('C');
					current.addChild(forw);
				} else if (!exploringAstar && (forwardSymbol == '*' || forwardSymbol == 'T' || forwardSymbol == '-')
						&& forw.numDynamitesHeld > 0) {

					forw.removeDynamite();
					forw.nodeWorld.updateCell(forw.getPosition().getRow(), forw.getPosition().getCol(), ' ');
					forw.setPosition(current.getPosition());

					forw.setMove('B');
					current.addChild(forw);

				}

			}

			Node left = new Node(current.getPosition(), current.getSymbol(),
					checkOrientation('L', current.getOrientation()), current);
			Node right = new Node(current.getPosition(), current.getSymbol(),
					checkOrientation('R', current.getOrientation()), current);
			left.setMove('L');
			right.setMove('R');

			current.addChild(left);
			current.addChild(right);

			open.remove(current);
			closed.add(current);

			for (Node child : current.getChildren()) {

				if (closed.contains(child)) {
					continue;
				}

				int moveCost = 1;
				if (exploringAstar) {
					if (child.getMove() == 'B') {
						moveCost = 10;
						if (current.getSymbol() == '~') {
							moveCost = +10;
						}

					} else if (child.getMove() == 'C' && child.haveRaft) {
						moveCost = 10;

						if (current.getSymbol() == '~') {
							moveCost += 15;
						}
					}

					if (current.getSymbol() == '~' && child.getSymbol() == ' ') {
						moveCost += 10;
					}
					if (current.getSymbol() == ' ' && child.getSymbol() == '~') {
						moveCost += 10;
					}
					if (child.getSymbol() == '#') {
						moveCost += 5;
					}
				} else {
					if (child.getMove() == 'B') {
						moveCost = 3;
						if (current.getSymbol() == '~') {
							moveCost = 100;
						}
					}
				}

				double temp_g = current.getG() + moveCost;

				if (temp_g < child.getG() || !open.contains(child)) {
					origins.put(child, current);

					if (showMaps) {
						System.out.println("******** Current: " + current.numDynamitesHeld);
						current.printWorld();
						System.out.println("******** child: " + child.numDynamitesHeld);
						child.printWorld();
						System.out.println("****************");
					}

					child.setG(temp_g);
					child.setH(manDistTo(child.getPosition(), goal, 9));
					child.updateF(w);
					if (!open.contains(child)) {
						open.add(child);
					}
				}

			}

		}

		if (findPartial && !open.isEmpty()) {
			return make_path(origins, current);
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

		for (int i = 0; i < world.numRows(); i++) {
			for (int j = 0; j < world.numCols(); j++) {
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

		if (manDist == 0 || orientation == 9)
			return manDist;

		if (position.getCol() == objective.getCol() && orientation == 0 && position.getRow() > objective.getRow()) {
			turnCost = 0;
		} else if (position.getCol() == objective.getCol() && orientation == 2
				&& position.getRow() < objective.getRow()) {
			turnCost = 0;
		} else if (position.getRow() == objective.getRow() && orientation == 3
				&& position.getCol() > objective.getCol()) {
			turnCost = 0;
		} else if (position.getRow() == objective.getRow() && orientation == 1
				&& position.getCol() < objective.getCol()) {
			turnCost = 0;
		} else if (position.getRow() >= objective.getRow() && (orientation == 0)) {
			turnCost = 1;
		} else if (position.getRow() <= objective.getRow() && (orientation == 2)) {
			turnCost = 1;
		} else if (position.getCol() <= objective.getCol() && (orientation == 1)) {
			turnCost = 1;
		} else if (position.getCol() >= objective.getCol() && (orientation == 3)) {
			turnCost = 1;
		} else {
			turnCost = 2;
		}

		return manDist + turnCost;

	}

	public List<Position> findObjective() {

		Map<Position, Double> allCandidates = new HashMap<Position, Double>();

		double highestValue = 0d;
		double nextValue = 0d;

		for (int i = 0; i < world.numRows(); i++) {
			for (int j = 0; j < world.numCols(); j++) {
				if (symbolValues.get(world.getCell(i, j).getValue()) != null) {
					Position nextPosition = world.getCell(i, j);
					nextValue = (double) symbolValues.get(nextPosition.getValue())
							/ (double) manDistTo(worldPosition, nextPosition, orientation);
					// System.out.println("position: "+nextPosition+" value:
					// "+nextValue);
					if (nextValue > 0d) {
						highestValue = nextValue;
						// allCandidates.add(nextPosition);
						allCandidates.put(nextPosition, highestValue);
					}
				}

			}
		}

		List<Position> output = allCandidates.entrySet().stream()
				.sorted(Entry.comparingByValue(Collections.reverseOrder())).map(Entry::getKey)
				.collect(Collectors.toList());

		if (highestValue > 0d) {
			return output;

		}
		return new ArrayList<Position>();

	}

	public List<Position> findReachableTiles(Position position, int iteration) {
		// Iteration:
		// 0: No chop, blow, or move to water
		// 1: Allow chop
		// 2: Allow move to water or from water
		// 3: Allow blow

		Map<Integer, Set<Character>> boundaries = new HashMap<Integer, Set<Character>>();

		boundaries.put(0, new HashSet<Character>());
		for (Character c : obstacles) {
			boundaries.get(0).add(c);

		}
		boundaries.get(0).add('#');
		if (have_key) {
			boundaries.get(0).remove('-');
		}

		if (!on_water) {
			boundaries.get(0).add('~');
		} else {
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
		for (Character c : boundaries.get(0)) {
			boundaries.get(1).add(c);
		}
		if (!on_water) {
			boundaries.get(1).remove('T');
		}

		boundaries.put(2, new HashSet<Character>());

		for (Character c : boundaries.get(1)) {
			boundaries.get(2).add(c);
		}
		if (on_water) {
			boundaries.get(2).remove(' ');
			boundaries.get(2).remove('T');
		} else {
			boundaries.get(2).remove('~');
		}

		boundaries.put(3, new HashSet<Character>());

		boundaries.put(3, new HashSet<Character>());
		for (Character c : obstacles) {
			boundaries.get(3).add(c);

		}
		boundaries.get(3).add('#');
		boundaries.get(3).remove('*');
		boundaries.get(3).remove('-');
		boundaries.get(3).remove('T');
		if (!have_raft) {
			boundaries.get(3).add('~');
		}

		if (boundaries.get(iteration).contains(position)) {
			return null;
		}

		Set<Position> open = new HashSet<Position>();
		Set<Position> closed = new HashSet<Position>();
		Map<Position, Double> reachable = new HashMap<Position, Double>();

		open.add(position);
		Double score = null;

		while (!open.isEmpty()) {
			Iterator<Position> o = open.iterator();

			Position current = o.next();

			Position north = current.north();
			Position south = current.south();
			Position east = current.east();
			Position west = current.west();

			if (iteration != 2 || (on_water && getValueAt(current) == '~')
					|| (!on_water && getValueAt(current) != '~')) {
				if (north != null && (!boundaries.get(iteration).contains(getValueAt(north)))
						&& !closed.contains(north)) {
					open.add(north);
				}

				if (south != null && (!boundaries.get(iteration).contains(getValueAt(south)))
						&& !closed.contains(south)) {
					open.add(south);
				}

				if (east != null && (!boundaries.get(iteration).contains(getValueAt(east))) && !closed.contains(east)) {
					open.add(east);
				}

				if (west != null && (!boundaries.get(iteration).contains(getValueAt(west))) && !closed.contains(west)) {
					open.add(west);

				}
			}

			if (!position.equals(current)) {
				score = current.findExploreScore();
			}

			if (score != null && score > 0d) {
				reachable.put(current, score);
			}

			open.remove(current);
			closed.add(current);
		}

		List<Position> output = reachable.entrySet().stream()
				.sorted(Entry.comparingByValue(/* Collections.reverseOrder() */)).map(Entry::getKey)
				.collect(Collectors.toList());

		return output;
	}

	// Check if a position is reachable without expending finite items.
	public boolean checkReachable(Position from, Position position) {
		if (obstacles.contains(position) && !(getValueAt(position) == '-' && have_key)) {
			return false;
		}

		Set<Position> open = new HashSet<Position>();
		Set<Position> closed = new HashSet<Position>();

		open.add(position);

		while (!open.isEmpty()) {
			Iterator<Position> o = open.iterator();

			Position current = o.next();

			if (current.equals(from)) {
				return true;
			}

			Position north = current.north();
			Position south = current.south();
			Position east = current.east();
			Position west = current.west();

			if (north != null && (!obstacles.contains(getValueAt(north)) || (getValueAt(north) == '-' && have_key))
					&& !closed.contains(north)) {
				open.add(north);
			}

			if (south != null && (!obstacles.contains(getValueAt(south)) || (getValueAt(south) == '-' && have_key))
					&& !closed.contains(south)) {
				open.add(south);
			}

			if (east != null && (!obstacles.contains(getValueAt(east)) || (getValueAt(east) == '-' && have_key))
					&& !closed.contains(east)) {
				open.add(east);
			}

			if (west != null && (!obstacles.contains(getValueAt(west)) || (getValueAt(west) == '-' && have_key))
					&& !closed.contains(west)) {
				open.add(west);
			}

			open.remove(current);
			closed.add(current);
		}

		return false;
	}

	// Find the nearest reachable Position to the goal. If there is a tie,
	// returns the nearest of these positions to the agent.
	public Position findNearestReachable(Position goal) {
		List<Position> candidates = new ArrayList<Position>();
		int dist = Integer.MAX_VALUE;
		Position ret = null;
		for (int i = 0; i < world.numRows(); i++) {
			for (int j = 0; j < world.numCols(); j++) {
				Position temp = world.getCell(i, j);
				int tempDist = manDistTo(temp, goal, 9);

				if (tempDist <= dist && checkReachable(temp, worldPosition)) {

					if (tempDist == dist) {
						candidates.add(temp);
					} else {
						candidates = new ArrayList<Position>();
						candidates.add(temp);
						dist = tempDist;
					}

				}
			}
		}

		dist = Integer.MAX_VALUE;
		for (Position p : candidates) {
			int tempDist = manDistTo(p, worldPosition, 9);
			if (tempDist < dist) {
				dist = tempDist;
				ret = p;

			}
		}
		return ret;

	}

	public Character move(char[][] view) {
		Character move = null;

		if (have_treasure) {
			world.getCell(worldPosition.getRow() - positionRelativeToStart[0],
					worldPosition.getCol() - positionRelativeToStart[1]).setValue('S');
			symbolValues.put('S', 2);
		}

		Position forwardPosition = world.getCell(worldPosition.getRow() + forward.get(orientation)[0],
				worldPosition.getCol() + forward.get(orientation)[1]);
		char forwardSymbol = forwardPosition.getValue();
		if (!nextMoves.isEmpty() && nextMoves.get(nextMoves.size() - 1) == 'F') {

			// If set of moves has brought the agent to an obstacle, then find a
			// new move set.
			if (obstacles.contains(forwardSymbol)
					&& !((on_water && forwardSymbol == '~') || (forwardSymbol == '~' && have_raft))) {
				nextMoves = null;
			}

			// If set of moves has led the agent to change between land and
			// water, then reassess to objective to
			// avoid unnecessary switching.
			if (!(embarkingOrDisembarking && forwardPosition.equals(objective)) && failedToFindPathToObjective
					&& ((getValueAt(worldPosition) == '~' && forwardSymbol != '~')
							|| (getValueAt(worldPosition) != '~' && forwardSymbol == '~'))) {
				nextMoves = null;
			}
		}

		// Check whether new useful items for uncovered as a result of the last
		// move.
		newItems = checkForNewUsefulItems();

		// If there are no pending moves, find a new objective and move set.
		if (nextMoves == null || nextMoves.isEmpty() || newItems) {
			embarkingOrDisembarking = false;

			if (!failedToFindPathToObjective || (failedToFindPathToObjective && newItems)) {
				nextMoves = null;
				objective = null;
				objectives = findObjective();
				for (Position o : objectives) {

					nextMoves = astar(worldPosition, o, orientation, false, false, 1.2f, 5d, false);
					if (!(nextMoves == null) && !nextMoves.isEmpty()) {
						nextMoves.remove(nextMoves.size() - 1);
						objective = o;
						failedToFindPathToObjective = false;
						break;
					}
				}
			}

			// If no path to an objective is found, then do exploration
			if (nextMoves == null || (nextMoves.isEmpty() && move == null)) {

				// If an objective was found, but no path was found, set this to
				// true.
				if (!objectives.isEmpty()) {
					failedToFindPathToObjective = true;
				}

				// Find reachable positions with no use of finite items
				List<Position> reachables = findReachableTiles(worldPosition, 0);

				// If none found, find reachable positions with tree chopping
				// allowed
				if (reachables.isEmpty() && have_axe) {
					reachables = findReachableTiles(worldPosition, 1);
				}

				// If none found, find nearest water tile with positive
				// exploreScore
				if (reachables.isEmpty() && have_axe) {
					reachables = findReachableTiles(worldPosition, 2);
					if (!reachables.isEmpty()) {
						embarkingOrDisembarking = true;
					}
				}
				// embarkingOrDisembarking = false;

				// If no exploration candidates are found, then find paths to
				// possibly useful items
				if (reachables.isEmpty()) {
					failedToFindPathToObjective = false;
					symbolValues.put('d', 1);

					if (!have_key) {
						symbolValues.put('k', 1);
					}
					if (!have_axe) {
						symbolValues.put('a', 1);
					}

					objectives = findObjective();

					for (Position o : objectives) {
						objective = o;
						Position nearestReachable = findNearestReachable(objective);

						nextMoves = astar(worldPosition, objective, orientation, false, false, 1.2f, 10, false);

						// If A* is not able to find a path to the objective,
						// find a path to the nearest reachable point to the
						// objective,
						// then find a path from there to the objective.
						if (nextMoves == null) {
							List<Character> positionToNearest = astar(worldPosition, nearestReachable, orientation,
									true, false, 1.2f, 2, false);
							int startOrientation = (int) positionToNearest.remove(positionToNearest.size() - 1);
							nextMoves = astar(nearestReachable, objective, startOrientation, false, false, 1.2f, 15,
									false);

							if (nextMoves == null) {
								continue;
							}
							nextMoves.remove(nextMoves.size() - 1);

							if (nextMoves.get(0) != null) {
								nextMoves.addAll(positionToNearest);
							} else {
								nextMoves = positionToNearest;
							}
						}

						if (!(nextMoves == null) && !nextMoves.isEmpty()) {
							break;
						}
					}

				} else {
					objective = reachables.get(reachables.size() - 1);

					nextMoves = astar(worldPosition, objective, orientation, true, false, 1.5f, 2, false);
					nextMoves.remove(nextMoves.size() - 1);
				}

				// Last resort exploration - try to find a new path for
				// exploration using dynamite.
				if (objective == null) {
					reachables = findReachableTiles(worldPosition, 3);
					objective = reachables.get(reachables.size() - 1);
					nextMoves = astar(worldPosition, objective, orientation, false, false, 1.5f, 2, false);
					nextMoves.remove(nextMoves.size() - 1);

				}

			}

		}

		move = nextMoves.remove(nextMoves.size() - 1);

		updateOrientation(move);
		lastMoveSuccessful = successfulMove(move, view);

		return move;
	}

	// Returns a value for how agents map and inventory should be updated after
	// a successful move.
	public int successfulMove(char move, char view[][]) {
		/*
		 * 0 - failed move or turn 1 - normal move forward 2 - open, blowup or
		 * chop 3 - move forward over key, dynamite or axe 4 - got the treasure,
		 * let's go home
		 */

		if (view[1][2] != '*' && view[1][2] != '-' && view[1][2] != 'T' && (move == 'f' || move == 'F')) {
			if (view[1][2] == 'a') {
				have_axe = true;
				numberOfAxes--;
				symbolValues.put('a', 0);

				if (on_water == true) {
					on_water = false;
					have_raft = false;
					obstacles.add('~');
				}

				return 3;
			} else if (view[1][2] == 'k') {
				numberOfKeys--;
				have_key = true;
				symbolValues.put('k', 0);
				if (on_water == true) {
					on_water = false;
					have_raft = false;
					obstacles.add('~');
				}

				return 3;
			} else if (view[1][2] == '$') {
				numberOfTreasures--;
				symbolValues.put('$', 0);
				have_treasure = true;

				if (on_water == true) {
					on_water = false;
					have_raft = false;
					obstacles.add('~');
				}
				return 3;
			} else if (view[1][2] == 'd') {

				numberOfDynamite--;

				if (on_water == true) {
					on_water = false;
					have_raft = false;
					obstacles.add('~');
				}

				num_dynamites_held++;
				return 3;
			} else if (view[1][2] == ' ' && on_water == true) {
				on_water = false;
				have_raft = false;
				obstacles.add('~');
				return 3;
			} else if (view[1][2] == '~') {
				obstacles.remove('~');
				on_water = true;
				return 1;
			}
			return 1;
		} else if (view[1][2] == '-' && have_key && (move == 'u' || move == 'U')) {
			return 2;
		} else if (view[1][2] == 'T' && have_axe && (move == 'c' || move == 'C')) {
			numberOfTrees--;
			symbolValues.put('~', 0);
			have_raft = true;
			symbolValues.put('T', 0);
			return 2;
		} else if ((view[1][2] == '*' || view[1][2] == 'T' || view[1][2] == '-') && num_dynamites_held > 0
				&& (move == 'b' || move == 'B')) {
			num_dynamites_held--;

			return 2;
		}

		return 0;
	}

	// Create first view of world
	public void initialiseWorld(char view[][]) {
		for (int i = 0; i < 5; i++) {
			for (int j = 0; j < 5; j++) {
				Position p = new Position(i, j);
				p.setValue(view[i][j]);
				world.addPosition(i, j, p);

			}
		}

	}

	public boolean needExpansion() {
		if (positionRelativeToStart[0] < 0 && Math.abs(positionRelativeToStart[0]) > expandedUpRightDownLeft[0]) {
			return true;
		} else if (positionRelativeToStart[0] > 0
				&& Math.abs(positionRelativeToStart[0]) > expandedUpRightDownLeft[2]) {
			return true;
		} else if (positionRelativeToStart[1] > 0
				&& Math.abs(positionRelativeToStart[1]) > expandedUpRightDownLeft[1]) {
			return true;
		} else if (positionRelativeToStart[1] < 0
				&& Math.abs(positionRelativeToStart[1]) > expandedUpRightDownLeft[3]) {
			return true;
		}

		return false;
	}

	public void updateFOV(char view[][]) {
		int count = 0;
		if (orientation == 0) {
			List<Position> FOV = world.getRow(worldPosition.getRow() + forward.get(orientation)[0] * 2);

			for (int i = 0; i < FOV.size(); i++) {
				if (worldPosition.getCol() - 2 <= i && i <= worldPosition.getCol() + 2 && count < 5) {
					FOV.get(i).setValue(view[0][count]);
					count++;
				}

			}
		} else if (orientation == 2) {
			List<Position> FOV = world.getRow(worldPosition.getRow() + forward.get(orientation)[0] * 2);

			for (int i = FOV.size() - 1; i >= 0; i--) {
				if (worldPosition.getCol() - 2 <= i && i <= worldPosition.getCol() + 2 && count < 5) {
					FOV.get(i).setValue(view[0][count]);
					count++;
				}

			}

		} else if (orientation == 1) {
			for (int i = 0; i < world.numRows(); i++) {
				if (worldPosition.getRow() - 2 <= i && i <= worldPosition.getRow() + 2 && count < 5) {
					world.getCell(i, worldPosition.getCol() + forward.get(orientation)[1] * 2).setValue(view[0][count]);
					count++;
				}
			}
		} else if (orientation == 3) {
			for (int i = world.numRows() - 1; i >= 0; i--) {
				if (worldPosition.getRow() - 2 <= i && i <= worldPosition.getRow() + 2 && count < 5) {
					world.getCell(i, worldPosition.getCol() + forward.get(orientation)[1] * 2).setValue(view[0][count]);
					count++;
				}
			}
		}
	}

	// Updates the agents WorldState - the memory of the world explored so far.
	public void updateWorld(char view[][], int successfulMove) {
		if (successfulMove != 0) {
			if (successfulMove == 2) {
				Integer[] position = forward.get(orientation);
				world.getCell(worldPosition.getRow() + position[0], worldPosition.getCol() + position[1]).setValue(' ');
			} else {
				int count = 0;
				updateWorldPosition();

				if (needExpansion()) {
					expandedUpRightDownLeft[orientation]++;

					if (orientation == 0) {
						List<Position> newRow = new ArrayList<Position>();
						for (int i = 0; i < world.numCols(); i++) {
							newRow.add(new Position('#'));
						}

						for (int i = 0; i < newRow.size(); i++) {
							if (worldPosition.getCol() - 2 <= i && i <= worldPosition.getCol() + 2 && count < 5) {
								newRow.get(i).setValue(view[0][count]);
								count++;
							}
						}

						world.addRow('T', newRow);
					} else if (orientation == 2) {
						List<Position> newRow = new ArrayList<Position>();
						for (int i = 0; i < world.numCols(); i++) {
							newRow.add(new Position('#'));
						}

						for (int i = newRow.size() - 1; i >= 0; i--) {
							if (worldPosition.getCol() - 2 <= i && i <= worldPosition.getCol() + 2 && count < 5) {
								newRow.get(i).setValue(view[0][count]);
								count++;
							}
						}
						world.addRow('B', newRow);
					} else if (orientation == 1) {
						for (int i = 0; i < world.numRows(); i++) {
							Position p = new Position('#');
							if (worldPosition.getRow() - 2 <= i && i <= worldPosition.getRow() + 2 && count < 5) {
								p.setValue(view[0][count]);

								count++;
							}

							world.addPosition(i, p);

						}
					} else if (orientation == 3) {
						for (int i = world.numRows() - 1; i >= 0; i--) {
							Position p = new Position('#');
							if (worldPosition.getRow() - 2 <= i && i <= worldPosition.getRow() + 2 && count < 5) {
								p.setValue(view[0][count]);
								count++;
							}

							world.addPosition(i, 0, p);
						}

					}
					count = 0;

				}

				for (int i = 0; i < world.numRows(); i++) {
					for (int j = 0; j < world.numCols(); j++) {
						world.getCell(i, j).setRow(i);
						world.getCell(i, j).setCol(j);
					}
				}

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

		if (!((worldPosition.getRow() <= 2 && orientation == 0) || (worldPosition.getCol() <= 2 && orientation == 3))) {

			worldPosition.setRow(worldPosition.getRow() + forward.get(orientation)[0]);
			worldPosition.setCol(worldPosition.getCol() + forward.get(orientation)[1]);

		}

	}

	public void updateOrientation(char turn) {
		if (turn == 'L' || turn == 'l') {
			orientation = (orientation - 1) % 4;
			if (orientation < 0) {
				orientation += 4;

			}
		} else if (turn == 'R' || turn == 'r') {
			orientation = (orientation + 1) % 4;
		}
	}

	public int checkOrientation(char turn, int start) {
		if (turn == 'L' || turn == 'l') {
			start = (start - 1) % 4;
			if (start < 0) {
				return start += 4;

			}
			return start;
		} else if (turn == 'R' || turn == 'r') {
			return (start + 1) % 4;
		}
		return start;
	}

	public char get_action(char view[][]) {
		int ch = 0;

		// System.out.print("Enter Action(s): ");

		while (ch != -1) {
			ch = move(view);

			if (lastMoveSuccessful != 0 || ch == 'L' || ch == 'l' || ch == 'R' || ch == 'r') {
				return ((char) ch);
			}
		}
		return 0;
	}

	void print_view(char view[][]) {
		int i, j;

		System.out.println("\n+-----+");
		for (i = 0; i < 5; i++) {
			System.out.print("|");
			for (j = 0; j < 5; j++) {
				if ((i == 2) && (j == 2)) {
					System.out.print('^');
				} else {
					System.out.print(view[i][j]);
				}
			}
			System.out.println("|");
		}
		System.out.println("+-----+");

	}

	public static void main(String[] args) {

		InputStream in = null;
		OutputStream out = null;
		Socket socket = null;
		Agent agent = new Agent();
		char view[][] = new char[5][5];
		char action = 'F';
		int port;
		int ch;
		int i, j;

		if (args.length < 2) {
			System.out.println("Usage: java Agent -p <port>\n");
			System.exit(-1);
		}

		port = Integer.parseInt(args[1]);

		try { // open socket to Game Engine
			socket = new Socket("localhost", port);
			in = socket.getInputStream();
			out = socket.getOutputStream();
		} catch (IOException e) {
			System.out.println("Could not bind to port: " + port);
			System.exit(-1);
		}

		try { // scan 5-by-5 window around current location
			while (true) {
				for (i = 0; i < 5; i++) {
					for (j = 0; j < 5; j++) {
						if (!((i == 2) && (j == 2))) {
							ch = in.read();
							if (ch == -1) {
								System.exit(-1);
							}
							view[i][j] = (char) ch;
						}
					}
				}
				// agent.print_view( view ); // COMMENT THIS OUT BEFORE
				// SUBMISSION

				if (agent.world.getRow(0).isEmpty()) {
					agent.initialiseWorld(view);
				} else {
					agent.updateWorld(view, agent.lastMoveSuccessful);
				}

				agent.world.print();
				action = agent.get_action(view);
				out.write(action);

			}

		} catch (IOException e) {
			System.out.println("Lost connection to port: " + port);
			System.exit(-1);
		} finally {
			try {
				socket.close();
			} catch (IOException e) {
			}
		}
	}
}
