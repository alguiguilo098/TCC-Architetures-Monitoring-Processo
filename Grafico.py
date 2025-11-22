import matplotlib.pyplot as plt

# Dados fornecidos
tempos = [5,10,15, 20]
cpu = [7.7,6.6,5.7,3.4]
mem = [0.3,0.3, 0.3, 0.3, ]

plt.figure(figsize=(8,5))
plt.plot(tempos, cpu, marker='o', label="CPU (%)")
plt.plot(tempos, mem, marker='o', label="Memória (%)")
plt.xlabel("Tempo (segundos)")
plt.ylabel("Valores")
plt.title("Uso de CPU e Memória")
plt.legend()
plt.grid(True)
plt.show()
