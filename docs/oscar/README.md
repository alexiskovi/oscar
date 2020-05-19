Проект OSCAR является ответвлением проекта [Apollo](https://github.com/ApolloAuto/apollo).

### Установка ПО

1. Установите [Docker](https://docs.docker.com/install/linux/docker-ce/ubuntu/)

2. Склонируйте репозиторий проекта:

    Публичный репозиторий сообщества

    ```
    git clone https://gitlab.com/starline/oscar.git
    ```

3. Выполнив следующую команду, запустите докер-контейнеры для работы apollo:

    ```
    cd oscar
    bash docker/scripts/dev_start.sh -b
    ```

    В случае первого запуска после клонирования репозитория с удаленного докер-хранилища будут подтянуты все требуемые докер-образы автоматически. Для остановки всех докер-контейнеров apollo можно выполнить:

    ```
    bash docker/scripts/dev_start.sh stop
    ```

4. Для работы с apollo войдите в рабочий докер-контейнер:

    ```
    bash docker/scripts/dev_into.sh
    ```

5. Соберите проект:

    ```
    bash apollo.sh build
    ```

6. Запустите Dreamview:

    ```
    bash scripts/bootstrap.sh
    ```

    В случае наличия UI установку можно считать законченой. Работу с проектом можно начать используя [симулятор](lgsvl_simulator.md).


[Устранение возможных проблем](possible_problems.md)

### Настройка и калибровка датчиков

Для калибровки камер, а так же определения и настройки фокусного растояния камер ознакомтесь с соответствующией инструкцией - [README](../../scripts/oscar/sensor_calibration/camera/README.md).

### oscar_tools

*oscar_tools* - набор утилит, позволяющий упростить навигацию по проекту и реализующий интерфейс к разрабатываему в рамках проекта ПО.

Для использования *oscar_tools* вне контейнера задайте соответствующие переменные окружения:

```
echo "source path-to-oscar-root/scripts/oscar/oscar_tools/setup.sh" >> ~/.bashrc
source ~/.bashrc
```

[Документация по использованию oscar_tools](oscar_tools.md)


### Пример начала работы с OSCAR

[![](http://img.youtube.com/vi/9XCLKZ7Pb8s/0.jpg)](http://www.youtube.com/watch?v=9XCLKZ7Pb8s "")
