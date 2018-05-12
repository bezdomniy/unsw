

public class PingResult {
	private boolean isActive;
	private Integer firstNeighbour;
	private Integer secondNeighbour;
	
	public PingResult(boolean isActive, Integer firstNeighbour, Integer secondNeighbour) {
		this.isActive = isActive;
		this.firstNeighbour = firstNeighbour;
		this.secondNeighbour = secondNeighbour;
	}

	public boolean isActive() {
		return isActive;
	}

	public Integer getFirstNeighbour() {
		return firstNeighbour;
	}

	public Integer getSecondNeighbour() {
		return secondNeighbour;
	}
	

}
