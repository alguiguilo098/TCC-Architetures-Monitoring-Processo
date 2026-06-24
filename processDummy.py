from kafka import KafkaConsumer
from elasticsearch import Elasticsearch
import logging
import time
from datetime import datetime
import json

logging.basicConfig(
    level=logging.INFO,
    format='%(asctime)s - %(levelname)s - %(message)s'
)

KAFKA_BOOTSTRAP_SERVERS = 'localhost:9092'
KAFKA_TOPIC = 'monitoring-topic'
ELASTIC_HOSTS = ['https://localhost:9200']
ELASTIC_USER = 'elastic'
ELASTIC_PASSWORD = 'LM5v1tZ0wLT7WvULS4DB'
ELASTIC_INDEX = 'monitoring-logs'
ELASTIC_INDEX1= 'url-logs'
ELASTIC_INDEX2= 'programs-logs'
ELASTIC_INDEX3= 'kernel-logs'

def connect_elasticsearch():
    return Elasticsearch(
        hosts=ELASTIC_HOSTS,
        basic_auth=(ELASTIC_USER, ELASTIC_PASSWORD),
        verify_certs=False
    )

def create_kafka_consumer():
    return KafkaConsumer(
        KAFKA_TOPIC,
        bootstrap_servers=KAFKA_BOOTSTRAP_SERVERS,
        auto_offset_reset='latest',  
        group_id='monitoring-consumer-group',
        enable_auto_commit=True,      
        value_deserializer=lambda x: json.loads(x.decode('utf-8'))
    )

def main():
    es = connect_elasticsearch()

    while True:
        try:
            consumer = create_kafka_consumer()
            logging.info("Consumidor Kafka iniciado, aguardando mensagens...")
            
            for message in consumer:
                try:
                    if "processes" in message.value:
                        for process in message.value["processes"]:
                            logging.info(f"Mensagem recebida: {process}")
                            es.index(index=ELASTIC_INDEX, document=process)
                    elif "url" in message.value:
                        logging.info(f"Mensagem recebida: {message.value['url']}")
                        es.index(index=ELASTIC_INDEX1, document=message.value)
                        logging.info("Mensagem enviada ao Elasticsearch com sucesso.")
                    elif "programs" in message.value:
                        logging.info(f"Mensagem recebida: {message.value['programs']}")
                        es.index(index=ELASTIC_INDEX2, document=message.value)
                        logging.info("Mensagem enviada ao Elasticsearch com sucesso.")
                    elif "kernel" in message.value:
                        logging.info(f"Mensagem recebida: {message.value['kernel']}")
                        es.index(index=ELASTIC_INDEX3, document=message.value)
                        logging.info("Mensagem enviada ao Elasticsearch com sucesso.")
                    else:
                        logging.info(f"Mensagem recebida com formato inesperado: {message.value}")
                        es.index(index=ELASTIC_INDEX, document=message.value)
                except Exception as e:
                    logging.error(f"Erro ao enviar mensagem ao Elasticsearch: {e}")

        except Exception as e:
            logging.error(f"Erro ao consumir Kafka: {e}. Reconectando em 10s")
            time.sleep(10)

        finally:
            if 'consumer' in locals():
                consumer.close()

if __name__ == "__main__":
    main()