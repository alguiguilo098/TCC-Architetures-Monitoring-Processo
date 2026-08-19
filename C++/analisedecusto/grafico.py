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

lenovo15=pd.read_csv('lenovo 1 5s.csv')
lenovo25=pd.read_csv('lenovo 2 5s.csv')
lenovo35=pd.read_csv('lenovo 3 5s.csv')

lenovo110=pd.read_csv('lenovo 1 10s.csv')
lenovo210=pd.read_csv('lenovo 2 10s.csv')
lenovo310=pd.read_csv('lenovo 3 10s.csv')

lenovo15=pd.read_csv('lenovo.csv')

k=lenovo110['cpu_percent'].std()
cpu_15=(dell115['pid'].mean() + dell215['pid'].mean() + dell315['pid'].mean())/3
desvio_15=(dell115['pid'].std() + dell215['pid'].std() + dell315['pid'].std())/3
desvio_10=(dell110['pid'].std() + dell210['pid'].std() + dell310['pid'].std())/3
cpu_10=(dell110['pid'].mean() + dell210['pid'].mean() + dell310['pid'].mean())/3
cpu_5=(dell105['pid'].mean() + dell205['pid'].mean() + dell305['pid'].mean())/3
desvio_5=(dell105['pid'].std() + dell205['pid'].std() + dell305['pid'].std())/3


lenovo_5=(lenovo15['cpu_percent'].mean() + lenovo25['cpu_percent'].mean() + lenovo35['cpu_percent'].mean())/3
lenovo_desvio_5=(lenovo15['cpu_percent'].std() + lenovo25['cpu_percent'].std() + lenovo35['cpu_percent'].std())/3
lenovo110=(lenovo110['cpu_percent'].mean() + lenovo210['cpu_percent'].mean() + lenovo310['cpu_percent'].mean())/3
lenovo_desvio_10=(k + lenovo210['cpu_percent'].std() + lenovo310['cpu_percent'].std())/3
labels = ["5s", "10s", "15s"]
medias = [cpu_5, cpu_10, cpu_15]
desvios = [desvio_5, desvio_10, desvio_15]

medias_lenovo = []
print(medias_lenovo)
plt.figure(figsize=(7,5))

plt.errorbar(
    labels,
    medias,
    yerr=desvios,
    fmt='-o',        
    linewidth=2,
    markersize=8,
    capsize=8,
    label="i7 12ª geração"
)
plt.errorbar(
    ["5s", "10s", "15s"],
    [lenovo_5, lenovo110, lenovo15['cpu_percent'].mean()],
    yerr=[lenovo_desvio_5, lenovo_desvio_10, lenovo15['cpu_percent'].std()],
    fmt='-o',        
    color='red',
    markersize=8,
    capsize=8,
    label="i3 10ª geração"
)

plt.legend()
plt.title("Uso médio de CPU")
plt.xlabel("Intervalo de coleta")
plt.ylabel("CPU (%)")
plt.grid(True, linestyle="--", alpha=0.6)

plt.tight_layout()
plt.savefig("grafico_cpu.png", dpi=300, bbox_inches="tight")
plt.close()
