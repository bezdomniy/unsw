import net.datastructures.*;
import java.io.*;
import java.util.*;

public class TaskScheduler<K,V> {
	/*
	A task scheduling class used to optimally allocate tasks to processing cores in O(nlogn) time.
	*/
	
	public TaskScheduler() {}
	
	/*
	Task class used to store attributes of each discrete task.
	*/
	protected static class Task {
		private String task;
		private Integer releaseTime;
		private Integer deadline;
	
		public Task() {
			reset();
		}

		public String getTask() {
			return this.task;
		}
		
		public Integer getReleaseTime() {
			return this.releaseTime;
		}
		
		public Integer getDeadline() {
			return this.deadline;
		}
		
		public void setTask(String task) {
			this.task = task;
		}
		
		public void setReleaseTime(Integer releaseTime) {
			this.releaseTime = releaseTime;
		}
		
		public void setDeadline(Integer deadline) {
			this.deadline = deadline;
		}
		
		public void reset() {
			this.deadline = null;
			this.releaseTime = null;
			this.task = null;
		}
	}
	
	
	/*
	A task scheduling algorithm which runs in O(nlogn) time using heap based priority queues to first sort by
	releaseTime of each task, then at each increment of time sort the released tasks by deadline
	and assign then to one of m cores in each time period.
	*/
	static <T> void scheduler(String file1, String file2, int m) throws FileNotFoundException,NumberFormatException {

		Scanner input = null;
		try {
			input = new Scanner(new File(file1)).useDelimiter("[\\s\\r\\n]+");
		}
		catch (FileNotFoundException e) {
			 System.err.println(file1+" does not exist");
			 return;
		}
		
		
		/*
		This loop reads each token in the input file and creates a Task object storing its attributes
		and inserts the object into the pq heap with releaseTime as the index O(nlogn)
		*/
		
		int count=0;
		Integer minReleaseTime = null;
		Integer release = null;
		Integer deadline = null;
		String taskName = null;
		Task t = null;
		HeapPriorityQueue<Integer, Task> pq = new HeapPriorityQueue<Integer, Task>();
		
		while (input.hasNext()) {	
			t = new Task();
			
			if (count % 3 == 0) {
				taskName=input.next();
			}
			else if (count % 3 == 1) {
				try {
					release=Integer.parseInt(input.next());
				} catch (NumberFormatException e) {
					System.err.println("input error when reading the atrributes of the task "+taskName);
				}
				if (minReleaseTime == null || release < minReleaseTime) {
					minReleaseTime = release;
				}
			}
			else if (count % 3 == 2) {
				try {
					deadline=Integer.parseInt(input.next());
				} catch (NumberFormatException e) {
					System.err.println("input error when reading the atrributes of the task "+taskName);
				}
			}
			
			if (deadline != null && taskName != null) {
				t.setDeadline(deadline);
				t.setTask(taskName);
				t.setReleaseTime(release);
				
				pq.insert(t.getReleaseTime(), t);
				
				deadline = null;
				taskName = null;
			}
			count++;
		}
		input.close();
		
		
		/*
		This loop takes each element from pq heap then inserts it into the temppq heap with the
		task deadline as key: O(nlogn)
		
		When the next Entry in pq increases in releaseTime, Entries are removed from temppq
		and added to the out List: O(nlogk) where k is <= n and corresponds to the largest size of temppq 
		before the entire backlog of tasks at a given time is complete
		*/
		
		int coresUsed = 0;
		Entry<Integer, Task> out = null;
		Entry<Integer, Task> out2 = null;
		HeapPriorityQueue<Integer, Task> temppq = new HeapPriorityQueue<Integer, Task>();
		List<String> output = new ArrayList<String>();
		
		while (!pq.isEmpty()) {
			// Add new batch of tasks to the temppq with event increment of minReleaseTime
			if (pq.min().getValue().getReleaseTime() <= minReleaseTime) {
				out = pq.removeMin();
				temppq.insert(out.getValue().getDeadline(),out.getValue());
			}
			
			// If all tasks are exhausted or the next tasks is of a higher releaseTime then write from temppq to the output List
			if (pq.isEmpty() || pq.min().getValue().getReleaseTime() > out.getValue().getReleaseTime() ) {
				coresUsed = 0;
				while (!temppq.isEmpty()) {
					if (coresUsed < m && minReleaseTime < temppq.min().getValue().getDeadline()) {
						out2 = temppq.removeMin();
						output.add(out2.getValue().getTask());
						output.add(minReleaseTime.toString());
						coresUsed++;
					}
					// If all cores are used then go the next break the loop to move to next time step
					else if (coresUsed == m && minReleaseTime < temppq.min().getValue().getDeadline()) {
						break;
					}
					// If all cores are used and the deadline of the next task is equal or greater than the current time, 
					// then there is no feasible schedule.
					else {		
						System.err.println("No feasible schedule exists");
						return;
					}
				}
				
				// Go to next time step
				minReleaseTime++;
			}
		}

		
		/*
		This step iterates over the output List and writes the content to file2: O(n)
		*/
		
		PrintStream writeToFile = null;
		if (file2.endsWith(".txt")) {
			writeToFile = new PrintStream(new BufferedOutputStream(new FileOutputStream(file2)), true);
		}
		else {
			writeToFile = new PrintStream(new BufferedOutputStream(new FileOutputStream(file2+".txt")), true);
		}
		System.setOut(writeToFile);
		String sep = "";
		for (String s : output) {
			System.out.print(sep + s);
			sep = " ";
		}
		
	}
	
  public static void main(String[] args) throws Exception{

	    TaskScheduler.scheduler("samplefile1.txt", "feasibleschedule1", 4);
	   /** There is a feasible schedule on 4 cores */      
	    TaskScheduler.scheduler("samplefile1.txt", "feasibleschedule2", 3);
	   /** There is no feasible schedule on 3 cores */
	    TaskScheduler.scheduler("samplefile2.txt", "feasibleschedule3", 5);
	   /** There is a feasible scheduler on 5 cores */ 
	    TaskScheduler.scheduler("samplefile2.txt", "feasibleschedule4", 4);
	   /** There is no feasible schedule on 4 cores */

	   /** The sample task sets are sorted. You can shuffle the tasks and test your program again */  

	  }

}
