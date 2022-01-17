import pandas as pd 
import numpy as np 
from matplotlib import pyplot as plt 

y = pd.read_csv("/tmp/parametros.txt")

plt.figure()
plt.plot(y.index, y.iloc[:,0], marker='.')
plt.plot(y.index, y.iloc[:,1], marker='.')
plt.plot(y.index, y.iloc[:,2], marker='.')
plt.grid()
plt.ylim(0,4096)
plt.show(block=False)

plt.figure()
y = pd.read_csv("/tmp/plot.txt")
plt.plot(y.index, y.iloc[:,0], marker='.', color='red')
plt.grid()
plt.show()
