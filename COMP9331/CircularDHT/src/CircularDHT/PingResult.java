package CircularDHT;

public class PingResult {
	private boolean firstActive;
	private boolean secondActive;
	
	public boolean isFirstActive() {
		return firstActive;
	}

	public void setFirstActive(boolean firstActive) {
		this.firstActive = firstActive;
	}

	public boolean isSecondActive() {
		return secondActive;
	}

	public void setSecondActive(boolean secondActive) {
		this.secondActive = secondActive;
	}

	
	public PingResult(boolean firstNeighbourActive, boolean secondNeighbourActive) {
		this.firstActive = firstNeighbourActive;
		this.secondActive = secondNeighbourActive;
	}
	
	@Override
	public String toString() {
		if (this.firstActive && this.secondActive) {
			return "Both active";
		}
		else if (this.firstActive) {
			return "Second dropped";
		}
		else if (this.secondActive) {
			return "First dropped";
		}
		else {
			return "Both dropped";
		}
	}
}
