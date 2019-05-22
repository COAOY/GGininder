import matplotlib
import matplotlib.pyplot as plt
import seaborn; seaborn.set_style('whitegrid')
import numpy

from pomegranate import *

d1 = DiscreteDistribution({True: 0.2, False: 0.8})
d2 = DiscreteDistribution({True: 0.6, False: 0.4})
d3 = ConditionalProbabilityTable(
        [[True,  True,  True,  0.2],
         [True,  True,  False, 0.8],
         [True,  False, True,  0.3],
         [True,  False, False, 0.7],
         [False, True,  True,  0.9],
         [False, True,  False, 0.1],
         [False, False, True,  0.4],
         [False, False, False, 0.6]], [d1, d2])

s1 = State(d1, name="s1")
s2 = State(d2, name="s2")
s3 = State(d3, name="s3")

model = BayesianNetwork("APItest")
model.add_states(s1, s2, s3)
model.add_edge(s1, s3)
model.add_edge(s2, s3)
model.bake()

numpy.random.seed(111)

X = numpy.random.randint(2, size=(15, 15))
X[:,5] = X[:,4] = X[:,3]
X[:,11] = X[:,12] = X[:,13]
print(X)

model = BayesianNetwork.from_samples(X,algorithm='exact')
model.plot("APItest")