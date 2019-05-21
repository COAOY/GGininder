import matplotlib
import matplotlib.pyplot as plt
import seaborn; seaborn.set_style('whitegrid')
import numpy

from pomegranate import *

d1 = DiscreteDistribution({True: 1./2, False: 1./2})
d2 = DiscreteDistribution({True: 1./2, False: 1./2})
AND = ConditionalProbabilityTable(
        [[True,  True,  True,  1.0],
         [True,  True,  False, 0.0],
         [True,  False, True,  0.0],
         [True,  False, False, 1.0],
         [False, True,  True,  0.0],
         [False, True,  False, 1.0],
         [False, False, True,  0.0],
         [False, False, False, 1.0]], [d1, d2])

OR = ConditionalProbabilityTable(
        [[True,  True,  True,  1.0],
         [True,  True,  False, 0.0],
         [True,  False, True,  1.0],
         [True,  False, False, 0.0],
         [False, True,  True,  1.0],
         [False, True,  False, 0.0],
         [False, False, True,  1.0],
         [False, False, False, 0.0]], [d1, d2])


i1 = State(d1 , name="input1")
i2 = State(d2 , name="input2")
i3 = State(d1 , name="input3")

model = BayesianNetwork("Circuit")
ga2 = State(AND, name="AND2")
ga3 = State(AND, name="AND3")
# model.add_states(i1,i2,i3,ga2,ga3)
model.add_states(i1,i2,ga2)

model.add_edge(i1,ga2)
model.add_edge(i2,ga2)
# model.add_edge(i3,ga3)
# model.add_edge(ga2,ga3)
model.bake()

print(model.probability({'input1' : 'True', 'input2' : 'True'}))
print(model.predict_proba({}))
model.plot("YoyoToy")