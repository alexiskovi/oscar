import numpy as np
import cv2
import glob
import pickle
import os
import matplotlib.pyplot as plt
import time
import make_photos

def main():
    # Создание объектов координат (0,0,0), (1,0,0), (2,0,0) ....,(6,5,0)
    nx = 8
    ny = 6
    objp = np.zeros((ny*nx,3), np.float32)
    objp[:,:2] = np.mgrid[0:nx, 0:ny].T.reshape(-1,2)

    # Массивы хранения точек.
    objpoints = [] # 3d точки реального мира
    imgpoints = [] # 2d точки изображения

    calibrate_file = "camera_cal_out/calibrate_params3646.pickle"


    def fillfile(path, D, K, R, P):
        """
        Функция заполнения .yaml файла
        :path: путь, по которому запишется файл
        :D, K, R, P: соответствующие матрицы калибровки камеры
        """
        try:
            yf = open(path, 'w')
        except:
            raise BlockingIOError('Ошибка создания файла конфигурации')
        yf.write('header:\n')
        yf.write('  seq: 0\n')
        yf.write('  stamp:\n')
        yf.write('  secs: 0\n')
        yf.write('    nsecs: 0\n')
        yf.write('  frame_id: short_camera\n')
        yf.write('height: 1080\n')
        yf.write('width: 1920\n')
        yf.write('distortion_model: plumb_bob\n')
        yf.write('D: {}\n'.format([i for i in D.flat]))
        yf.write('K: {}\n'.format([i for i in K.flat]))
        yf.write('R: {}\n'.format([i for i in R.flat]))
        yf.write('P: {}\n'.format([i for i in P.flat]))
        yf.write('binning_x: 0\n')
        yf.write('binning_y: 0\n')
        yf.write('roi:\n')
        yf.write('  x_offset: 0\n')
        yf.write('  y_offset: 0\n')
        yf.write('  height: 0\n')
        yf.write('  width: 0\n')
        yf.write('  do_rectify: False')
        yf.close()


    def calibrate_camera(dataset_path,cam_num):
        """
        Процесс калибровки по пути к датасету. Возвращает матрицы калибровки
        :dataset_path: адрес директории, в которую записывается датасет
        :cam_num: номер/имя камеры
        """

        """
        # check if calibration file already exist
        if os.path.exists(calibrate_file):
            try:
                print("{0} file exist".format(calibrate_file))
                with open(calibrate_file, 'rb') as f:
                # The protocol version used is detected automatically, so we do not
                # have to specify it.
                    data = pickle.load(f)

                    return data["mtx"], data["dist"]
            except:
                print("Some error while load calibration file")
       """
        # Make a list of calibration images
        images = glob.glob(dataset_path+'/calibration*.jpg')

        # Step through the list and search for chessboard corners
        for idx, fname in enumerate(images):
            print("Process calibration step {0}: {1}".format(idx,fname))
            img = cv2.imread(fname)
            gray = cv2.cvtColor(img, cv2.COLOR_BGR2GRAY)

            # Find the chessboard corners
            try:
                ret, corners = cv2.findChessboardCorners(gray, (nx,ny), None)
            except:
                raise Exception('Ошибка поиска углов доски')

            # If found, add object points, image points
            if ret == True:
                objpoints.append(objp)
                imgpoints.append(corners)

        cv2.destroyAllWindows()

        img_size = (img.shape[1], img.shape[0])

        # Do camera calibration given object points and image points
        ret, mtx, dist, rvecs, tvecs = cv2.calibrateCamera(objpoints, imgpoints, img_size,None,None)

        #Calculating R and P matrixes
        Rm, _ = cv2.Rodrigues(rvecs[0])
        rt = np.hstack((np.asarray(Rm), np.asarray(tvecs[0])))
        Pm = mtx.dot(rt)

        dist_pickle = {}
        dist_pickle["mtx"] = mtx
        dist_pickle["dist"] = dist
        dist_pickle["Rm"] = Rm
        dist_pickle["Pm"] = Pm
        try:
            with open('camera{}/calibration_params.pickle'.format(cam_num), 'wb') as f:
                pickle.dump(dist_pickle, f, pickle.HIGHEST_PROTOCOL)
                print("Calibration parameters was saved to file")
        except:
            raise BlockingIOError('Ошибка записи в pickle файл')

        #yaml
        fillfile('camera{}/apollo_calibrate.yaml'.format(cam_num), dist, mtx, Rm, Pm)

        return mtx, dist


    def start_calibrate(key, cam_num):
        """
        Перенаправление в зависимости от прочитанного ключа
        Вывод графического окна с результатом калибровки
        :key: ключ, введенный в начале выполнения программы
        :cam_num: номер/имя камеры
        """
        for i in range(5):
            print(5-i)
            time.sleep(1)

        if key == '0':
            make_photos.collect_data('camera{}/dataset'.format(cam_num))
        if key == '0' or key == '1':
            Km, Dm = calibrate_camera('camera{}/dataset'.format(cam_num),cam_num)
        if key == '2':
            with open('camera{}/calibration_params.pickle'.format(cam_num), 'rb') as f:
                data = pickle.load(f)
                Km = data["mtx"]; Dm = data["dist"]
        else:
            raise ValueError('Число за пределами возможных ключей (0, 1, 2)')

        try:
            cam = cv2.VideoCapture(1)
        except:
            raise Exception('Невозможно получить данные от камеры')
        while True:
            ready, img = cam.read()
            while not(ready):
                print('Нет данных от камеры \n')
                time.sleep(1)
            img2 = np.copy(img)
            img2 = cv2.resize(img, (960, 540), interpolation=cv2.INTER_NEAREST)
            cv2.imshow('Без калибровки', img2)
            img_after = cv2.undistort(img, Km, Dm)
            img_after = cv2.resize(img_after, (960, 540), interpolation=cv2.INTER_NEAREST)
            cv2.imshow('Калибровка', img_after)
            if cv2.waitKey(1) & 0xFF == ord('q'):
                break
        cam.release()
        cv2.destroyAllWindows()


    print('0 - Новая калибровка со сборкой датасета\n')
    print('1 - Калибровка по существующему датасету\n')
    print('2 - Проверка существующих параметров калибровки\n')
    value = input()
    print('Имя камеры?')
    number = input()
    start_calibrate(value, number)


if name == "__main__":
    try:
        main()
    except Exception as e:
        logging.error(e)
