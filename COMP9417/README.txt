READ ME: GeneticAlgorithm.py 
Developed by Ilia Chibaev, z3218424

Requirements:
1. Python 3
2. Packages required: numpy, scipy.io.arff, scipy.stats, random, math, copy (all in Anaconda)
3. balance-scale.arff and mushroom.arff must be in the same folder as GeneticAlgorythm.py 

Initialise:
python GeneticAlgorithm.py dataUsed [chooseSelection crossoverMethod populationSize replacementRate mutationRate]

Parameters:
	Required argument:
	
	1. dataUsed: 
		B: Balance-scale - bitString hypothesis
		BT: Balance-scale - binary tree hypothesis
		M: Mushroom
	
	
	Optional arguements:
	
	2. proportionTrain (only used for mushroom, equals 1 for 'B' and 'BT'):
		Range: greater than 0 and less than or equal to 1

	3. chooseSelection:
		F: Fitness proportionate
		R: Rank
		T: Tournament
		
	4. crossoverMethod (not dataUsed = 'BT' which uses subtree crossover):
		O: One-point crossover
		T: Two-point crossover
		U: Uniform crossover

	5. populationSize:
		Range: greater than 0
		
	6. replacementRate: 
		Definition: rate of crossover
		Range: between 0 and 1
		
	7. mutationRate:
		Range: between 0 and 1
		
	8. similarityAllowed: 
		Definition: parameter for the minimum gene difference required for crossover
		greater than 0, less than or equal to hypothesis length
		
	9. probabilityOfFitterWinning: 
		Definition: probability of individual with higher fitness being chosen in tournament selection
		Range: between 0 and 1
		
	10. probabilityOfOneGene:
		Definition: Values further from 0.5 generate a more general hypothesis when initialising the population
		Range: between 0 and 1 (0.5 is uniform probability)
		
	11. genesPerMutation:
		Definition: Number of genes changed in a mutation
		Range: greater than 0, less than or equal to hypothesis length
		
	
		
	