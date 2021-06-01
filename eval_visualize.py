import matplotlib
matplotlib.use("Agg")
from matplotlib import pyplot as plt
import numpy as np

#最終的に出力された評価関数を可視化する
path="build/eval.txt"
with open(path) as f:
    L=f.readlines()
a=float(L[64].replace("\n",""))
L=np.array([float(i.replace("\n","")) for i in L[:64]])
L=L.reshape((8,8))

plt.figure()
plt.imshow(L,interpolation="nearest",vmin=-1,vmax=1,cmap="jet")
plt.colorbar()
plt.savefig("eval.png")

L*=a
plt.clf()
plt.imshow(L,interpolation="nearest",vmin=-1,vmax=1,cmap="jet")
plt.colorbar()
plt.savefig("eval_opponent.png")