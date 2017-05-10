import java.time.LocalDate;
import java.util.Map;
import java.util.Random;
 
import java.util.TreeMap;
 
public class test {
    public static void main(String...a) {
	final Map<LocalDate, Integer> foobar = new TreeMap<>();
 
	// Fill a date -> int map with 12 random ints between 0 and 100, 
	new Random(System.currentTimeMillis()).ints(0,100).limit(12).forEach(value -> 
		foobar.put(
			LocalDate.now().withMonth(foobar.size() + 1), 
			value
		));
	// print them for verbosity
	foobar.entrySet().forEach(System.out::println);
	// get the maximum
	Map.Entry<LocalDate, Integer> max 
		= foobar
		    // from all entries
		    .entrySet()
		    // stream them
		    .stream()
		    // max, obviously
		    .max(
			    // this one is cool. It generates
			    // Map.Entry comparators by delegating to another
			    // comparator, exists also for keys
			    Map.Entry.comparingByValue(Integer::compareTo)
		    )
		    // Get the optional (optional because the map can be empty)
		    .get();
	System.out.println("Max is " + max);
    }
}