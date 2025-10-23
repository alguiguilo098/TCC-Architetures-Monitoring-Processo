from tinydb import TinyDB, Query, where

class TinyDBManages:
    """
    A class to manage TinyDB operations including insert, delete,
    migrate, and query.
    """

    def __init__(self, db_path: str, table_name: str):
        """
        Initialize TinyDBManages class.

        Args:
            db_path (str): Path to database file.
            table_name (str): Table name inside TinyDB.
        """
       
        self.__db_path = db_path
        self.__table_name = table_name
        self.__db = TinyDB(self.__db_path)
        self.__table = self.__db.table(self.__table_name)

    def insert_multiples(self, data: list):
        if not all(isinstance(item, dict) for item in data):
            raise ValueError("All items in the data list must be dictionaries.")
        return self.__table.insert_multiple(data)


    def delete_all_bank(self):
        """
        Delete all records from the table.
        """
        self.__table.truncate()
        print(f"All data deleted from table '{self.__table_name}'.")

    def migrate_to_new_db_data(self, target_db):
        records = self.__table.all()

    # Remove o doc_id para evitar conflito
        cleaned_records = [
            {k: v for k, v in r.items() if k != 'doc_id'} for r in records
        ]

        target_db.__table.truncate()  # opcional, se quiser começar do zero
        target_db.__table.insert_multiple(cleaned_records)

    def query_n_register_by_pid(self, pid: int, begin: int = 5, end: int = 10):
        """
        Consulta registros com um 'pid' específico no banco de dados dentro de um intervalo determinado.

        Args:
            pid (int): O valor de 'pid' a ser filtrado.
            begin (int, opcional): Índice inicial do intervalo (padrão: 5).
            end (int, opcional): Índice final do intervalo (padrão: 10).

    Returns:
        dict: Um dicionário contendo:
            - 'records': Lista dos registros filtrados no intervalo.
            - 'begin': Índice inicial.
            - 'end': Índice final.
            - 'total': Quantidade total de registros encontrados.
    """
        # Verificar se o 'pid' é válido
        if not isinstance(pid, int):
            raise TypeError("O parâmetro 'pid' deve ser um inteiro.")
        q=Query()
        response = self.__table.search(q.pid == pid)

        total = len(response)
        print(f"Registros encontrados para pid {pid}: {total}")

        # Aplicar o fatiamento do intervalo solicitado
        sliced_records = response[begin:end]

        return {
        "records": sliced_records,
        "begin": begin,
        "end": end,
        "total": total
    }

    
    def get_all(self):
        """
        Retrieve all records in the table.
        """
        
        return self.__table.all()
