#!/usr/bin/env python3
import asyncio
import time
import websockets
import configparser

config = configparser.ConfigParser()
config.read('/apollo/modules/supervisor/gui/config.ini', encoding='utf-8')
host_ip = config.get('server', 'host_ip')
tornado_port = config.get('server', 'port')


async def main():
    async with websockets.connect('ws://'+host_ip+':'+str(tornado_port)+'/ws/') as websocket:
        try:
            await websocket.send("stop server")
        except Exception as e:
            print(e)


asyncio.get_event_loop().run_until_complete(main())
