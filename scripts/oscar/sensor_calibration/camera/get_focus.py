# -*- coding: utf-8 -*-

import cv2
import numpy as np
import time

def main():
    c = 5.784
    #Чтение параметров из файла
    try:
        par_file = open('focus.txt', 'r')
    except:
        raise BlockingIOError('Конфигурационный файл focus.txt утерян')
    l = []
    for line in par_file:
        for t in line.split():
            try:
                l.append(float(t))
            except ValueError:
                pass
    d, real_w = l
    par_file.close()
    print('Расстояние до объекта: {}'.format(str(d)))
    print('Ширина референсного объекта: {}'.format(str(real_w)))
    time.sleep(1)
    try:
        vid = cv2.VideoCapture(0)
    except:
        raise Exception('Невозможно получить данные от камеры')
    rtv, frame = vid.read()
    while not(rtv):
        print('Кадр не получен!')
        rtv, frame = vid.read()
    hc = frame.shape[0]//2
    vid.set(3, 1920)
    vid.set(4, 1080) 
    while True:
        rtv, frame = vid.read()
        wc = frame.shape[1]//2
        canny = cv2.Canny(frame, 100, 100)
        #Нахождение крайних точек
        while canny[hc,wc]==0 and canny[hc-1,wc]==0 and canny[hc+1,wc]==0:
            wc+=1
            if wc == frame.shape[1]-1:
                break
        cv2.circle(canny, (wc, hc), 7, 255, -1)
        p=wc
        wc = canny.shape[1]//2
        while canny[hc,wc]==0 and canny[hc-1,wc]==0 and canny[hc+1,wc]==0:
            wc-=1
            if wc == 0:
                break
        cv2.circle(canny, (wc, hc), 7, 255, -1)
        p-=wc
        canny = cv2.resize(canny, (960, 540), interpolation=cv2.INTER_NEAREST)
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
        #Расчет фокусного расстояния
        if p<frame.shape[1]-10 and p>10:
            f = c*d/(frame.shape[1]*real_w/p)
            cv2.putText(canny, '%4.2f' % f, (100, 100), cv2.FONT_HERSHEY_SIMPLEX, 2, 255, thickness = 3)

        cv2.imshow('canny', canny)
        frame = cv2.resize(frame, (960, 540), interpolation=cv2.INTER_NEAREST)
        cv2.imshow('frame', frame)
    vid.release()
    cv2.destroyAllWindows()

if __name__ == "__main__":
    try:
        main()
    except Exception as e:
        logging.error(e)
