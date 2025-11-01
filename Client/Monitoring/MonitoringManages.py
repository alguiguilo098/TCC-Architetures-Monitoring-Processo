
import os
import psutil
import yaml
from datetime import datetime
import time
import concurrent.futures
import threading
from Client.Monitoring.LogManager import LogManager
from Client.Monitoring.TinyDBManages import TinyDBManages

class MonitoringManages:
    
    def __init__(self,config_path_env:str):
        
        """
            initialize MonitoringManages class
         
        Args:
            config_path_env (str): path to ymal config file
            intervargetmetric (float): _interval to get metrics
            interval_send_data (float): _interval to send data to server
            
        """
        
        metrics=self.__get_metrics_ymal(config_path_env)
        self.__path_db=metrics.get("database_name")
        self.__path_db_new=metrics.get("export_database_name")
        self.__threads_using=metrics.get("threads_using")
        self.__interval = metrics.get("check_interval")
        self.__register_migrate=metrics.get("register_migrate")
        self.__path_db=metrics.get("database_name")
        self.__path_db_new=metrics.get("export_database_name") 
        self.__db=TinyDBManages(db_path=self.__path_db,table_name="MonitoringData")
        self.__db_new=TinyDBManages(db_path=self.__path_db_new,table_name="MonitoringDataNew")
        
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
            data_process['name']=p.name()
            data_process["user"]=p.username()
            data_process['timestamp']=datetime.now().isoformat()
            data_process['status']=p.status()
            data_process['create_time']=p.create_time()
            data_process["num_threads"]=p.num_threads()
            data_process['cpu_percent']=p.cpu_percent(interval=0.1)
            data_process["num_child_processes"]=len(p.children())
            data_process["memory"]=p.memory_percent()
            io_counters=p.io_counters()
            data_process['read_bytes']=io_counters.read_bytes
            data_process['write_bytes']=io_counters.write_bytes
            data_process["num_fds"]=p.num_fds() if hasattr(p,'num_fds') else None
            data_process["nice"]=p.nice()
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
    
    def run(self, stop=3, semaphore: threading.Semaphore = None, lock: threading.Lock = None):
        count = 0
        manager = LogManager(log_file="monitoring_manager.log")
        log_manager = LogManager(log_file="database_manager.log")

        while True:
            try:
            # 🔹 BLOQUEIO OPCIONAL: evita que outro processo migre ou insira ao mesmo tempo
                if lock:
                    lock.acquire()

                total_records = len(self.__db.get_all())

            # 🔹 Coleta normal de métricas
                if self.__register_migrate > total_records:
                    data_list = self.get_all_metrics_monitoring(['cpu_percent', 'io_counters'])
                    manager.log_info(f"Collected {len(data_list)} metrics.")
                    self.__db.insert_multiples(data_list)
                    log_manager.log_info(f"Inserted {len(data_list)} metrics into the database.")
                    time.sleep(self.__interval)

            # 🔹 Inicia migração segura
                else:
                    log_manager.log_info("Starting database migration.")
                    migrated_count = self.__db.migrate_to_new_db_data(target_db=self.__db_new)
                    log_manager.log_info(f"Database migration completed. Migrated {migrated_count} records.")

                # Verifica consistência antes de apagar
                    remaining = len(self.__db.get_all())
                    if remaining == 0 or remaining == migrated_count:
                        self.__db.delete_all_bank()
                        log_manager.log_info("Source database cleared after migration.")
                    else:
                        log_manager.log_warning(
                        f"Migration mismatch: migrated={migrated_count}, remaining={remaining}. "
                        f"Skipping delete to prevent data loss."
                        )

                # 🔹 Controle de semáforo (coordenação com outros gerenciadores)
                    if count > 0 and count % 2 == 0:
                        print("Releasing semaphore for MonitoringManager")
                        if semaphore is not None:
                            print("Releasing semaphore for AlertManager")
                            semaphore.release()

                    print("Dormindo Fora:" + str(count))
                    count += 1

            # 🔹 Fim do ciclo
                time.sleep(self.__interval)

            except Exception as e:
                manager.log_error(f"Error in MonitoringManager.run: {str(e)}")
                time.sleep(self.__interval)

            finally:
            # 🔹 Libera lock se estiver ativo
                if lock and lock.locked():
                    lock.release()


                
            
            
