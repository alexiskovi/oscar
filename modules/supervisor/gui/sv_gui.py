#!/usr/bin/env python3
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
import configparser

config = configparser.ConfigParser()
config.read('/apollo/modules/supervisor/gui/config.ini', encoding='utf-8')
host_ip=config.get('server', 'host_ip')
tornado_port=config.get('server', 'port')
server_started = {'state': True, 'tornado_thread': None, 'check_status_thread': None}
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
    # server_state[name]['text_field'] = '<br>'.join('{}{}'.format(
    #     key, value) for key, value in status.items())

    server_state[name]['text_field'] = '<tr>'+'</tr><tr>'.join('<td>{}</td><td>{}</td>'.format(
        key, value) for key, value in status.items())+'</tr>'
    # server_state[name]['tf1']='<br>'.join(status.keys())
    # server_state[name]['tf2']='<br>'.join(status.values())
    #print("tf2",server_state[name]['tf2'])
    #print('VALUES:','<br>'.join(status.values()))


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
    dict={}
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
        if not route_name:
            self.render("templates/main_screen.html", host_ip=host_ip, port=tornado_port)
        elif 'info' in route_name:
            route_name = route_name.replace("info_", "")
            self.render("templates/full_info.html", title=route_name, host_ip=host_ip,
             port=tornado_port, current_state=server_state.get(route_name).get('status'))


class WebSocketHandler(tornado.websocket.WebSocketHandler):
    def open(self, *args):
        print("New connection")

    def on_message(self, message):
        if message == 'get_server_state':
            ans = get_status_words()
            ans['sound_on']=server_global_preferences['sound_on']
            ans['debug_mode']=server_global_preferences['debug_mode']
            self.write_message(ans)
        elif 'get_server_state' in message:
            message = message.replace('get_server_state_', "")
            ans=server_state.get(message)
            ans={**ans, **get_status_words()}
            self.write_message(ans)
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
        elif message=='stop server':
            close_app(server_started['tornado_thread'], server_started['check_status_thread'])

    def on_close(self):
        print("Connection closed")


def tornado_start():
    asyncio.set_event_loop(asyncio.new_event_loop())
    define("port", default=int(tornado_port), type=int)
    app = tornado.web.Application([
        (r'/(info_\w+)*', IndexHandler),
        (r'/ws/', WebSocketHandler),
    ])
    app.listen(options.port)
    server_started['tornado_instance'] = tornado.ioloop.IOLoop.instance()
    server_started['tornado_instance'].start()


def close_app(thread1, thread2):
    thread1.stop()
    thread2.stop()
    server_started['state'] = False
    supervisor.stop()
    print('App closed')


if __name__ == '__main__':
    server_started['tornado_thread'] = StoppableThread(target=tornado_start)
    server_started['tornado_thread'].start()
    server_started['check_status_thread'] = StoppableThread(
        target=status_check, delete_link=True)
    server_started['check_status_thread'].start()
