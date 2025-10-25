import ast
import yaml
from Client.Monitoring.TinyDBManages import TinyDBManages
from threading import Semaphore

import concurrent.futures
class AlertaManager:
    def __load_config(self, config_path_env:str):
        with open(config_path_env,'r') as file:
            config = yaml.safe_load(file)
            return config
    def __init__(self):
        self.__config=self.__load_config("Client/env.yaml")
        self.__metrics_threshold_alarm=self.__config.get("metrics_threshold_alarm")
        self.__winsize=self.__config.get("winsize")
        self.__db=TinyDBManages(db_path=self.__config.get("export_database_name"),table_name="MonitoringDataNew")
        self.__metrics=self.__config.get("metrics")
        

    def check_anomaly(self, pid: int, times: int):
        """
        Verifica se um processo ultrapassou limites de métricas em uma ou mais janelas.
        """
        winsize = self.__winsize
        max_violations = self.__metrics_threshold_alarm  # define o limite de violações para acionar o alerta

        for i in range(times):
        # Busca os registros da janela
            records = self.__db.query_n_register_by_pid(
                pid=pid, begin=i * winsize, end=(i + 1) * winsize
            )

        # Caso o retorno seja string (TinyDB pode salvar como texto)
            if isinstance(records, str):
                try:
                    records = ast.literal_eval(records)
                except Exception:
                    continue

            #Se for um dicionário que contém "records", acessa a chave
            if isinstance(records, dict) and "records" in records:
                records = records["records"]

            if not isinstance(records, list):
                continue

            violations = 0
            for record in records:
            # Se o registro vier em formato string, converte
                if isinstance(record, str):
                    try:
                        record = ast.literal_eval(record)
                    except Exception:
                        continue

                for metric, limit in self.__metrics.items():
                    value = record.get(metric)
                    if value is not None and value > limit:
                        violations += 1

            if violations >= max_violations:
                msg = f"[ALERTA] Processo {pid} ultrapassou limite em {violations} métricas (janela {i})"
                self.send_alert(msg)
                return pid

        return -1

    def run(self, semaphore, max_workers: int = 5) -> None:
        """
    Run anomaly detection on all PIDs using a thread pool.

    Args:
        semaphore: Semaphore for synchronization
        max_workers: Maximum number of concurrent threads
    """
        try:
            pids = self.__db.get_all_pids()
            if not pids:
                print("No PIDs found in database")
                return

            def task(pid):
                semaphore.acquire()
                try:
                    return self.check_anomaly(pid, times=3)
                finally:
                    semaphore.release()

            with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
                futures = [executor.submit(task, pid) for pid in pids]
                for future, pid in zip(concurrent.futures.as_completed(futures), pids):
                    try:
                        result = future.result()
                        if result:
                            continue
                    except Exception as e:
                        print(f"Error checking anomaly for PID {pid}: {str(e)}")

        except Exception as e:
            print(f"Error in run method: {str(e)}")

    def send_alert(self, message):
        # Logic to send alert
        print(f"Alert sent: {message}")
        
if __name__=="__main__":
    alerta_manager=AlertaManager()
    alerta_manager.run(semaphore=Semaphore())