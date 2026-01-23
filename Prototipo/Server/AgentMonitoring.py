import threading
import time
from Client.Monitoring.MonitoringManages import MonitoringManages
from Client.Monitoring.ElastichSearch import ElasticsearchClient
import distro

semaphore_alerta = threading.Semaphore(0)  # Inicialmente bloqueado
semaphore_alerta_versao = threading.Semaphore(0)  # Inicialmente bloqueado


def sistema_info():
    # Nome da distribuição e versão
    nome_distro = distro.name(pretty=True)
    versao_distro = distro.version()
    
    def kernel_version():
        with open("/proc/version", "r") as f:
            return f.read().strip()
    
    return f"{nome_distro} {versao_distro}, Kernel: {kernel_version()}"


def start_monitoring_sistema_info():
    semaphore_alerta_versao.acquire()
    info = sistema_info()
    print(f"Sistema Operacional: {info}")
    
    

def start_monitoring():
    monitor = MonitoringManages(config_path_env="Client/env.yaml")
    monitor.run(semaphore=semaphore_alerta)


def start_elastic_search():
    elastic_search = ElasticsearchClient(path="Client/env.yaml")
    
    while True:
        semaphore_alerta.acquire()  # espera o sinal para enviar dados
        try:
            if elastic_search.is_connected():
                print("✅ Conectado ao Elasticsearch.")
                # Método público que envia todos os dados usando ThreadPool
                elastic_search.all_send_data(index_name="monitoring_data")
                print("✅ Dados enviados com sucesso ao Elasticsearch.")
                
            else:
                print("❌ Não foi possível conectar ao Elasticsearch. Tentando novamente em 5s...")
                time.sleep(5)  # evita loop de CPU alta
        except Exception as e:
            print(f"❌ Erro durante o envio: {e}")
        finally:
            semaphore_alerta.acquire()  # garante que o semáforo seja liberado


    
    

if __name__ == "__main__":
    thread_monitoring = threading.Thread(target=start_monitoring)
    thread_elastic_search = threading.Thread(target=start_elastic_search)
    threads_sistemas = threading.Thread(target=start_monitoring_sistema_info)
      
    thread_monitoring.start()
    thread_elastic_search.start()
    thread_monitoring.join()
    thread_elastic_search.join()
    
    

       