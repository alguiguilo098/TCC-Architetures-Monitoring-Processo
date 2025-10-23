import threading
from Client.Monitoring.MonitoringManages import MonitoringManages
from Client.AlertaManeger.AlertaManager import AlertaManager
from Client.DBmanages.TinyDBManages import TinyDBManages


semaphore_agent = threading.Semaphore()
semaphore_alerta = threading.Semaphore()

def start_monitoring():
    monitor = MonitoringManages(config_path_env="Client/env.yaml")
    monitor.run()
    

if __name__ == "__main__":
    monitoring_thread = threading.Thread(target=start_monitoring)
    monitoring_thread.start()
    
    alerta_manager = AlertaManager()
    # Aqui você pode adicionar chamadas para métodos do alerta_manager conforme necessário
    monitoring_thread.join()
       