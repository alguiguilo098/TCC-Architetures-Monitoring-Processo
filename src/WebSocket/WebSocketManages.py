
class WebSocketManages():
    
    def __init__(self):
        self.active_connections = []
    
    def connect(self, websocket):
        self.active_connections.append(websocket)
    
    def disconnect(self, websocket):
        self.active_connections.remove(websocket)
    
    def send_personal_message(self, message: str, websocket):
        pass
    
    def receive_personal_message(self, websocket):
        pass
    