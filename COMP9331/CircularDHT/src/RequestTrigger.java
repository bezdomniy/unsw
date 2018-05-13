
public class RequestTrigger {
	public static void updatePredecessors(DHTPeer peer, String request) {
		Integer receivedInteger = Integer.parseInt(request);
		Integer[] predecessorPorts = peer.getPredecessorPorts();

		if (predecessorPorts[0] == null) {
			predecessorPorts[0] = receivedInteger;
		} else if (!predecessorPorts[0].equals(receivedInteger)) {
			predecessorPorts[1] = receivedInteger;
		}

		// Set the correct order for predecessor peers - probably don't need to yet, but leave it until confirmed.
//		if (predecessorPorts[0] != null && predecessorPorts[1] != null) {
//			if ((predecessorPorts[0] < peer.getPeerIdentity() && predecessorPorts[1] > peer.getPeerIdentity())
//					|| (predecessorPorts[1] < peer.getPeerIdentity() && predecessorPorts[0] > peer.getPeerIdentity())) {
//				if (predecessorPorts[0] > predecessorPorts[1]) {
//					int temp = predecessorPorts[0];
//					predecessorPorts[0] = predecessorPorts[1];
//					predecessorPorts[1] = temp;
//				}
//			} else {
//				if (predecessorPorts[1] > predecessorPorts[0]) {
//					int temp = predecessorPorts[0];
//					predecessorPorts[0] = predecessorPorts[1];
//					predecessorPorts[1] = temp;
//				}
//			}
//		}
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

		// peer.getClient().initialisePingSender(peer);

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
