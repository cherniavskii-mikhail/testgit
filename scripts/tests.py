import matplotlib.pyplot as plt
import os, sys

import subprocess
import json

proc = subprocess.Popen([os.path.join(os.getcwd(), "main")], stdout=subprocess.PIPE)

while True:
  line = proc.stdout.readline()
  print(line)
  if not line:
    break


with open(".//results/results.txt", "r") as f:
    results_txt = f.read().strip()
    print(results_txt)

alg2results = dict()
for line in results_txt.split("\n"):
    data = json.loads(line)
    dataset_name = str(list(set(data.keys()) - set(["algorithm"]))[0])
    alg = str(data["algorithm"])
    alg2results[alg] =  alg2results.get(alg, []) + [(dataset_name, data[dataset_name])]

colors = {
    "KTNS": "blue", 
    "IGA-bit": "orange", 
    "IGA": "green", 
    "IGA-full": "red"
}


for k in colors.keys():
    labels = list(map(lambda x : x[0], sorted(alg2results[k])))
    alg2results[k] = list(map(lambda x : x[1], sorted(alg2results[k])))
n = len(labels)

plt.rcParams["figure.figsize"] = (10,5)
fig, ax = plt.subplots()
plt.gca().set_xticks(range(n))
ax.set_xticklabels(labels)

for alg_name, results in alg2results.items():
    ax.plot(list(range(n)), results, label=alg_name, color=colors[alg_name])

plt.legend(loc='upper left')
plt.grid(True)
plt.xlabel("dataset name")
plt.ylabel("time, sec")
#fig.savefig('results.png', format='png', dpi=300)
plt.show()
