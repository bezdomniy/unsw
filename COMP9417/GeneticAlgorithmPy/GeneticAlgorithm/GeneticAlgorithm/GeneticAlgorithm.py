import pandas as pd
import numpy as np
import random
from scipy.io.arff import loadarff
import string
from copy import deepcopy


#random.seed(1)
proportionTrain = 1
hypothesisLength = 14
populationSize = 50
replacementRate = 0.2
mutationRate = 0.05


variableVal = {'00':'LW','01':'LD','10':'RW','11':'RD'}
operatorVal = {'00':'+','01':'-','10':'/','11':'*'}

variable = ('LW','RW','LD','RD')
operator = ('+','-','/','*')

exampleOrder = {'LW':0,'LD':1,'RW':2,'RD':3}


dataset, meta = loadarff(open('C:/dev/unsw/COMP9417/balance-scale.arff','r'))

dataset = dataset[meta.names()].tolist()
dataset = np.asarray(dataset, dtype='<U1')

random.shuffle(dataset)
train, test = dataset[:len(dataset) * proportionTrain], dataset[len(dataset) * proportionTrain:]

arributeNum = train.shape[1]
fitnessThreshold = 1

class Node():
    def __init__(self,value):
        self.left = None
        self.right = None
        self.value = value

class Individual():
    def __init__(self):
        self.genes = np.random.choice([0, 1], size=(hypothesisLength,))
        self.fitness = 0

    def setFitness(self,fitness):
        self.fitness = fitness

    def setGenes(self,genes):
        for i in range(0,hypothesisLength):
            self.genes[i] = genes[i]

    def __str__(self):
        funcArray = []

        for i in range(0,hypothesisLength,2):
            if i % 4 == 0:
                funcArray.append(variableVal[str(self.genes[i]) + str(self.genes[i + 1])])
            else:
                funcArray.append(operatorVal[str(self.genes[i]) + str(self.genes[i + 1])])
        return ' '.join(funcArray)

    def __repr__(self):
        return self.__str__()

class scaleIndividual():
    def __init__(self):
        self.root = Node(random.choice(operator))
        self.root.left = Node(random.choice(operator))
        self.root.right = Node(random.choice(operator))

        self.root.left.left = Node(random.choice(variable))
        self.root.left.right = Node(random.choice(variable))
        self.root.right.left = Node(random.choice(variable))
        self.root.right.right = Node(random.choice(variable))

        self.fitness = 0

    def setFitness(self,fitness):
        self.fitness = fitness

    def inorder(node):
        if node is not None:
            inorder(node.left)
            print(node.value,end=' ')
            inorder(node.right)

    def evaluate(self,example):
        return _evaluate(self)

    def _evaluate(node):
        if node is None:
            return 0
        if node.left is None and node.right is None:
            return int(node.value)

        leftEval = _evaluate(node.left)
        righEval = _evaluate(node.right)

        if node.value == '+':
            return leftEval + righEval
        elif node.value == '-':
            return leftEval - righEval
        elif node.value == '*':
            return leftEval * righEval
        else:
            return leftEval / righEval

    def __str__(self):
        scaleIndividual.inorder(self.root)
        return ''

    def __repr__(self):
        return self.__str__()

    

class Population():
    def __init__(self):
        self.individuals = []
        self.populationFitness = 0
        self.maxFitness = 0

    def createPopulation(self):
        self.individuals = [Individual() for _ in range(populationSize)]

    def addIndividual(self,individual):
        self.individuals.append(deepcopy(individual))

    def findFitness(self):
        for individual in self.individuals:
            fitness = fitnessScale(individual)
            self.populationFitness += fitness
            individual.setFitness(fitness)
            if fitness > self.maxFitness:
                self.maxFitness = fitness

        self.individuals.sort(key=lambda x: x.fitness ,reverse=True)

def fitnessScale(individual):
    fitness = 0
    genes = individual.genes

    for example in train:
        if testHypothesis(genes,example):
            fitness += 1


    #print(genes," fit: ",fitness)
    return (fitness/len(train)) ** 2
    #return fitness

def testHypothesis(hypothesis,example):
    funcArray = []

    for i in range(0,hypothesisLength,2):
        if i % 4 == 0:
            funcArray.append(variableVal[str(hypothesis[i]) + str(hypothesis[i + 1])])
        else:
            funcArray.append(operatorVal[str(hypothesis[i]) + str(hypothesis[i + 1])])

    func = [example[exampleOrder[x]] if x in exampleOrder is not None else x for x in funcArray]
    
    ev = eval(''.join(func))

    #print(func," balance: ",example[4]," = ",ev)

    if ev == 0:
        return example[4] == "B"
    if ev > 0:
        return example[4] == "R"
    if ev < 0:
        return example[4] == "L"

def crossover(individual1,individual2):
    return _onePointCrossover(individual1,individual2)

def _twoPointCrossover(individual1,individual2):
    n0 = random.randint(0, hypothesisLength)
    n1 = random.randint(0, hypothesisLength - n0)

    parents = [individual1,individual2]

    crossoverMask = [0] * n0 + [1] * n1 + [0] * (hypothesisLength - n0 - n1)

    offspring1 = Individual()
    offspring2 = Individual()
    for i in range(0,hypothesisLength):
        offspring1.genes[i] = parents[crossoverMask[i]].genes[i]
        offspring2.genes[i] = parents[crossoverMask[i] ^ 1].genes[i]

    return offspring1,offspring2

def _onePointCrossover(individual1,individual2):
    n0 = random.randint(0, hypothesisLength)

    parents = [individual1,individual2]

    crossoverMask = [0] * n0 + [1] * (hypothesisLength - n0)

    offspring1 = Individual()
    offspring2 = Individual()
    for i in range(0,hypothesisLength):
        offspring1.genes[i] = parents[crossoverMask[i]].genes[i]
        offspring2.genes[i] = parents[crossoverMask[i] ^ 1].genes[i]

    return offspring1,offspring2



def mutate(individual):
    gene = random.randint(0, hypothesisLength-1)
    
    if individual.genes[gene] == 0:
        individual.genes[gene] = 1
    else:
        individual.genes[gene] = 0


def runGA():
    pop = Population()
    pop.createPopulation()
    pop.findFitness()

    end = 20
    c=0

    while pop.maxFitness < fitnessThreshold :
        nextPop = Population()

        individualsForCrossover = np.random.choice(pop.individuals, replacementRate*populationSize, p=[x.fitness / pop.populationFitness for x in pop.individuals], replace=False)
        for i in range(0,len(individualsForCrossover),2):
            offspring1, offspring2 = crossover(individualsForCrossover[i],individualsForCrossover[i +1 ])
            nextPop.addIndividual(offspring1)
            nextPop.addIndividual(offspring2)

        individualsForNextGeneration = np.random.choice(pop.individuals, (1-replacementRate)*populationSize, p=[x.fitness / pop.populationFitness for x in pop.individuals], replace=False)
        for individual in individualsForNextGeneration:
            nextPop.addIndividual(individual)


        individualsForMutation = np.random.choice(nextPop.individuals, mutationRate*populationSize, p=[1 / len(nextPop.individuals) for x in nextPop.individuals], replace=False)
        for mutant in individualsForMutation:
            mutate(mutant)

        pop=nextPop
        pop.findFitness()
        c+=1

        print("generation: ",c)
        for i in range(0,12):
            print(pop.individuals[i]," fit: ",pop.individuals[i].fitness)


