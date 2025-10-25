import threading
from Client.Monitoring.MonitoringManages import MonitoringManages
from Client.Monitoring.AlertaManager import AlertaManager



semaphore_alerta = threading.Semaphore()

def start_monitoring():
    monitor = MonitoringManages(config_path_env="Client/env.yaml")
    monitor.run(semaphore=semaphore_alerta)

def start_alerta_manager():
    alert=AlertaManager()
    semaphore_alerta.acquire()
    alert.run(semaphore=semaphore_alerta)
    # Here you can add code to run alerta_manager tasks
    # For example, checking for anomalies periodically
    # This is just a placeholder for demonstration


if __name__ == "__main__":
    thread_monitoring = threading.Thread(target=start_monitoring)  
    thread_monitoring.start()
    
    thread_alerta = threading.Thread(target=start_alerta_manager)
    thread_alerta.start()
    
    thread_monitoring.join()
    thread_alerta.join()
       