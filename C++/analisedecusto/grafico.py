import pandas as pd
import matplotlib.pyplot as plt

dell115=pd.read_csv('dell 1 15s.csv')
dell215=pd.read_csv('dell 2 15s.csv')
dell315=pd.read_csv('dell 3 15s.csv')

dell110=pd.read_csv('dell 1 10s.csv')
dell210=pd.read_csv('dell 2 10s.csv')
dell310=pd.read_csv('dell 3 10s.csv')

dell105=pd.read_csv('dell 1 5s.csv')
dell205=pd.read_csv('dell 2 5s.csv')
dell305=pd.read_csv('dell 3 5s.csv')


cpu_15=dell115['pid'].mean() + dell215['pid'].mean() + dell315['pid'].mean()/3
desvio_15=dell115['pid'].std() + dell215['pid'].std() + dell315['pid'].std()/3
desvio_10=dell110['pid'].std() + dell210['pid'].std() + dell310['pid'].std()/3
cpu_10=dell110['pid'].mean() + dell210['pid'].mean() + dell310['pid'].mean()/3
cpu_5=dell105['pid'].mean() + dell205['pid'].mean() + dell305['pid'].mean()/3
desvio_5=dell105['pid'].std() + dell205['pid'].std() + dell305['pid'].std()/3
    





labels = ["5s", "10s", "15s"]
medias = [cpu_5, cpu_10, cpu_15]
desvios = [desvio_5, desvio_10, desvio_15]

plt.figure(figsize=(7,5))

plt.errorbar(
    labels,
    medias,
    yerr=desvios,
    fmt='-o',        # linha com marcadores
    linewidth=2,
    markersize=8,
    capsize=8,
)

plt.title("Uso médio de CPU")
plt.xlabel("Intervalo de coleta")
plt.ylabel("CPU (%)")
plt.grid(True, linestyle="--", alpha=0.6)

plt.tight_layout()
plt.savefig("grafico_cpu.png", dpi=300, bbox_inches="tight")
plt.close()