import threading
import time
from supervisor_api import SupervisorPreferences
import tornado.ioloop
import tornado.web
import tornado.websocket
from tornado.escape import json_encode
from tornado.options import define, options, parse_command_line

define("port", default=8888, type=int)
server_state={
'canbus':'-',
'control':'-',
'perception':'-',
'gnss':'-',
'localization':'-',
'planning':'-'
}

def get_status_dict(supervisor):
    server_state['gnss']=supervisor.get_gnss_status_word()

def status_check():
    supervisor = SupervisorPreferences()
    while True:
        try:
            get_status_dict(supervisor)
            time.sleep(0.3)
        except KeyboardInterrupt:
            print('break')
            break

class IndexHandler(tornado.web.RequestHandler):
    def get(self, route_name):
        self.render("templates/index.html")


class WebSocketHandler(tornado.websocket.WebSocketHandler):
    def open(self, *args):
        print("New connection")
        #self.write_message("Welcome!")

    def on_message(self, message):
        print("New message {}".format(message))
        if message=='get_server_state':
            self.write_message(server_state)

    def on_close(self):
        print("Connection closed")


app = tornado.web.Application([
    (r'/', IndexHandler),
    (r'/ws/', WebSocketHandler),
])



if __name__ == '__main__':
    threading.Thread(target=status_check).start()
    app.listen(options.port)
    tornado.ioloop.IOLoop.instance().start()
    #threading.Thread(target=tornado.ioloop.IOLoop.instance).start()
    print('HERE')
