import pandas as pd 
import numpy as np 
from matplotlib import pyplot as plt 

y = pd.read_csv("../build-QT_ADC_1-Desktop-Profile/parametros.txt")

plt.scatter(y.index, y.iloc[:,0], marker='.')
plt.plot(y.index, y.iloc[:,0])
plt.scatter(y.index, y.iloc[:,1], marker='.')
plt.plot(y.index, y.iloc[:,1])
plt.scatter(y.index, y.iloc[:,2], marker='.')
plt.plot(y.index, y.iloc[:,2])
plt.grid()
#plt.ylim(0,4096)
plt.show()

y = pd.read_csv("../build-QT_ADC_1-Desktop-Profile/plot.txt")
plt.scatter(y.index, y.iloc[:,0], marker='.')
plt.plot(y.index, y.iloc[:,0])
plt.grid()
plt.show()