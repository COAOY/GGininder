import matplotlib
import matplotlib.pyplot as plt
import seaborn; seaborn.set_style('whitegrid')
import numpy
import copy

from pomegranate import *

d1 = DiscreteDistribution({True: 0.5, False: 0.5})
d2 = DiscreteDistribution({True: 0.5, False: 0.5})
d3 = DiscreteDistribution({True: 0.5, False: 0.5})
AND = ConditionalProbabilityTable(
        [[True,  True,  True,  1.0],
         [True,  True,  False, 0.0],
         [True,  False, True,  0.0],
         [True,  False, False, 1.0],
         [False, True,  True,  1.0],
         [False, True,  False, 0.0],
         [False, False, True,  1.0],
         [False, False, False, 0.0]], [d1, d2])

AND2 = ConditionalProbabilityTable(
        [[True,  True,  True,  1.0],
         [True,  True,  False, 0.0],
         [True,  False, True,  0.0],
         [True,  False, False, 1.0],
         [False, True,  True,  1.0],
         [False, True,  False, 0.0],
         [False, False, True,  1.0],
         [False, False, False, 0.0]], [d3, AND])

# OR = ConditionalProbabilityTable(
#         [[True,  True,  True,  1.0],
#          [True,  True,  False, 0.0],
#          [True,  False, True,  1.0],
#          [True,  False, False, 0.0],
#          [False, True,  True,  1.0],
#          [False, True,  False, 0.0],
#          [False, False, True,  1.0],
#          [False, False, False, 0.0]], [d1, d2])

# NOT = ConditionalProbabilityTable(
#         [[True,  True,  0.0],
#          [True,  False, 1.0],
#          [False, True,  1.0],
#          [False, False, 0.0],], [d3])


i1 = State(d1 , name="input1")
i2 = State(d2 , name="input2")
i3 = State(d3 , name="input3")
# i4 = State(d1 , name="input4")
# i5 = State(d2 , name="input5")
# i6 = State(d1 , name="input6")
# i7 = State(d2 , name="input7")
# ga = State(AND, name="AND")
# go = State(OR , name="OR")
# gn = State(NOT, name="NOT")

ga1 = State(AND, name="AND1")
ga2 = State(AND2, name="AND2")
# ga3 = State(AND, name="AND3")
# go4 = State(OR , name="OR4")
# ga5 = State(AND, name="AND5")
# go6 = State(OR , name="OR6")
# ga7 = State(AND, name="AND7")

model = BayesianNetwork("Circuit")
model.add_states(i1,i2,i3,ga1,ga2)
# model.add_states(i1,i2,i3,i4,i5,i6,gn1,ga2,ga3,go4,ga5,go6,ga7)
model.add_edge(i1, ga1)
model.add_edge(i2,ga1)
model.add_edge(ga1,ga2)
model.add_edge(i3,ga2)
# model.add_edge(i4,ga3)
# model.add_edge(i5,ga3)
# model.add_edge(i6,go4)
# model.add_edge(i6,go4)
# model.add_edge(ga2,ga5)
# model.add_edge(ga3,ga5)
# model.add_edge(ga5,go6)
# model.add_edge(go4,go6)
# model.add_edge(gn1,ga7)
# model.add_edge(go6,ga7)
# model.add_edge(ga7,buf1)
model.bake()

# print(model.structure)
print(model.predict_proba({"AND2": True}))
# model.probability({"AND2": "True"})
model.plot("Circuit")

# print('Probability Buffer: ',model.probability(['A', 'B', 'C']) )


# numpy.random.seed(111)

# X = numpy.random.randint(2, size=(15, 15))
# X[:,5] = X[:,4] = X[:,3]
# X[:,11] = X[:,12] = X[:,13]
# print(X)

# model = BayesianNetwork.from_samples(X,algorithm='exact')
# model.plot("APItest")