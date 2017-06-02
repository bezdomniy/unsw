import numpy as np
import random
import math
from scipy.io.arff import loadarff
import scipy.stats as ss
import string
from collections import OrderedDict
from copy import deepcopy


## Global bariables

#random.seed(1)
populationSize = 50
replacementRate = 0.2
mutationRate = 0.1


## Balance scale definitions

#proportionTrain = 1
#hypothesisLength = 14
variableVal = {'00':'LW','01':'LD','10':'RW','11':'RD'}
operatorVal = {'00':'+','01':'-','10':'/','11':'*'}
variable = ('LW','RW','LD','RD')
operator = ('+','-','/','*')
exampleOrder = {'LW':0,'LD':1,'RW':2,'RD':3}

## Mushroom definitions

proportionTrain = 1
hypothesisLength = 127
mushroomAttributes =    [
                [ 'b', 'c', 'f', 'k', 's', 'x'],
                [ 'f', 'g', 's', 'y'],
                [ 'b', 'c', 'e', 'g', 'n', 'p', 'r', 'u', 'w', 'y'],
                [ 'f', 't'],
                [ 'a', 'c', 'f', 'l', 'm', 'n', 'p', 's', 'y'],
                [ 'a', 'd', 'f', 'n'],
                [ 'c', 'd', 'w'],
                [ 'b', 'n'],
                [ 'b', 'e', 'g', 'h', 'k', 'n', 'o', 'p', 'r', 'u', 'w', 'y'],
                [ 'e', 't'],
                [ 'b', 'c', 'e', 'r', 'u', 'z'],
                [ 'f', 'k', 's', 'y'],
                [ 'f', 'k', 's', 'y'],
                [ 'b', 'c', 'e', 'g', 'n', 'o', 'p', 'w', 'y'],
                [ 'b', 'c', 'e', 'g', 'n', 'o', 'p', 'w', 'y'],
                [ 'p', 'u'],
                [ 'n', 'o', 'w', 'y'],
                [ 'n', 'o', 't'],
                [ 'c', 'e', 'f', 'l', 'n', 'p', 's', 'z'],
                [ 'b', 'h', 'k', 'n', 'o', 'r', 'u', 'w', 'y'],
                [ 'a', 'c', 'n', 's', 'v', 'y'],
                [ 'd', 'g', 'l', 'm', 'p', 'u', 'w'],
                [ 'e', 'p']
                ]

mushroomAttributeLabels =   ['cap-shape',
                            'cap-surface',
                            'cap-color',
                            'bruises?',
                            'odor',
                            'gill-attachment',
                            'gill-spacing',
                            'gill-size',
                            'gill-color',
                            'stalk-shape',
                            'stalk-root',
                            'stalk-surface-above-ring',
                            'stalk-surface-below-ring',
                            'stalk-color-above-ring',
                            'stalk-color-below-ring',
                            'veil-type',
                            'veil-color',
                            'ring-number',
                            'ring-type',
                            'spore-print-color',
                            'population',
                            'habitat',
                            'class']

mushroomVariableLabels =    [
                            {"bell":"b","conical":"c","convex":"x","flat":"f","knobbed":"k","sunken":"s"},
                            {"fibrous":"f","grooves":"g","scaly":"y","smooth":"s"},
                            {"brown":"n","buff":"b","cinnamon":"c","gray":"g","green":"r","pink":"p","purple":"u","red":"e","white":"w","yellow":"y"},
                            {"bruises":"t","no":"f"},
                            {"almond":"a","anise":"l","creosote":"c","fishy":"y","foul":"f","musty":"m","none":"n","pungent":"p","spicy":"s"},
                            {"attached":"a","descending":"d","free":"f","notched":"n"},
                            {"close":"c","crowded":"w","distant":"d"},
                            {"broad":"b","narrow":"n"},
                            {"black":"k","brown":"n","buff":"b","chocolate":"h","gray":"g","green":"r","orange":"o","pink":"p","purple":"u","red":"e","white":"w","yellow":"y"},
                            {"enlarging":"e","tapering":"t"},
                            {"bulbous":"b","club":"c","cup":"u","equal":"e","rhizomorphs":"z","rooted":"r","missing":"?"},
                            {"ibrous":"f","scaly":"y","silky":"k","smooth":"s"},
                            {"ibrous":"f","scaly":"y","silky":"k","smooth":"s"},
                            {"brown":"n","buff":"b","cinnamon":"c","gray":"g","orange":"o","pink":"p","red":"e","white":"w","yellow":"y"},
                            {"brown":"n","buff":"b","cinnamon":"c","gray":"g","orange":"o","pink":"p","red":"e","white":"w","yellow":"y"},
                            {"partial":"p","universal":"u"},
                            {"brown":"n","orange":"o","white":"w","yellow":"y"},
                            {"none":"n","one":"o","two":"t"},
                            {"cobwebby":"c","evanescent":"e","flaring":"f","large":"l","none":"n","pendant":"p","sheathing":"s","zone":"z"},
                            {"black":"k","brown":"n","buff":"b","chocolate":"h","green":"r","orange":"o","purple":"u","white":"w","yellow":"y"},
                            {"abundant":"a","clustered":"c","numerous":"n","scattered":"s","several":"v","solitary":"y"},
                            {"grasses":"g","leaves":"l","meadows":"m","paths":"p","urban":"u","waste":"w","woods":"d"}
                            ]

for i in range(0,len(mushroomVariableLabels)):
    mushroomVariableLabels[i] = {v: k for k, v in mushroomVariableLabels[i].items()}

## Load dataset

#dataset, meta = loadarff(open('C:/dev/unsw/COMP9417/balance-scale.arff','r'))
dataset, meta = loadarff(open('C:/dev/unsw/COMP9417/mushroom.arff','r'))

dataset = dataset[meta.names()].tolist()
dataset = np.asarray(dataset, dtype='<U3')
#dataset = np.asarray(dataset, dtype='<U1')

dataset = np.core.defchararray.replace(dataset,"'","")

random.shuffle(dataset)
train, test = dataset[:len(dataset) * proportionTrain], dataset[len(dataset) * proportionTrain:]

fitnessThreshold = 0.98
#fitnessThreshold = len(train) * len(mushroomAttributes) * 0.95
#fitnessThreshold = len(train) * 0.95

def splitMushroom(data):
    edible = []
    poisonous = []
    for d in data:
        if d[22] == 'e':
            edible.append(d)
        else:
            poisonous.append(d)
    return edible,poisonous

edible,poisonous = splitMushroom(train)

class Node():
    def __init__(self,value):
        self.left = None
        self.right = None
        self.value = value
        self.parent = None

    def __str__(self):
        self.inorder()
        return ''

    def __repr__(self):
        return self.__str__()

    def addLeftChild(self,node):
        self.left = node
        self.left.parent = self

    def addRightChild(self,node):
        self.right = node
        self.right.parent = self

    def inorder(self):
        if self.left is not None:
            self.left.inorder()

        print(self.value,end=' ')

        if self.right is not None:
            self.right.inorder()

    def _evaluate(self,example):
        if self is None:
            return 0
        if self.left is None and self.right is None:
            return int(example[exampleOrder[self.value]])

        leftEval = self.left._evaluate(example)
        rightEval = self.right._evaluate(example)

        if self.value == '+':
            return leftEval + rightEval
        elif self.value == '-':
            return leftEval - rightEval
        elif self.value == '*':
            return leftEval * rightEval
        else:
            if rightEval == 0:
                return 0
            return leftEval / rightEval

    def getSize(self):
        if self.left is None and self.right is None:
            return 1
        leftSize = self.left.getSize()
        rightSize = self.right.getSize()

        return 1 + leftSize + rightSize

    def getHeight(self):
        return int(math.log(self.getSize(),2))

    def replaceNode(self,node,position):
        if position == 0:
            self.left = node.left
            self.right = node.right
            self.value = node.value
            return

        replace = self._traverse(position)
        parent = replace.parent

        node.parent = parent
        #replace.parent = None

        if parent.left == replace:
            parent.left = node
        else:
            parent.right = node


    def getRandomSubtree(self):
        rand = random.randint(0,self.getSize()-1)
        return self._traverse(rand), rand

    def _traverse(self,stop):
        stack = []
        current = self
        step = 0

        while len(stack) > 0 or current is not None:
            if step == stop:
                return current

            if current is not None:
                stack.append(current)
                current = current.left
            else:
                current = stack.pop()
                current = current.right

            if current is not None:
                step += 1

class Attribute():
    def __init__(self,number,value, attribute, label):
        self.number = number
        self.value = value
        self.attribute = attribute
        self.label = label


class Individual():
    def __init__(self):
        self.genes = np.random.choice([0, 1], size=(hypothesisLength,))
        self.fitness = 0
        self.rank = 0

    def setFitness(self,fitness):
        self.fitness = fitness

    def setRank(self,rank):
        self.rank = rank


    def setGenes(self,genes):
        for i in range(0,hypothesisLength):
            self.genes[i] = genes[i]

    def scaleToString(self):
        funcArray = []
        for i in range(0,hypothesisLength,2):
            if i % 4 == 0:
                funcArray.append(variableVal[str(self.genes[i]) + str(self.genes[i + 1])])
            else:
                funcArray.append(operatorVal[str(self.genes[i]) + str(self.genes[i + 1])])
        return ' '.join(funcArray)

    def mushroomToString(self):
        #return ' '.join([str(x) for x in self.genes])
        strOut=[]
        hypothesis = self.genes
        geneCount = 0
        nextStart = 0
        attributeCount = 0

        if (self.genes[125] == self.genes[126] == 0) or (self.genes[125] == self.genes[126] == 1):
            return 'Invalid hypothesis'

        for attrib in mushroomAttributes[:-1]:
            strTemp = []

            nextStart += len(attrib)

            allOnes = True
            for val in attrib:
                if hypothesis[geneCount] == 1:
                    if strTemp == []:
                        strTemp += mushroomVariableLabels[attributeCount][val]
                    else:
                        strTemp += ' or '+ mushroomVariableLabels[attributeCount][val]

                    geneCount = nextStart
                else:
                    allOnes = False
                    geneCount +=1

            if not (allOnes or strTemp == []):
                label = mushroomAttributeLabels[attributeCount]+': '
                strTemp.insert(0,label)
                if strOut == []:
                    strOut += '('+''.join(strTemp)+')'
                else:
                    strOut += ' and ('+''.join(strTemp)+')'
            attributeCount += 1

        if self.genes[125] == 1:
            strOut += ' = Edible'
        else:
            strOut += ' = Poisonous'
        
        return ''.join(strOut)

    #def negativeForm(genes):
    #    pass

    def evaluateMushroom(self,example):
        hypothesis = self.genes
        geneCount = 0
        exampleCount = 0
        nextStart = 0

        if (self.genes[125] == self.genes[126] == 0) or (self.genes[125] == self.genes[126] == 1):
            return False

        if ((example[22] == 'p' and self.genes[126] == 0) or (example[22] == 'e' and self.genes[125] == 0)):
            return True

        for attrib in mushroomAttributes:
            if exampleCount == 22:
                break
            nextStart += len(attrib)
            allZeroes = True
            for val in attrib:
                #print("hypothesis gene: ",hypothesis[geneCount],", attribute: ",val,", example value: ",example[exampleCount])
                if (hypothesis[geneCount] == 1 and (val == example[exampleCount]) or example[exampleCount] == '?'):
                    geneCount = nextStart
                    break
                elif val != example[exampleCount]:
                    if hypothesis[geneCount] == 1:
                        allZeroes = False
                    geneCount +=1
                elif allZeroes:
                    continue
                else:
                    return False
            exampleCount += 1
        return True


    def evaluateScale(self,example):
        funcArray = []
        hypothesis = self.genes
    
        for i in range(0,hypothesisLength,2):
            if i % 4 == 0:
                funcArray.append(variableVal[str(hypothesis[i]) + str(hypothesis[i + 1])])
            else:
                funcArray.append(operatorVal[str(hypothesis[i]) + str(hypothesis[i + 1])])
    
        func = [example[exampleOrder[x]] if x in exampleOrder is not None else x for x in funcArray]
        return eval(''.join(func))

    def __str__(self):
        return self.mushroomToString()
        #return self.scaleToString()

    def __repr__(self):
        return self.__str__()

class scaleIndividual():
    def __init__(self):
        self.root = Node(random.choice(operator))
        self.root.addLeftChild(Node(random.choice(operator)))
        self.root.addRightChild(Node(random.choice(operator)))

        self.root.left.addLeftChild(Node(random.choice(variable)))
        self.root.left.addRightChild(Node(random.choice(variable)))
        self.root.right.addLeftChild(Node(random.choice(variable)))
        self.root.right.addRightChild(Node(random.choice(variable)))

        self.fitness = 0


    def setFitness(self,fitness):
        self.fitness = fitness

    def evaluateScale(self,example):
        return self.root._evaluate(example)

    def __str__(self):
        self.root.inorder()
        return ''

    def __repr__(self):
        return self.__str__()

    

class Population():
    def __init__(self):
        self.individuals = []
        self.populationFitness = 0
        self.totalRank = 0
        self.maxFitness = 0

    def createPopulation(self):
        self.individuals = [Individual() for _ in range(populationSize)]

    def addIndividual(self,individual):
        self.individuals.append(deepcopy(individual))

    def findFitness(self):
        self._fitnessProportionate()
        self._rankSelection()

    def _rankSelection(self):
        ranked = ss.rankdata([i.fitness for i in self.individuals])

        #for r in ranked:
            #r = populationSize - r - 1

        for i in range(0,populationSize):
            self.totalRank += ranked[i]
            self.individuals[i].setRank(ranked[i])

        #self.individuals.sort(key=lambda x: x.fitness ,reverse=True)

    def _fitnessProportionate(self):
        for individual in self.individuals:
            fitness = fitnessMushroom(individual)
            self.populationFitness += fitness
            individual.setFitness(fitness)
            if fitness > self.maxFitness:
                self.maxFitness = fitness

        #self.individuals.sort(key=lambda x: x.fitness ,reverse=True)





def fitnessScale(individual):
    fitness = 0

    for example in train:
        if testScaleHypothesis(individual,example):
            fitness += 1

    #print("fit: ",fitness)
    #return (fitness/len(train)) ** 2
    return fitness

def fitnessMushroom(individual):
    edibleFitness = 0
    poisonousFitness = 0

    for example in edible:
        edibleFitness += testMushroomHypothesis(individual,example)
    for example in poisonous:
        poisonousFitness += testMushroomHypothesis(individual,example)

    #print(edibleFitness," ",poisonousFitness)

    fitness = (edibleFitness / len(edible)) + (poisonousFitness / len(poisonous))

    #print("fit: ",fitness)
    #return (fitness/len(train)) ** 2
    return fitness/2


def testScaleHypothesis(hypothesis,example):
    ev = hypothesis.evaluateScale(example)
    if ev == 0:
        return example[4] == "B"
    if ev > 0:
        return example[4] == "R"
    if ev < 0:
        return example[4] == "L"

def testMushroomHypothesis(hypothesis,example):
    return hypothesis.evaluateMushroom(example)



def crossover(individual1,individual2):
    #return _onePointCrossover(individual1,individual2)
    return _twoPointCrossover(individual1,individual2)
    #return _uniformCrossover(individual1,individual2)


def _subtreeCrossover(individual1,individual2):
    replacement1,pos1 = individual1.root.getRandomSubtree();
    replacement2,pos2 = individual2.root.getRandomSubtree();

    r1=deepcopy(replacement1)
    r2=deepcopy(replacement2)

    individual1.root.replaceNode(r2,pos1)
    individual2.root.replaceNode(r1,pos2)

    return individual1,individual2


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

def _uniformCrossover(individual1,individual2):
    parents = [individual1,individual2]

    crossoverMask = [random.randint(0,1) for _ in range(0,hypothesisLength)]

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

def _mutate(individual):
    mutation,pos = individual.root.getRandomSubtree();

    if mutation.value in '+-*/':
        mutation.value = random.choice(operator)
    else:
        mutation.value = random.choice(variable)



def runGA():
    pop = Population()
    pop.createPopulation()
    pop.findFitness()

    limit = 1000
    c=0
   

    while pop.maxFitness < fitnessThreshold and c < limit:
        nextPop = Population()

        #individualsForCrossover = np.random.choice(pop.individuals, replacementRate*populationSize, p=[x.fitness / pop.populationFitness for x in pop.individuals], replace=False)
        individualsForCrossover = np.random.choice(pop.individuals, replacementRate*populationSize, p=[x.rank / pop.totalRank for x in pop.individuals], replace=False)
        for i in range(0,len(individualsForCrossover),2):
            offspring1, offspring2 = crossover(individualsForCrossover[i],individualsForCrossover[i +1 ])
            nextPop.addIndividual(offspring1)
            nextPop.addIndividual(offspring2)

        #individualsForNextGeneration = np.random.choice(pop.individuals, (1-replacementRate)*populationSize, p=[x.fitness / pop.populationFitness for x in pop.individuals], replace=False)
        individualsForNextGeneration = np.random.choice(pop.individuals, (1-replacementRate)*populationSize, p=[x.rank / pop.totalRank for x in pop.individuals], replace=False)
        for individual in individualsForNextGeneration:
            nextPop.addIndividual(individual)


        individualsForMutation = np.random.choice(nextPop.individuals, mutationRate*populationSize, p=[1 / len(nextPop.individuals) for x in nextPop.individuals], replace=False)
        for mutant in individualsForMutation:
            mutate(mutant)

        pop=nextPop
        pop.findFitness()
        c+=1

        print("generation: ",c)
        pop.individuals.sort(key=lambda x: x.rank ,reverse=True)
        for i in range(0,12):
            print("Rank: ", pop.individuals[i].rank,", Fit: {0:.2f}%".format(pop.individuals[i].fitness*100)," Hypothesis ",pop.individuals[i])
            #print(pop.individuals[i]," fit: ",pop.individuals[i].fitness," rank", pop.individuals[i].rank)
            #print(pop.individuals[i]," fit: {0:.0f}%".format(pop.individuals[i].fitness*100))
        

    if c == limit:
        print("failed at {}, reseting".format(limit))
        runGA()
    else:
        print("Found solution: ", pop.individuals[0],", at generation: ",c)
    #print("Top 3 solutions: ")
    #for i in range(0,1):
        #pop._fitnessProportionate()
        #print(pop.individuals[i]," fit: ",pop.individuals[i].fitness)
    

    return c

def _test(n):
    generationsTaken = []
    for i in range(0,n):
        c = runGA()
        generationsTaken.append(c)
    print("Average generations taken: ",sum(generationsTaken)/len(generationsTaken))


def testHypothesis(individual,data):
    fitness = 0

    for example in data:
        if individual.evaluateMushroom(example):
            fitness+=1
    return fitness

print(len(edible)," edible examples")
print(len(poisonous)," poisonous examples")

#x='1 1 1 1 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0'
#x='0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0'
#x='0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 0 0 0 0 1'
#x='1 0 1 1 1 1 1 1 1 1 0 1 1 1 1 1 1 0 1 1 1 1 1 1 1 0 0 1 1 0 1 1 1 1 1 1 1 1 1 1 1 1 0 1 1 1 0 1 0 1 0 1 1 1 1 1 1 1 0 0 0 1 1 1 1 1 1 1 1 1 0 1 1 0 1 1 1 0 1 1 1 1 1 1 1 1 1 1 0 1 1 0 1 1 0 0 1 1 0 0 1 0 1 1 1 1 1 1 1 0 0 0 1 1 1 1 1 1 1 1 1 1 1 0 1 1 0'
#x='0 1 1 1 0 0 0 0 1 0 0 1 1 1 0 0 0 0 1 1 1 1 1 1 0 1 0 1 1 0 1 0 0 1 0 0 1 0 1 1 0 0 1 1 1 0 1 0 1 0 0 0 0 0 1 0 0 0 1 0 0 0 0 1 0 0 0 1 1 0 0 0 0 0 0 1 0 0 1 0 0 0 1 1 0 1 1 0 1 0 1 0 0 1 1 0 1 1 0 1 1 0 0 1 0 1 0 0 1 1 0 0 1 1 1 0 1 1 1 1 1 1 0 1 1 1 0'

# Found on full dataset, with 98.61% fitness
x=' 0 0 0 0 0 0 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 1 0 1 1 0 0 1 1 1 1 0 1 1 1 1 0 1 0 1 1 1 0 0 0 0 0 1 1 1 1 1 0 0 0 0 0 1 0 0 0 1 0 1 1 0 1 1 0 0 0 1 0 1 1 1 0 0 0 1 1 0 0 1 0 1 1 1 1 1 1 0 1 1 1 1 1 1 0 0 1 0 0 0 0 0 0 0 0 0 0 0 0 0 1 0 1'

# Found on full dataset, with 98.39% fitness
#x='0 0 1 1 1 1 0 0 0 1 1 1 1 1 1 0 1 1 1 1 0 0 1 1 0 1 1 1 1 1 1 0 0 1 1 1 0 1 1 1 0 0 0 0 0 0 1 1 1 0 1 0 1 1 1 1 1 1 0 1 1 1 1 0 0 1 1 1 1 0 0 0 1 0 1 1 1 0 1 0 0 1 0 1 1 0 0 0 0 0 0 0 0 1 1 1 0 1 1 0 0 1 0 0 1 1 1 0 1 0 1 1 0 1 0 1 1 1 0 0 0 0 0 1 1 0 1'

x=[int(i) for i in x if i != ' ']
y = Individual()
y.setGenes(x)
testHypothesis(y,dataset)