### Компиляция и запуск (unix):
1. `$ git clone https://github.com/rkuchumov/geo_diagrams`
2. `$ cd geo_diagrams`
3. `$ make`  
В результате будет создан исполняемый файл `geodiagrams` в текущей директории.

Для работы необходима программа gnuplot. 

Использование:  
`$ geodiagrams [АРГУМЕНТЫ] WFDISK`

WFDISK – путь к файлу описания (.wfdisk)

Аргументы коммандной строки:
<pre>
  -c, --channel=CODE         Имя канала  
  -o, --output=FILE          Имя создаваемого изображаения  
  -p, --olverlap=NUM         Количество соседних графиков, которые график может перекрывать  
  -d, --order=NUM            Порядок фильтра Баттерворфа  
  -h, --highcut=FREQUENCY    Нижняя частота среза фильтра Баттерворфа  
  -l, --lowcut=FREQUENCY     Верхняя частота среза фильтра Баттерворфа  
  -v, --verbose              Вывод подробных сообщений о действиях программы на консоль (stderr)  
  -?, --help                 Отображение списка аргументов и использования  
</pre>

Например,  
`$ geodiagrams -c MHZ -p 3 -o output.png ./test/data.wfdisc`  
`$ geodiagrams -c MHZ -l 0.03 -h 4.1 -o output.png ./test/data.wfdisc`  

