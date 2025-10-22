import src.Monitoring.MonitoringManages as mm
import src.DBmanages.TinyDBManages as tdbm
if __name__ =="__main__":
    try:
        monitor=mm.MonitoringManages(config_path_env="src/env.yaml",
                                 path_db="./monitoring_db.json")
        monitor.run()
    except KeyboardInterrupt:
        print("Monitoring stopped by user.")
        db=tdbm.TinyDBManages(db_path="./monitoring_db.json",
                                      table_name="MonitoringData")
        print(len(db.get_all()))
        