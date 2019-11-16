from simple_websocket_server import WebSocketServer, WebSocket
import db
from threading import Thread

class WebSocketManager(WebSocket):
    def __init__(self, server, sock, address):
        super().__init__(server, sock, address)
        ring = None
    def handle(self):
        # echo message back to client
        print("Received: " + self.data)
        # if self.data.split(",")[1] == "null":
        #     i.remove_object_location(self.data.split(" ")[0])
        # elif (self.data.split(",")[1] != None):
        #     i.insertobjectlocation(self.data.split(",")[0], self.data.split(",")[1])
        # self.send_message(self.data)

    def connected(self):
        print(self.address, 'connected')

    def handle_close(self):
        print(self.address, 'closed')

    def audio_on(self):
        if self.ring is not None:
            pass

class WebSocketManagerBuzzer(WebSocket):
    def __init__(self, server, sock, address):
        super().__init__(server, sock, address)
        ring = None
    def handle(self):
        # echo message back to client
        print("Received: " + self.data)

       # if self.data.split(",")[1] == "null":
       #      i.remove_object_location(self.data.split(" ")[0])
       #  elif (self.data.split(",")[1] != None):
       #      i.insertobjectlocation(self.data.split(",")[0], self.data.split(",")[1])
       #  # self.send_message(self.data)

    def connected(self):
        self.send_message("hello world")
        print(self.address, 'connected')

    def handle_close(self):
        print(self.address, 'closed')

    def audio_on(self):
        if self.ring is not None:
            pass

def main():

    server_tool = WebSocketServer('192.168.43.238', 8000, WebSocketManagerBuzzer)
    thread_tool = Thread(target=server_tool.serve_forever)

    server_buzzer = WebSocketServer('192.168.43.238', 8001, WebSocketManagerBuzzer)
    thread_buzzer = Thread(target=server_buzzer.serve_forever)
    input()
    thread_tool.start()
    thread_buzzer.start()


i = db.Interface("tools", "postgres", "ivan127")
i.connect()
if __name__ == "__main__":
    main()
