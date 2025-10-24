import threading
from Client.Monitoring.MonitoringManages import MonitoringManages

from Client.Monitoring.TinyDBManages import TinyDBManages


semaphore_agent = threading.Semaphore()
semaphore_alerta = threading.Semaphore()

def start_monitoring():
    monitor = MonitoringManages(config_path_env="Client/env.yaml")
    monitor.run(sempahore=semaphore_agent)

def start_alerta_manager():
    alerta_manager = AlertaManager()
    # Here you can add code to run alerta_manager tasks
    # For example, checking for anomalies periodically
    # This is just a placeholder for demonstration
    pass

if __name__ == "__main__":
    thread_monitoring = threading.Thread(target=start_monitoring)  
    thread_monitoring.start()
    thread_monitoring.join()
    
       