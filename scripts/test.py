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

# results_txt = """{"algorithm": "IGAfull", "A1": 0.550053}
# {"algorithm": "IGAfull", "A2": 0.812279}
# {"algorithm": "IGAfull", "A3": 0.815154}
# {"algorithm": "IGAfull", "A4": 0.939388}
# {"algorithm": "IGAfull", "B1": 1.05829}
# {"algorithm": "IGAfull", "B2": 1.46396}
# {"algorithm": "IGAfull", "B3": 1.6429}
# {"algorithm": "IGAfull", "B4": 1.91273}
# {"algorithm": "IGA_bit", "A1": 0.158383}
# {"algorithm": "IGA_bit", "A2": 0.224205}
# {"algorithm": "IGA_bit", "A3": 0.24367}
# {"algorithm": "IGA_bit", "A4": 0.287175}
# {"algorithm": "IGA_bit", "B1": 0.284667}
# {"algorithm": "IGA_bit", "B2": 0.455754}
# {"algorithm": "IGA_bit", "B3": 0.532304}
# {"algorithm": "IGA_bit", "B4": 0.567801}
# {"algorithm": "IGA", "A1": 0.273261}
# {"algorithm": "IGA", "A2": 0.344487}
# {"algorithm": "IGA", "A3": 0.37643}
# {"algorithm": "IGA", "A4": 0.389899}
# {"algorithm": "IGA", "B1": 0.612308}
# {"algorithm": "IGA", "B2": 0.805438}
# {"algorithm": "IGA", "B3": 0.877023}
# {"algorithm": "IGA", "B4": 0.882526}
# {"algorithm": "KTNS", "A1": 1.8576}
# {"algorithm": "KTNS", "A2": 1.90265}
# {"algorithm": "KTNS", "A3": 1.62117}
# {"algorithm": "KTNS", "A4": 1.47916}
# {"algorithm": "KTNS", "B1": 6.71067}
# {"algorithm": "KTNS", "B2": 6.17351}
# {"algorithm": "KTNS", "B3": 5.40777}
# {"algorithm": "KTNS", "B4": 4.85053}"""

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


plt.rcParams["figure.figsize"] = (13,5)
fig, ax = plt.subplots()
plt.gca().set_xticks(range(n))
ax.set_xticklabels(labels)


for alg_name, results in alg2results.items():
    ax.plot(list(range(n)), results, label=alg_name, color=colors[alg_name])


plt.legend()
plt.grid(True)
plt.xlabel("dataset name")
plt.ylabel("time, sec")
plt.show()