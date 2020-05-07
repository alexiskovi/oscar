# OSCAR Supervisor module
Задача модуля *Supervisor* - наблюдение за исправной работой узлов беспилотного автомобиля и его остановка в случае ошибок.  
### Запуск
Компонент IPC, прослушивающий все узлы и выводящий информацию в /supervisor/general:

```mainboard -d /apollo/modules/supervisor/dag/sv_ipc.dag```

Компонент Guardian, останавливающий автомобиль (в стадии разработки) и информирующий оператора об ошибках:

```mainboard -d /apollo/modules/supervisor/dag/sv_guardian.dag```
