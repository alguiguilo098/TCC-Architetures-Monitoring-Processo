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
KAFKA_TOPIC = 'netflow-topic'
ELASTIC_HOSTS = ['https://localhost:9200']
ELASTIC_USER = 'elastic'
ELASTIC_PASSWORD = 'LM5v1tZ0wLT7WvULS4DB'
ELASTIC_INDEX = 'netflow-logs'

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
        group_id='netflow-consumer-group',
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
                    processed_data = message.value
                    processed_data['@timestamp'] = datetime.utcnow().isoformat()

                    es.index(
                        index=ELASTIC_INDEX,
                        document=processed_data
                    )
                    logging.info(f"Dados Enviado: {processed_data}")
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