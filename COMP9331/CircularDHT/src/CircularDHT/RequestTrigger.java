package CircularDHT;

public class RequestTrigger {
	private DHTPeer peer ;
	public RequestTrigger(DHTPeer peer) {
		this.peer = peer;
	}
	
	public void updatePredecessors(String request) {
		Integer receivedInteger = Integer.parseInt(request);
		Integer[] predecessorPorts = this.peer.getPredecessorPorts();
		
		if (predecessorPorts[0] == null) {
			predecessorPorts[0] = receivedInteger;
		} else if (!predecessorPorts[0].equals(receivedInteger)) {
			predecessorPorts[1] = receivedInteger;
		}
		
		//System.out.println("First "+predecessorPorts[0]+", "+"Second "+predecessorPorts[1]);

	}
	
	public void updateSuccessor(Integer quitterPort, Integer quitterFirstNeighbourPort) {
		if (quitterPort.equals(this.peer.getFirstSuccessorPort())) {
			this.peer.setFirstSuccessorPort(this.peer.getSecondSuccessorPort());
		}
		else if (quitterPort.equals(this.peer.getSecondSuccessorPort())) {
			this.peer.setSecondSuccessorPort(quitterFirstNeighbourPort);
		}
		
		
		this.peer.getClient().initialisePingSender(this.peer.getFirstSuccessorPort(), this.peer.getSecondSuccessorPort(), this.peer.getPingResult());
		
		//System.out.println("First "+predecessorPorts[0]+", "+"Second "+predecessorPorts[1]);

	}
}
