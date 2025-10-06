import psutil
import time
import os

def monitorar_processos(intervalo=1, top_n=20):
    """
    Monitora processos em tempo real.

    :param intervalo: tempo em segundos entre atualizações
    :param top_n: número de processos mais pesados a mostrar
    """
    try:
        while True:
            os.system('clear')  # no Windows use 'cls'
            print(f"{'PID':>6} {'CPU%':>6} {'MEM%':>6} {'NAME'}")
            print("-" * 40)

            lista = []
            for proc in psutil.process_iter(['pid', 'name', 'cpu_percent', 'memory_percent']):
                try:
                    pid = proc.info['pid']
                    nome = proc.info['name']
                    cpu = proc.info['cpu_percent']
                    mem = proc.info['memory_percent']
                    lista.append((pid, cpu, mem, nome))
                except (psutil.NoSuchProcess, psutil.AccessDenied):
                    continue

            # Ordena por CPU decrescente
            lista.sort(key=lambda x: x[1], reverse=True)

            for pid, cpu, mem, nome in lista[:top_n]:
                print(f"{pid:6} {cpu:6.2f} {mem:6.2f} {nome}")

            time.sleep(intervalo)
    except KeyboardInterrupt:
        print("\nMonitoramento finalizado.")

if __name__ == "__main__":
    # Inicializa CPU% de todos os processos
    for proc in psutil.process_iter():
        proc.cpu_percent(interval=None)

    monitorar_processos(intervalo=1, top_n=20)
