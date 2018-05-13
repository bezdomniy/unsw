
public class RequestTrigger {
	public static void updatePredecessors(DHTPeer peer, String request) {
		Integer receivedInteger = Integer.parseInt(request);
		Integer[] predecessorPorts = peer.getPredecessorPorts();

		if (predecessorPorts[0] == null) {
			predecessorPorts[0] = receivedInteger;
		} else if (!predecessorPorts[0].equals(receivedInteger)) {
			predecessorPorts[1] = receivedInteger;
		}

		// System.out.println("First "+predecessorPorts[0]+", "+"Second
		// "+predecessorPorts[1]);

	}

	public static void updateSuccessor(DHTPeer peer, Integer quitterPort, Integer requesterFirstNeighbourPort,
			Integer requesterSecondNeighbourPort) {
		if (quitterPort.equals(peer.getFirstSuccessorPort())) {
			peer.setFirstSuccessorPort(peer.getSecondSuccessorPort());
			peer.setSecondSuccessorPort(requesterSecondNeighbourPort);
		} else {
			peer.setSecondSuccessorPort(requesterFirstNeighbourPort);
		}
		System.out.println("My first successor is now " + peer.getFirstSuccessorPort() + ".");
		System.out.println("My second successor is now " + peer.getSecondSuccessorPort() + ".");

		//peer.getClient().initialisePingSender(peer);

		// System.out.println("First "+predecessorPorts[0]+", "+"Second
		// "+predecessorPorts[1]);

	}

	public static void updateDroppedSuccessor(DHTPeer peer, Integer droppedNeighbourPort,
			Integer replacementNeighbour) {
		if (droppedNeighbourPort.equals(peer.getFirstSuccessorPort())) {
			peer.setFirstSuccessorPort(peer.getSecondSuccessorPort());
		}
		peer.setSecondSuccessorPort(replacementNeighbour);
	}
}
