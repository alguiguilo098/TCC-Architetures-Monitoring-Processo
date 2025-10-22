
import os
import psutil
import yaml
from datetime import datetime
import time
import concurrent.futures
from src.DBmanages.TinyDBManages import TinyDBManages
class MonitoringManages:
    
    def __init__(self,config_path_env:str,path_db:str):
        
        """
            initialize MonitoringManages class
         
        Args:
            config_path_env (str): path to ymal config file
            intervargetmetric (float): _interval to get metrics
            interval_send_data (float): _interval to send data to server
            
        """
        
        metrics=self.__get_metrics_ymal(config_path_env)
        self.__threads_using=metrics.get("threads_using",)
        self.__interval = metrics.get("check_interval",10) 
        self.__db=TinyDBManages(db_path=path_db,table_name="MonitoringData")
        
    def __get_metrics_ymal(self, config_path_env:str):
        
        """
        get parameters from ymal config file

        Args:
            config_path_env (str): _path to ymal config file
        Returns:
            list: list parameters from ymal file
        """
        
        with open(config_path_env,'r') as file:
            config = yaml.safe_load(file)
            return config
        
    def __get_one_metrics_monitoring(self,pid:int,list_metrics_process:list):
        """
        get one process metrics monitoring

        Args:
            pid (int): _process id
            list_metrics_process (dict): _list of metrics to get for process

        Returns:
            dict: _metrics process monitoring
        """
        data_process={}
        try:
            p = psutil.Process(pid)
            data_process['pid']=pid
            data_process["uid"]=p.uids().real
            data_process["gid"]=p.gids().real
            data_process['name']=p.name()
            data_process["user"]=p.username()
            data_process['timestamp']=datetime.now().isoformat()
            data_process['status']=p.status()
            data_process['create_time']=p.create_time()
            data_process["num_threads"]=p.num_threads()
            data_process["exe"]=p.exe()
            data_process["cmdline"]=p.cmdline()
            data_process['cpu_percent']=p.cpu_percent(interval=0.1)
            mem_info=p.memory_info()
            data_process['memory_rss']=mem_info.rss
            data_process['memory_vms']=mem_info.vms
            io_counters=p.io_counters()
            data_process['read_bytes']=io_counters.read_bytes
            data_process['write_bytes']=io_counters.write_bytes
        except (psutil.NoSuchProcess, psutil.AccessDenied, psutil.ZombieProcess):
            pass
        return data_process
    
    def __get_all_metrics_process(self,pidlist,list_metrics_process:list):
        """
        get all process metrics monitoring

        Args:
            pidlist (list): _list of pids
            list_metrics_process (dict): _list of metrics to get for process

        Returns:
            list: _list of metrics process monitoring
        """
        features=[]
        for pid in pidlist:
            features.append(self.__get_one_metrics_monitoring(pid,list_metrics_process))
        return features
    
    def __div_pids_in_threads(self,list_pids:list,threads_process_div:int):
        """
        divide pids in threads

        Args:
            list_pids (list): _list of pids
            threads_process_div (int): _number of threads to use

        Returns:
            list: _list of pids divided in threads
        """
        div_pids=[]
        for i in range(0,len(list_pids),threads_process_div):
            div_pids.append(list_pids[i:i+threads_process_div])
        return div_pids


    def get_all_metrics_monitoring(self, list_metrics_process: list):
        list_pids = psutil.pids()
        if not list_pids:
            return []

        max_workers = min(self.__threads_using, len(list_pids))
        threads_process_div = max(1, len(list_pids) // max_workers)
        div_pids = self.__div_pids_in_threads(list_pids, threads_process_div)

        results = []
        with concurrent.futures.ThreadPoolExecutor(max_workers=max_workers) as executor:
            futures = [executor.submit(self.__get_all_metrics_process, pid_chunk, list_metrics_process)
                   for pid_chunk in div_pids]

        for fut in concurrent.futures.as_completed(futures):
            try:
                res = fut.result()
                if res:
                    results.extend(res)  # <- use extend se res for lista
            except Exception as e:
                print(f"Erro ao coletar métricas de um chunk: {e}")

        return results


    
    def run(self):
        while True:
            data_list=self.get_all_metrics_monitoring(['cpu_percent','io_counters'])
            self.__db.insert_multiples(data_list)
            time.sleep(self.__interval)
            
if __name__ =="__main__":
    monitor=MonitoringManages(config_path_env="src/env.yaml",intervargetmetric=10,interval_send_data=30)
    data_list=monitor.get_all_metrics_monitoring(['cpu_percent','io_counters'])
    