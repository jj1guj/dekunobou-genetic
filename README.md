# dekunobou-genetic
遺伝的アルゴリズムでオセロの評価関数を生成する.  
Twitter上では<del>[遺伝的アルゴリズムくん](https://twitter.com/jj1guj/status/1398257722638835717?s=20)</del>[でくのぼうくん](https://twitter.com/jj1guj/status/1398625513216692230?s=20)と呼称しています.
# 現状できること
1手読みのAI同士を対戦させることでより勝率の高い評価関数を生成する.  
# 動作環境
<del>WSL又はLinux系OS</del>Windows(VisualStudio)でもビルドできると思います  
ビルドに当たりcmake, make, OpenMPが必要です.  
可視化用のPythonスクリプトではmatplotlib, OpenCV, numpy, tqdmが必要です.
# ビルド
```
mkdir build && cd build
cmake ..
make
```
# 実行
```
./dekunobou
```
スレッド数を指定したい場合は
```
./dekunobou [スレッド数]
```
スレッド数の指定がないと動作環境での最大スレッド数で動作します

# 生成した評価関数(eval.txt)を可視化したい
numpy, matplotlibが必要です.
```
python eval_visualize.py
```
これで.pngファイルが出力されます.
