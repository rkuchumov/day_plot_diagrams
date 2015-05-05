### Компиляция:
#### Unix, Cygwin:
1. `$ git clone https://github.com/rkuchumov/geo_diagrams`
2. `$ cd geo_diagrams`
3. `$ make`  
В результате будет создан исполняемый файл `dpd` в текущей директории.

#### Windows (MinGW):
1. `$ git clone https://github.com/rkuchumov/geo_diagrams`
2. `$ cd geo_diagrams`
3. `$ mingw32-make`  
В результате будет создан исполняемый файл `dpd.exe` в текущей директории.

### Использование:  
Для работы необходима программа gnuplot в одной из директорий $PATH  
`$ dpd [АРГУМЕНТЫ] WFDISK`

WFDISK – путь к файлу описания (.wfdisk), или путь к директории, содержащей data.wfdisc  

Аргументы коммандной строки:
<pre>
  -c, --channel=CODE         Имя канала (default: MHZ)  
  -o, --output=FILE          Имя создаваемого изображаения  
  -p, --olverlap=NUM         Количество соседних графиков, которые могут перекрываться перекрываться (default: 3)  
  -d, --order=NUM            Порядок фильтра Баттерворта (default: 6)  
  -H, --highcut=FREQUENCY    Нижняя частота среза фильтра Баттерворта  
  -L, --lowcut=FREQUENCY     Верхняя частота среза фильтра Баттерворта  
  -v, --verbose              Вывод подробных сообщений о действиях программы на консоль (stderr)  
  -h, --help, --usage        Отображение списка аргументов и использования  
</pre>

Например,  
`$ dpd -c MHZ -p 3 -o output.png ./test/data.wfdisc`  
`$ dpd -c MHZ -L 2 -H 8 ./test`  

Если не укзано имя изображения (`-o, --output=FILE`), то по умолчанию будет `DATE_STATION_CHANNEL_FILTER_OVERLAP.png`. Например, `20150425_PTZ3_MHZ_2.00_20.00_3.png`.  

Координаты станций могут быть указаны в файле `stations.txt`. Каждая строка которого должна состоять из имени станции, пробела, и произвольной строки с координатами.
