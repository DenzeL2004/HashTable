# Хеш-таблица


## Цель работы
Изучить возможности оптимизации хеш-таблицы с целью повышения их производительности.

## Теоретическая справка

Хеш-таблица — это структура данных, реализующая интерфейс ассоциативного массива.
Существуют два основных варианта хеш-таблиц: с открытой адресацией и списками. В данной работе представлена реализация второго варинта с фиксированным размером таблицы (при добавление или удаления элемента размр хеш-таблицы не изменяется).

В данной работе в таблице будут храниться строчные английские слова, длина которых не превосходит 32-ух символов. Ключ и знчение совпадают. 

Важное свойство хеш-таблиц состоит в том, что, при некоторых разумных допущениях, операции добавление, удаление, поиск элемента в среднем выполняются за время O(1). Чтобы этого достичь, распределение элементов по спискам должно быть равномерерным.

За распределение элементов отвечает **хеш-функция** - функция, осуществляющая преобразование массива входных данных произвольной длины в число (хеш).

При работе с хеш-функциями могут фозникать **коллизии** - совпадение значения хеша при различных значениях ключа.
При хорошем выборе хеш-функции шанс возникновения коллизей минимален.

# Часть I

В этой части работы было произведено сравнение распределения различных хеш-функций.

Для тестирования всех функция был взят один и тот же файл [words.txt](resource/words.txt). Размер хеш-таблицы не изменялся и был равен 700.

Список хеш-функций:
1. **DumbHash** - возвращающает всегда ноль.
2. **FirstByteHash** - возвращает байт код первой буквы в ключа.
3. **LenHash** - возвращет длину ключа.
4. **AsciiSumHash** - возвращающает сумму ascii-символов ключа.
5. **RolHash** - циклический сдвиг хеша влево и xor со значением элемента ключа.
6. **RorHash** - циклический сдвиг хеша вправо и xor со значением элемента ключа.
7. **CRC32Hash** - алгоритм нахождения контрольной суммы.

## Анализ

Ниже представлены граффики размеров списков после заполнения хеш-таблицы.

---

<h4> 1. DumbHash </h4>

![DumbHash](resource/img/DumbHash.png)

Результаты этой хеш-функции неудивтельны. При таком оаспределение среднее время поиска элемента в таблице будет занимать O(n).

- Максимальный размер списка: 58108
- Минимальный размер списка: 0
- Среднее квадратичное размеров списков: 2194.7

---

<h4> 2. FirstByteHash </h4>

![FirstByteHash](resource/img/FirstByteHash.png)

Эта хеш-функция показала себя уже лучше, чем предыдущая, однако количество коллизей остается велико.
Это связано с тем, что в качестве клучей брались только английские прописные слова, следовательно в качистве результата функции было всего 26 уникальных значений. 


- Максимальный размер списка: 6670          
- Минимальный размер списка: 0
- Среднее квадратичное размеров списков: 532.8

---

<h4> 3. LenHash </h4>

![LenHash](resource/img/LenHash.png)

По результатам распределения видем, что данная функций крайне плоха в качестве хешь-функции. Причина схожа с FirstByteHash. В связи с тем, что в качестве ключей берутся реальносуществующие английские слова, то их длина (в редких исключениях) не будет превосходит 32 символов.

- Максимальный размер списка: 9395     
- Минимальный размер списка: 0
- Среднее квадратичное размеров списков: 731.9      

---

<h4> 4. AsciiSumHash </h4>

![AsciiSumHash](resource/img/AsciiSumHash.png)

Результаты этой хеш-функции гораздо лучше, чем результаты трех предыдущих функций. Однако, как мы можем видеть, граффик распределения имеет *"волнообразный"* характер, в то время как нам бы хотелось более равномерное распределение.


- Максимальный размер списка:  234   
- Минимальный размер списка: 0
- Среднее квадратичное размеров списков: 50.6 


---

<h4> 5. RolHash </h4>

![RolHash](resource/img/RolHash.png)


- Максимальный размер списка:  168   
- Минимальный размер списка: 32
- Среднее квадратичное размеров списков: 28.2 

---

<h4> 6. RorHash </h4>

![RorHash](resource/img/RorHash.png)


- Максимальный размер списка:  162   
- Минимальный размер списка: 36
- Среднее квадратичное размеров списков: 22.4

---

<h4> 7. CRC32Hash </h4>

![CRC32Hash](resource/img/CRC32Hash.png)


- Максимальный размер списка:  114   
- Минимальный размер списка: 57
- Среднее квадратичное размеров списков: 9

---
 

## Вывод

Из собранных данных можем сказать, что лучше всего себя показала хеш-функция CRC32Hash. Ее квадартичное отклонение наименьшее среди всех представленных функций. Это говорит о том, что распределение элементов по хеш таблице почти равномерно.