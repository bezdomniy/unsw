import java.util.*;
// import java.util.List;
// import java.util.LinkedList;
// import java.util.Collections;

public class BucketSort {
	public BucketSort() {
		
	}
	
	public static Integer[] bSort(List<Integer> input) {
		Integer index = null;
		int arrayLength = input.size();
		int maxElement = Collections.max(input);
		
		LinkedList<Integer>[] buckets = new LinkedList[arrayLength+1];
		Integer[] output = new Integer[arrayLength];
		
		for (Integer i:input) {
			index = i*arrayLength/maxElement;
			
			if (buckets[index] == null) {
				buckets[index] = new LinkedList();
				buckets[index].add(i);
			}
			else {
				int count = 0;
				for (Iterator<Integer> iter = buckets[index].iterator(); iter.hasNext(); ) {
					Integer key = iter.next();
					if (i<=key) {
						buckets[index].add(count,i);
						count=0;
						break;
					}
					// else if (!iter.hasNext()) {
						// buckets[index].addLast(i);
						// count=0;
						// break;
					// }
					count++;
				
				}
			}
		}
		
		for (LinkedList t:buckets){
			System.out.println(t);
		}
		
		int count2=0;
		Integer out = null;
		for (LinkedList x:buckets) {
			if (x != null) {
				for (Iterator<Integer> y = x.iterator(); y.hasNext(); ) {
					output[count2]= y.next();
					count2++;
				}
			}
		}
		return output;
	}
	
	
	public static void main(String[] args) {
		List<Integer> test = new ArrayList<Integer>();
		test.add(3);
		test.add(2);
		test.add(3);
		test.add(1);
		test.add(3);
		test.add(2);
		test.add(4);
		
		List<Integer> o = new Integer[test.size()];
		o=BucketSort.bSort(test);
		
		for (Integer p:o) {
			System.out.println(p);
		}
	}
}