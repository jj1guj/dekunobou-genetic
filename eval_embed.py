path="build/eval.txt"
with open(path, 'r') as f:
    L=f.readlines()
L=[i.replace("\n",",\n") for i in L]

with open("eval_embed.txt", 'w') as f:
    f.writelines(L)