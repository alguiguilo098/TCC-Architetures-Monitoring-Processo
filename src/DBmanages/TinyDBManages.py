from tinydb import TinyDB, Query

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

    def migrate_data_to_db_fail_send(self, target_db: 'TinyDBManages'):
        """
        Migrate all records from this database to another TinyDBManages instance.

        Args:
            target_db (TinyDBManages): Target database manager.
        """
        records = self.__table.all()
        if not records:
            print("No records to migrate.")
            return

        target_db.__table.insert_multiple(records)
        print(f"Migrated {len(records)} records from '{self.__table_name}' to '{target_db.__table_name}'.")

    def query(self, query_params: dict):
        """
        Query records from the table based on key-value pairs.

        Args:
            query_params (dict): Dictionary with field-value pairs to filter.

        Returns:
            list: List of matching records.
        """
        if not isinstance(query_params, dict):
            raise ValueError("Query params must be a dictionary.")

        q = Query()
        results = self.__table.search(
            eval(" & ".join([f"(q['{k}'] == v)" for k, v in query_params.items()]))
        )
        print(f"Querying TinyDB with params: {query_params}")
        return results

    def get_all(self):
        """
        Retrieve all records in the table.
        """
        return self.__table.all()
