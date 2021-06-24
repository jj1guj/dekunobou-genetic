import matplotlib
matplotlib.use("Agg")
from matplotlib import pyplot as plt
import numpy as np

#対応表
ref_table=[
     0,1,2,3,3,2,1,0,
     1,4,5,6,6,5,4,1,
     2,5,7,8,8,7,5,2,
     3,6,8,9,9,8,6,3,
     3,6,8,9,9,8,6,3,
     2,5,7,8,8,7,5,2,
     1,4,5,6,6,5,4,1,
     0,1,2,3,3,2,1,0,
]
#最終的に出力された評価関数を可視化する
path="build/eval.txt"
with open(path) as f:
    L=f.readlines()
L=[float(i.replace("\n","")) for i in L]

for p in range(3):
    a=L[11*p+9]
    board=np.array([L[11*p+ref_table[i]] for i in range(64)])
    board=board.reshape((8,8))

    plt.figure()
    plt.imshow(board,interpolation="nearest",vmin=-1,vmax=1,cmap="jet")
    plt.colorbar()
    plt.savefig("eval_{}.png".format(p))