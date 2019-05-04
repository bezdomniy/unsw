// COMP6452 Assignment 1
// Ilia Chibaev, z3218424
// 
// I would improve this contract by allowing the participants to 
// put their money where their mouth is. I would allow the friends to
// pledge ether along-side their vote and the votes would be weighted
// by the amount of ether pledged. This total ether pledged would go
// into a pool which, after the contract is killed, would be split
// evenly amongst the friends, effectively reducing each friend's contribution
// to the lunch bill by an equal amount.
// 
// This plan would be (economically) more efficient because the person
// that desires a particular lunch spot most would get what they want,
// while the others would get compensated for missing out on their choices
// with a reduction in their lunch bill.
// 
// 
// pseudocode for additional structs and functions:
//   
//   totalEtherReceived = 0
//  
//   function vote(choiceName, etherAmount):
//      <check conditions for valid vote>
//      increment choices.choiceName by etherAmount
//      set friend voted = true
//      increment total votes
// 
//   function distrubute():
//      check condition that voting has ended and result has been seen
//      for each friend address in friends map:
//          send to friend totalEtherReceived / number of friends
// 

// Note: Inputs and returns of choices names must first encoded/decoded into
//  hexidecimal ascii represention e.g.: "ABC" = 0x414243 .
pragma solidity ^0.4.18;
contract GetLunch {
    address public creator;
    uint quorum;
    uint totalVotes;

    // Constructor function adding creator as a potential voter
    function GetLunch() public {
        creator = msg.sender;
        friends[creator] = Friend({voted:false, isValue:true});
        totalVotes = 0;
        
        friends[address(1)] = Friend({voted:false, isValue:true});
    }
    
    // Structs to define a friend and lunch choice
    // isValue boolean used to implement dict.contains functionality
    struct Choice {
        bytes32 name;
        uint votes;
        bool isValue;
    }
    
    struct Friend {
        bool voted;
        bool isValue;
    }
    
    // Mapping variables to allow efficient lookup
    mapping(address => Friend) public friends;
    mapping(bytes32 => Choice) public choices;
    
    // List of keys maintained for output and to iterate over
    bytes32[] choiceKeys;

    // Creator is able to add a choice if it does not already exist
    // Note: choiceName must be of type byte32
    function addChoice(bytes32 choiceName) public {
        require(msg.sender == creator && choices[choiceName].isValue == false);
        choices[choiceName] = Choice({name:choiceName, votes:0, isValue:true});
        choiceKeys.push(choiceName);
    }
    
    // Creator is able to set the quorum if they have not already
    // Note: once set, quorum cannot be revised
    function setQuorum(uint q) public {
        require(msg.sender == creator && quorum == 0);
        quorum = q;
    }
    
    // Creator is able to select friends to vote
    function selectFriend(address friendAddress) public {
        require(msg.sender == creator && friends[friendAddress].isValue == false);
        friends[friendAddress] = Friend({voted:false, isValue:true});
    }

    // Creator or friend is able to vote for a choice
    // Note 1: each friend is able to vote only once and only for a valid choiceName
    // no votes are accepted after the quorum is met
    // Notes 2: choiceName must be of type byte32
    function vote(bytes32 choiceName) public {
        require(friends[msg.sender].isValue != false && totalVotes < quorum && 
            friends[msg.sender].voted != true && choices[choiceName].isValue == true);
        
        choices[choiceName].votes++;
        friends[msg.sender].voted = true;
        totalVotes++;
    }
    
    // If the quorum has been set and met, this can return a getResult
    // Note 1: this will only return a single result, so if there is a tie
    // then the choice added first will win
    // Note 2: returns type byte32 winner name
    function getResult() public view returns(bytes32 winner) {
        require(totalVotes == quorum && quorum > 0);
        
        Choice memory choice;
        
        for (uint i = 0; i < choiceKeys.length; i++) {
            if (choices[choiceKeys[i]].votes > choice.votes) {
                choice.votes = choices[choiceKeys[i]].votes;
                choice.name = choices[choiceKeys[i]].name;
            }
        }
        
        return choice.name;
    }
    
    // Return a list of choices
    // Note: returns an array of elements of type byte32
    function getChoices() public view returns(bytes32[] allChoices) {
        return choiceKeys;
    }
    
    // The creator can destroy the contract after the quorum is met,
    // or before the quorum has been set and any votes have been cast
    function kill() public {
        require(msg.sender == creator && totalVotes == quorum);
        selfdestruct(creator);
    }
}