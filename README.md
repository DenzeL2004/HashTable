# Хеш-таблица


## Цель работы
Изучить возможности оптимизации хеш-таблицы с целью повышения их производительности.

## Теоретическая справка

Хеш-таблица — это структура данных, реализующая интерфейс ассоциативного массива.
Существуют два основных варианта хеш-таблиц: с открытой адресацией и списками. В данной работе представлена реализация второго варинта с фиксированным размером таблицы (при добавление или удаления элемента размр хеш-таблицы не изменяется).

В данной работе в таблице будут храниться английские слова, длина которых не превосходит 32-ух символов. Ключ и знчение совпадают. 

Важное свойство хеш-таблиц состоит в том, что, при некоторых разумных допущениях, операции добавление, удаление, поиск элемента в среднем выполняются за время O(1). Чтобы этого достичь, распределение элементов по спискам должно быть равномерерным.

За распределение элементов отвечает **хеш-функция** - функция, осуществляющая преобразование массива входных данных произвольной длины в число (хеш).

При работе с хеш-функциями могут фозникать **коллизии** - совпадение значения хеша при различных значениях ключа.
При хорошем выборе хеш-функции шанс возникновения коллизей минимален.

# Часть I

В этой части работы было произведено сравнение распределения различных хеш-функций.

Для тестирования всех функция был взят один и тот же файл [words.txt](resource/words.txt). Размер хеш-таблицы не изменялся и был равен 700.

Список хеш-функций:
1. DumbHash - возвращающает всегда ноль.
2. FirstByteHash - возвращает байт код первой буквы в ключа.
3. LenHash - возвращет длину ключа.
4. AsciiSumHash - возвращающает сумму ascii-символов ключа.
5. RolHash - циклический сдвиг хеша влево и xor со значением элемента ключа.
6. RorHash - циклический сдвиг хеша вправо и xor со значением элемента ключа.
7. CRC32Hash - алгоритм нахождения контрольной суммы.

## Анализ


В таблице ниже представлены данные по распределению элементов в таблице.

| Хеш-функция   | Максимальный размер | Минимальный размер | Среднее квадратичное |
|:-------------:|:-------------------:|:------------------:|:--------------------:|
| DumbHash      |       58108         |         0          |          2194        |
| FirstByteHash |       6670          |         0          |          532         |
| LenHash       |       9395          |         0          |          731         |
| AsciiSumHash  |       234           |         0          |          50          |
| RolHash       |       168           |         32         |          28          |
| RorHash       |       162           |         36         |          22          |
| CRC32Hash     |       114           |         57         |          9           |
 
 
Ниже представлены графики размеров списков при заполнения хеш-таблицы.


![Graph1](resource/img/DumbHash.png)

Последний график отражает размеры не всех списков, а лишь тех, чей порядковый номер кратен 25. Это было сделано для большей наглядности.


С полными результатами распределений по каждой хеш-функции можно ознакомится с файлом [result](temp/result_distribution.csv).


### Вывод


Из представленных данных можем заключить, что DumbHash ни в коем случае не рекомендуется для использования в хеш-таблицы количество. По итогу мы получаем массив, который к тому же будет работать медленнее, чем классическая структура данных, так как будет тратится системное время на вызов функции.


FirstByteHash и LenHash работают гораздо лучше, чем DumbHash. Однако количество коллизий остается достаточно большим. Это связано с тем, что берутся английские слова.


Следующие четыре хеш-функции(AsciiSumHash, RolHash, RorHash, CRC32Hash) показали себя гораздо лучше. Несмотря на это, AsciiSumHash выдает заметное число коллизий. Это следует из приведенных графиков, а также из данных таблицы(есть списки, чьи размеры остаются равными нулю).


Несмотря на то, что идея RolHash и RorHash остается одной и той же, на данных тестах RolHash показал себя гораздо лучше.


CRC32Hash показал себя лучше всего, на нем оказалось наименьшее число коллизий, однако стоить отметить, что в данной версии использовалась реализация с дополнительной памятью.

