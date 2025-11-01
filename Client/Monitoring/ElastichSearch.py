from concurrent.futures import ThreadPoolExecutor, as_completed
import yaml
from elasticsearch import Elasticsearch
from Client.Monitoring.TinyDBManages import TinyDBManages
from concurrent.futures import ThreadPoolExecutor, as_completed
from Client.Monitoring.LogManager import LogManager

class ElasticsearchClient:
    def get_config(self,config_path_env:str):
        with open(config_path_env, 'r') as file:
            config = yaml.safe_load(file)
            return config
        
    def __init__(self, host='localhost', port=9200, path='Client/env.yaml'):
        # Conecta ao Elasticsearch sem segurança (modo dev)
        self.es = Elasticsearch([{'host': host, 'port': port, "scheme": "http"}])
        metric= self.get_config(path)
        path = metric.get("export_database_name")
        self.__log_manager= LogManager(log_file='elasticsearch_client.log')
        self.__db = TinyDBManages(db_path=path, table_name="MonitoringDataNew")
        
    def send_data(self, index_name, data):
        print(f"Enviando dado para o índice {index_name}: {data}")
        response = self.es.index(index=index_name, document=data)
        return response
    

    def all_send_data(self,index_name, max_workers=10, chunk_size=50):
        process_data = self.__db.get_all()
        self.__log_manager.log_info(f"Iniciando envio de {len(process_data)} registros para o Elasticsearch.")
        
    # Função que envia um lote de dados
        def send_chunk(chunk):
            for data in chunk:
                data.pop('doc_id', None)
                try:
                    self.send_data(index_name=index_name, data=data)
                except Exception as e:
                    print(f"❌ Erro ao enviar dado: {e}")
    
    # Dividir dados em chunks
        chunks = [process_data[i:i + chunk_size] for i in range(0, len(process_data), chunk_size)]
    
        with ThreadPoolExecutor(max_workers=max_workers) as executor:
            futures = [executor.submit(send_chunk, chunk) for chunk in chunks]
        
            for future in as_completed(futures):
                try:
                    future.result()
                except Exception as e:
                    print(f"❌ Erro em thread: {e}")
        
        self.__db.delete_all_bank()
    def is_connected(self):
        return self.es.ping()

