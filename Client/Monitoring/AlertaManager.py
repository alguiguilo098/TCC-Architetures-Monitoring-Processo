import ast
import yaml
from TinyDBManages import TinyDBManages
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
                return True

        return False

        
    def send_alert(self, message):
        # Logic to send alert
        print(f"Alert sent: {message}")
        
if __name__=="__main__":
    alerta_manager = AlertaManager()
    pids = alerta_manager._AlertaManager__db.get_all_pids()
    for pid in pids:
        alerta_manager.check_anomaly(pid=pid, times=3)