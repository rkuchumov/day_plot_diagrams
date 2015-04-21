### Компиляция:
#### Unix:
1. `$ git clone https://github.com/rkuchumov/geo_diagrams`
2. `$ cd geo_diagrams`
3. `$ make`  
В результате будет создан исполняемый файл `geodiagrams` в текущей директории.

#### Windows (MinGW):
1. `$ git clone https://github.com/rkuchumov/geo_diagrams`
2. `$ cd geo_diagrams`
3. `$ mingw32-make`  
В результате будет создан исполняемый файл `geodiagrams.exe` в текущей директории.

### Использование:  
Для работы необходима программа gnuplot в одной из директорий $PATH  
`$ geodiagrams [АРГУМЕНТЫ] WFDISK`

WFDISK – путь к файлу описания (.wfdisk)

Аргументы коммандной строки:
<pre>
  -c, --channel=CODE         Имя канала  
  -o, --output=FILE          Имя создаваемого изображаения  
  -p, --olverlap=NUM         Количество соседних графиков, которые могут перекрываться перекрываться  
  -d, --order=NUM            Порядок фильтра Баттерворта  
  -H, --highcut=FREQUENCY    Нижняя частота среза фильтра Баттерворта  
  -L, --lowcut=FREQUENCY     Верхняя частота среза фильтра Баттерворта  
  -v, --verbose              Вывод подробных сообщений о действиях программы на консоль (stderr)  
  -?, --help                 Отображение списка аргументов и использования  
</pre>

Например,  
`$ geodiagrams -c MHZ -p 3 -o output.png ./test/data.wfdisc`  
`$ geodiagrams -c MHZ -L 0.03 -H 4.1 -o output.png ./test/data.wfdisc`
