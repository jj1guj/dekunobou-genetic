#これを参考にした
#https://spcx8.hatenablog.com/entry/2018/10/09/070000
#https://tomosoft.jp/design/?p=29005
import cv2
import matplotlib
matplotlib.use("Agg")
from matplotlib import pyplot as plt
import numpy as np
import os
import tqdm

file_path="build/data/"
file_num=len(os.listdir(file_path))

#動画の設定
mov_h=480
mov_w=640
fourcc=cv2.VideoWriter_fourcc("m","p","4","v")
video=cv2.VideoWriter("result.mp4",fourcc,5.0,(mov_w,mov_h),True)

#刻み幅を指定
L=[i for i in range(0,100*file_num-99,100)]
L[0]=1
print(L)

plt.figure()
for j in tqdm.tqdm(range(len(L))):
    i=L[j]
    filename=file_path+"out_{}.csv".format(i)
    with open(filename) as f:
        data=f.readlines()
    data=[i.replace("\n","") for i in data]
    data=[i.split(",")[:-1] for i in data]
    data=[[float(j) for j in i] for i in data]
    data=np.array(data)

    #ヒートマップの描画
    plt.clf()
    plt.imshow(data,interpolation="nearest",vmin=-1,vmax=1,cmap="jet",aspect=0.05)
    plt.colorbar()
    plt.title("Generation: {}".format(i))
    plt.savefig("out/out_{}.png".format(i))

    #動画への出力
    img=cv2.imread("out/out_{}.png".format(i))
    video.write(img)
video.release()