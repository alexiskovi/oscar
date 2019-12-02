Проект OSCAR является ответвлением проекта [Apollo](https://github.com/ApolloAuto/apollo) релиза 3.5.

### Установка ПО

1. Установите [Docker](https://docs.docker.com/install/linux/docker-ce/ubuntu/)

2. Склонируйте репозиторий проекта:

    Публичный репозиторий сообщества

    ```
    git clone https://gitlab.com/starline/oscar.git
    ```

    Локальный репозиторий для разработчиков StarLine

    ```
    git clone git@git.starline.ru:oscar/oscar.git
    ```

3. Выполнив следующую команду вы запустите докер-контейнеры для работы apollo:

    ```
    cd oscar
    bash docker/scripts/dev_start.sh
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

    В случае отсутствия UI Dreamview по localhost:8888 требуется пересобрать frontend Dreamview в подгрузкой соответствующих зависимостей:

    ```
    cd modules/dreamview/frontend
    yarn install && yarn build
    ```

    В случае наличия UI установку можно считать законченой


[Устранение возможных проблем](docs/oscar/possible_problems.md)

### Настройка и калибровка датчиков

Для калибровки камер, а так же определения и настройки фокусного растояния камер ознакомтесь с соответствующией инструкцией - [README](../../scripts/oscar/sensor_calibration/camera/README.md)

### oscar tools

Для использования oscar tools нужно добавить соответствующие переменные окружения:

```
echo "source path-to-oscar-root/scripts/oscar/oscar_tools/setup.sh" >> ~/.bashrc
source ~/.bashrc
```