#これを参考にした
#https://spcx8.hatenablog.com/entry/2018/10/09/070000
import matplotlib
matplotlib.use("Agg")
from matplotlib import pyplot as plt
import numpy as np

filename="build/out_1.csv"
with open(filename) as f:
    data=f.readlines()
data=[i.replace("\n","") for i in data]
data=[i.split(",")[:-1] for i in data]
data=[[float(j) for j in i] for i in data]
data=np.array(data[:100])
print(data.shape)

#ヒートマップの描画
plt.figure()
plt.imshow(data,interpolation="nearest",vmin=-1,vmax=1,cmap="jet")
plt.savefig("test.png")
