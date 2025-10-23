import yaml
import Client.DBmanages.TinyDBManages as tdbm

class AlertaManager:
    
    def get_paramters_cofig(self,path):
        with open(path,'r') as file:
            config = yaml.safe_load(file)
            return config
        
    def __init__(self):
        self.__alertmanager=self.get_paramters_cofig("Client/env.yaml")
        self.__winsize=self.__alertmanager.get("window_size")
        self.__metrics_threshold_alarm=self.__alertmanager.get("metrics_threshold_alarm")
        self.__metrics=self.__alertmanager.get("metrics")
        
    def checkprocessanomaly(self,pid:int):
        records=self.__tables.query_n_register_by_pid(pid=pid,begin=0,end=self.__winsize)
        print(records)
        
        
        
    
    def all_process_anomaly(self):
        pass
        
    
    def send_alert(self, message):
        print(f"Alert sent: {message}")