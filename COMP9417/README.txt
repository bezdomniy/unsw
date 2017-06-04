READ ME: GeneticAlgorythm.py 
Developed by Ilia Chibaev, z3218424

Requirements:
1. Python 3
2. Packages required: numpy, scipy.io.arff, scipy.stats, random, math, copy (all in Anaconda)
3. balance-scale.arff and mushroom.arff must be in the same folder as GeneticAlgorythm.py 

Initialise:
python GeneticAlgorythm.py dataUsed [chooseSelection crossoverMethod populationSize replacementRate mutationRate]

Parameters:
	Required argument:
	
	dataUsed: 
		B: Balance-scale - bitString hypothesis
		BT: Balance-scale - binary tree hypothesis
		M: Mushroom
	
	
	Optional arguements:
	
	proportionTrain: only used for mushroom, equals 1 for 'B' and 'BT'
		greater than 0 and less than or equal to 1

	chooseSelection:
		F: Fitness proportionate
		R: Rank
		T: Tournament
		
	crossoverMethod (not dataUsed = 'BT' which uses subtree crossover):
		O: One-point crossover
		T: Two-point crossover
		U: Uniform crossover

	populationSize:
		greater than 0
		
	replacementRate: rate of crossover
		between 0 and 1
		
	mutationRate:
		between 0 and 1
		
	similarityAllowed:
		greater than 0, less than or equal to hypothesis length
		
	probabilityOfFitterWinning:
		between 0 and 1
		
	probabilityOfOneGene:
		between 0 and 1 (0.5 is uniform probability)
		
	genesPerMutation:
		greater than 0, less than or equal to hypothesis length
		
	
		
	