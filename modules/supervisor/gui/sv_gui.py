import threading
import time
from supervisor_api import SupervisorPreferences
import tornado.ioloop
import tornado.web
import tornado.websocket
from tornado.escape import json_encode
from tornado.options import define, options, parse_command_line
import sys
import asyncio

server_started = {'state': True}
supervisor = SupervisorPreferences()
server_global_preferences={'sound_on':True, 'debug_mode': True}
server_state = {
    'canbus': {
        'status': '-',
        'sound_on': True,
        'debug_mode': True,
        'text_field': '-'},

    'control': {
        'status': '-',
        'sound_on': True,
        'debug_mode': True,
        'text_field': '-'},

    'perception': {
        'status': '-',
        'sound_on': True,
        'debug_mode': True,
        'text_field': '-'},

    'gnss': {
        'status': '-',
        'sound_on': True,
        'debug_mode': True,
        'text_field': '-'},

    'localization': {
        'status': '-',
        'sound_on': True,
        'debug_mode': True,
        'text_field': '-'},

    'planning': {
        'status': '-',
        'sound_on': True,
        'debug_mode': True,
        'text_field': '-'},

    'imu': {
        'status': '-',
        'sound_on': True,
        'debug_mode': True,
        'text_field': '-'}
}

def make_text_field_text(status, name):
    server_state[name]['text_field'] = '<br>'.join('{}{}'.format(
        key, value) for key, value in status.items())


def get_status_dict(supervisor):
    modules=['canbus', 'control', 'perception', 'gnss', 'localization', 'planning', 'imu']
    server_global_preferences.update(supervisor.get_sv_parameters())
    for module in modules:
        server_state[module]['status'] = getattr(supervisor, 'get_'+module+'_status_word')()
        make_text_field_text(getattr(supervisor, 'get_'+module+'_status')(), module)
        server_state[module].update(getattr(supervisor, 'get_'+module+'_parameters')())


def status_check():
    i = 0
    while server_started['state']:
        try:
            get_status_dict(supervisor)
            time.sleep(0.3)
        except Exception as e:
            print('Error:', e)
    print('status_check exiting')

def get_status_words():
    modules=['canbus', 'control', 'perception', 'gnss', 'localization', 'planning', 'imu']
    dict={'sound_on': server_global_preferences['sound_on'],
          'debug_mode': server_global_preferences['debug_mode']}
    for i in modules:
        dict[i]=server_state[i]['status']
    return dict


class StoppableThread(threading.Thread):

    def __init__(self, delete_link=False,  *args, **kwargs):
        super(StoppableThread, self).__init__(*args, **kwargs)
        self._stop_event = threading.Event()
        self.delete_link = delete_link

    def stop(self):
        if self.delete_link:
            server_started['tornado_instance'] = None
        self._stop_event.set()

    def stopped(self):
        return self._stop_event.is_set()


class IndexHandler(tornado.web.RequestHandler):
    def get(self, route_name):
        print(route_name)
        if not route_name:
            self.render("templates/index.html")
        elif 'info' in route_name:
            route_name = route_name.replace("info_", "")
            self.render("templates/index0.html", title=route_name)


class WebSocketHandler(tornado.websocket.WebSocketHandler):
    def open(self, *args):
        print("New connection")
        # self.write_message("Welcome!")

    def on_message(self, message):
        #print("New message {}".format(message))
        if message == 'get_server_state':
            print(server_global_preferences)
            self.write_message(get_status_words())
        elif 'get_server_state' in message:
            message = message.replace('get_server_state_', "")
            self.write_message(server_state.get(message))
        elif message=='sound_on_off':
            supervisor.define_sv_sound_state(not server_global_preferences['sound_on'])
        elif 'sound_on_off' in message:
            cur_module = message.replace('sound_on_off_', "")
            getattr(supervisor, 'define_'+cur_module+'_sound_state')(not server_state[cur_module]['sound_on'])
        elif message == 'change_debug_mode':
            supervisor.define_sv_debug_state(not server_global_preferences['debug_mode'])
        elif 'change_debug_mode' in message:
            cur_module = message.replace('change_debug_mode_', "")
            getattr(supervisor, 'define_'+cur_module+'_debug_state')(not server_state[cur_module]['debug_mode'])
        elif message=='save_config':
            supervisor.save_current_parameters()

    def on_close(self):
        print("Connection closed")


def tornado_start():
    asyncio.set_event_loop(asyncio.new_event_loop())
    define("port", default=8888, type=int)
    app = tornado.web.Application([
        (r'/(info_\w+)*', IndexHandler),
        (r'/ws/', WebSocketHandler),
    ])
    app.listen(options.port)
    # tornado_instance=
    server_started['tornado_instance'] = tornado.ioloop.IOLoop.instance()
    server_started['tornado_instance'].start()


def close_app(thread1, thread2):
    while True:
        if input() == 'Q':
            print('START stop')
            thread1.stop()
            thread2.stop()
            server_started['state'] = False
            print('thread1 stopped', thread1.stopped())
            print(server_started)
            break


if __name__ == '__main__':

    tornado_thread = threading.Thread(target=tornado_start)
    tornado_thread.start()
    # tornado.ioloop.IOLoop.instance().start()
    # input_thread=threading.Thread(target=read_keyboard_input, args=(tornado_instance, ))
    # input_thread.start()

    # print('Here1')
    status_check_thread = StoppableThread(
        target=status_check, delete_link=True)
    status_check_thread.start()
    print('Here2')
    #
    # time.sleep(2)
    # server_started['tornado_instance'].stop()
    # server_started['tornado_instance']=None
    # close_app_thread=threading.Thread(target=close_app, args=(tornado_thread, status_check_thread, ))
    # close_app_thread.start()
    # print('Here3')
    # # tornado_instance.start()
    # print('Started joining')
    # # input_thread.join()
    #
    # tornado_thread.join()
    # status_check_thread.join()
    # close_app_thread.join()
    print('Here3')
