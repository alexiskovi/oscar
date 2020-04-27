import cv2
import numpy as np

def collect_data(path, photos_n = 50):
    """
    Сбор датасета. Создание окна с отображением кадров, запись раскадровки в заданную директорию
    :path: путь к записи датасета
    :photos_n: количество кадров для калибровки
    """
    k = 0; t = 0; threshold = 30;
    cam = cv2.VideoCapture(1)
    while k<=photos_n:
        t+=1
        ret, frame = cam.read()
        frame_win = np.copy(frame)
        frame_win = cv2.resize(frame_win, (960, 540), interpolation=cv2.INTER_NEAREST)
        cv2.putText(frame_win, str(k)+' '+str(t//10), (200, 200), cv2.FONT_HERSHEY_SIMPLEX, 2, (0, 255, 0), thickness = 3)
        cv2.imshow("frame", frame_win)
        if t == threshold:
            cv2.imwrite(path+"/calibration{}.jpg".format(str(k)), frame)
            t = 0; k = k + 1
        if cv2.waitKey(1) & 0xFF == ord('q'):
            break
    cam.release()
    cv2.destroyAllWindows()
