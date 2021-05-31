# dekunobou-genetic
遺伝的アルゴリズムでオセロの評価関数を生成する.  
Twitter上では<del>[遺伝的アルゴリズムくん](https://twitter.com/jj1guj/status/1398257722638835717?s=20)</del>[でくのぼうくん](https://twitter.com/jj1guj/status/1398625513216692230?s=20)と呼称しています.
# 現状できること
1手読みのAI同士を対戦させることでより勝率の高い評価関数を生成する.  
# 動作環境
WSL又はLinux系OS  
ビルドに当たりcmake, makeが必要です.
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
