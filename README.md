# PPC_OS
Портативный программируемый компьютер с собственной операционной системой и своим языком программирования. Кибердека на максималках.
<img width="1811" height="1329" alt="image" src="https://github.com/user-attachments/assets/1aa949eb-b13b-423c-a09c-9faf8d6658e6" />

#### Что он умеет?
##### Рабочий стол
PPC OS умеет работать с файлами, в том числе - позволяет поставить обои на рабочий стол. Рабочий стол имеет виджеты даты, времени, а также можно включить напоминание о предстоящих делах из календаря. Уведомление из календаря включается если оно завтра/после завтра. Отображение виджетов рабочего стола можно изменить в настройках. Например, убрать уведомления календаря или убрать время, чтобы картинку на рабочем столе было видно полностью.


Экран имеет функцию автоматической блокировки. После выхода на рабочий стол, спустя заданное в настройках время (по умолчанию 5 минут). Экран заблокируется, внизу появится надпись А - РАЗБЛОК. Для входа в главное меню нужно нажать клавишу А с клавиатуры и ввести пинкод. Без этого не войти. Это позволяет заблокировать личные данные на устройстве. Никто не прочитает ваши заметки или адресную книгу.
<img width="1881" height="1297" alt="image" src="https://github.com/user-attachments/assets/1c737b78-838d-4aac-832e-ca48788a85a1" />

После разблокировки:
<img width="500" height="300" alt="image" src="https://github.com/user-attachments/assets/8cbdc4e2-5caf-45e7-8db2-5f0e0f8af603" />


## Главное Меню
Главное меню состоит из 8 разделов
1. WI-FI
2. ДАТА И ВРЕМЯ
3. ВЫЧИСЛЕНИЯ
4. ЗАПИСИ
5. ФАЙЛЫ
6. ИГРЫ
7. PROGRAM
8. НАСТРОЙКИ

### WI-FI
1. Сети / Подключение
ESP32 имеет радио модуль для работы с wifi. Она может сканировать ближайшие wifi сети и подключаться к ним.
Плата использует wifi для определения точного времени, а также для передачи файлов.
2. HTTP ОБМЕН ФАЙЛАМИ
Если плата подключена к wifi сети. Можно включить передачу данных по http. Плата создаст http сервер и предложит вам задать пинкод.
<img width="749" height="475" alt="image" src="https://github.com/user-attachments/assets/15000bd1-8966-4c0b-916f-6891409f7763" />

Пинкод установлен из за соображений безопасности, ведь вашу сеть могут просканировать и открыть http сервер PPC_OS. Будьте осторожны и обязательно передавайте файлы с пинкодом. Также, рекомендуется использовать функцию только в проверенных wifi сетях.
Вот так выглядит http сервер PPC_OS:
<img width="1090" height="421" alt="image" src="https://github.com/user-attachments/assets/eb44cb3f-acc6-417b-836b-50d9576f4370" />
<img width="728" height="807" alt="image" src="https://github.com/user-attachments/assets/cdfca397-3894-47e7-b404-b965a0190cc5" />

### ДАТА И ВРЕМЯ
1. ЧАСЫ И ДАТА
Показывает текущее время и дату синхронизированную через wifi.
<img width="547" height="341" alt="image" src="https://github.com/user-attachments/assets/3c6d13b5-ce2c-4b2d-95d7-bcc250821d48" />

2. ТАЙМЕР
Позволяет задать отчёт в формате ЧЧММСС.
<img width="507" height="405" alt="image" src="https://github.com/user-attachments/assets/1c7e645a-6c74-4328-84be-e3282c90dac0" />

3. СЕКУНДОМЕР
Позволяет засечь время
<img width="545" height="451" alt="image" src="https://github.com/user-attachments/assets/b1aa7709-7a79-481a-9ae7-ba0a99c533bb" />

4. Календарь
Календарь отображает все 12 месяцев. В нём можно создавать записи на определённый день и они будут отображаться на рабочем столе если они близко.
<img width="725" height="519" alt="image" src="https://github.com/user-attachments/assets/03fcb1a8-2818-4d5b-a00a-9b0d037b215f" />
Событие отображается точкой справа от числа (см на 16 число). На примере записи события можете изучить ввод с клавиатуры. Реализован как на кнопочных телефонах, т.е. есть подсказка где находится буква и сколько раз нужно нажать кнопку чтобы её ввести.
<img width="616" height="462" alt="image" src="https://github.com/user-attachments/assets/fa306a70-5c55-4eb1-b1b1-1393c841302a" />

### Вычисления
1. Обычный калькулятор
Самый стандартный калькулятор. Выполняет операции сложения, вычитания, умножения и деления.
2. ПРОГРАММИСТСКИЙ + КОНВ
<img width="716" height="504" alt="image" src="https://github.com/user-attachments/assets/d731e576-899e-4dfe-8695-35cb1a45b6ff" />
<img width="837" height="559" alt="image" src="https://github.com/user-attachments/assets/44d28e6c-6a25-4b86-9243-00d27cfcae01" />
3. ТАБЛИЦА ИСТИННОСТИ
Позволяет выбрать логическую операцию и построить по ней таблицу истинности.
<img width="747" height="485" alt="image" src="https://github.com/user-attachments/assets/bfb74cca-2044-42a7-a68b-77192fae46a9" />
4. МАТЕМАТИЧЕСКИЙ КАЛЬКУЛЯТОР
Расширенная версия обычного калькулятора, позволяющая посчитать значения математических фукнций. Например 

Калькулятор позволяет проводить логические операции (AND, NAND, OR, NOR, XOR, XNOR, NOT, BUFFER). Имеет конвертер (DEC, HEX, BIN)
<img width="717" height="519" alt="image" src="https://github.com/user-attachments/assets/826fa7de-85e2-4416-9fa5-4b268d7ff777" />
cos 1 = 0.999848
<img width="744" height="490" alt="image" src="https://github.com/user-attachments/assets/6bed997c-ae38-48f4-846f-aada724440be" />

5. ГРАФИК ФУНКЦИЙ
Позволяет строить графики функций. Например график синусоиды y = sin(x). Поддерживает регулировку построения графика.
<img width="627" height="397" alt="image" src="https://github.com/user-attachments/assets/f9a0972e-c6d9-4a66-ac7d-9d8497477565" />

6. ГЕНЕРАТОР ПАРОЛЕЙ
Модуль генератора паролей позволяет генерировать пароли в различных режимах (Только цифры, Усиленный, Буквы + цифры, HEX)
<img width="661" height="435" alt="image" src="https://github.com/user-attachments/assets/b9a0f4b6-7865-4c9c-b42f-00acd1fc3ef0" />
<img width="749" height="485" alt="image" src="https://github.com/user-attachments/assets/4458d311-4485-4989-b8ed-843814548d25" />
<img width="617" height="467" alt="image" src="https://github.com/user-attachments/assets/69208adc-808c-4e8e-80df-f1d3cabb7bd1" />
<img width="565" height="479" alt="image" src="https://github.com/user-attachments/assets/ad031cff-5457-4b82-bd47-7445ed6aba40" />

7. HASH КАЛЬКУЛЯТОР
Хэширует введёную строчку. Поддерживает алгоритмы CRC32, SHA1, SHA256.
<img width="544" height="491" alt="image" src="https://github.com/user-attachments/assets/803116ab-5687-4fb5-83d6-11c29e118261" />

<img width="655" height="517" alt="image" src="https://github.com/user-attachments/assets/34d34390-579d-4bc6-98e7-e00f135ade21" />


### Записи
1. Обычные заметки
Позволяет создавать заметки (plaintext). Может открывать текущие, создавать новые в виде файлов NOTE.TXT и удалять. Поддерживает режим навигации.
<img width="797" height="463" alt="image" src="https://github.com/user-attachments/assets/c686f900-bc99-47e5-abb1-b97feff32644" />

<img width="629" height="485" alt="image" src="https://github.com/user-attachments/assets/70f6041a-56a2-4f0f-bdca-466ea9a51deb" />
Реализована защита от случайного удаления файлов: перед удалением PPC OS попросит вас нажать клавишу C дважды.
<img width="651" height="503" alt="image" src="https://github.com/user-attachments/assets/f3b52657-8eaf-4a00-9c35-05e1ece397b3" />

2. ЗАШИФРОВАННЫЕ
Расширенный модуль заметок. Зашифрованные заметки создаются в отдельной хранилище внутри LittleFS. Записки шифруется AES-256 и сохраняются в виде шифротекстов SEC.DAT. Пинкод напрямую во флешпамять не записывается, вместо этого сохраняется его sha256 хэш.

При открытии нужно ввести пинкод расшифрования:
<img width="689" height="509" alt="image" src="https://github.com/user-attachments/assets/9e5995cc-c466-406d-9380-e783e73f093c" />
<img width="655" height="479" alt="image" src="https://github.com/user-attachments/assets/a23bd622-c0fc-402c-9ee9-36d3e0671ef3" />
<img width="613" height="491" alt="image" src="https://github.com/user-attachments/assets/446c148e-3986-4a23-9f89-1fa4a28ab4d0" />

3. АДРЕССНАЯ КНИГА
Адресная книга работает по такому же методу что и модуль зашифрованных записок. Но вместо записок здесь адресная книга.
Адресная книга зашифрована aes256. При открытии нужно ввести пинкод:

<img width="603" height="455" alt="image" src="https://github.com/user-attachments/assets/f1a8cad7-96ef-494f-9a50-6f13a9ba401c" />

<img width="835" height="551" alt="image" src="https://github.com/user-attachments/assets/2ef0e6b6-48b2-4e48-9740-1e1e289e287a" />
<img width="474" height="393" alt="image" src="https://github.com/user-attachments/assets/428abbcb-4855-40e4-99f6-bf8b747788f4" />


### Файлы
1. МЕНЕДЖЕР ФАЙЛОВ
<img width="585" height="437" alt="image" src="https://github.com/user-attachments/assets/f9324546-6d7a-434a-8986-cd6daa320c28" />
Позволяет проводить операции с файлами. Позволяет вывести информацию по файлу, переименовать, копировать, переместить, удалить, и сделать новую папку
<img width="747" height="473" alt="image" src="https://github.com/user-attachments/assets/671b9b6b-28bd-4e21-aa37-7e343d77ef2b" />
<img width="677" height="443" alt="image" src="https://github.com/user-attachments/assets/e2ed9461-9593-4459-82ce-76b20df4f40d" />

При открытии изображения открывается средство просмотра изображений. Можно переключаться между изображениями, а также выбрать какое понравится и поставить его на обои. Есть список изображений идущий по умолчанию. Их можно скачать из папки PPC_OS_wallpapers.

<img width="487" height="357" alt="image" src="https://github.com/user-attachments/assets/57a7cea6-2907-477e-be23-2910daaa0554" />
<img width="511" height="367" alt="image" src="https://github.com/user-attachments/assets/d481ad91-93e9-46b5-b515-cfd7382f0a22" />
<img width="463" height="353" alt="image" src="https://github.com/user-attachments/assets/9ac1192f-a784-4a2a-883a-8db9ebc51fe2" />
<img width="501" height="359" alt="image" src="https://github.com/user-attachments/assets/b10ff7de-f890-45ef-8043-395067d80d90" />
<img width="489" height="373" alt="image" src="https://github.com/user-attachments/assets/3baec4cb-b184-429a-a06b-2918a2ea7009" />
<img width="479" height="349" alt="image" src="https://github.com/user-attachments/assets/ce9cbe84-0c72-4f1e-b22b-a2d98c686a05" />
<img width="491" height="341" alt="image" src="https://github.com/user-attachments/assets/38cd2fdc-bfe6-4d2a-a8bb-ee796018f414" />
<img width="507" height="363" alt="image" src="https://github.com/user-attachments/assets/6a170d78-4b66-4add-81f7-ef4b275348c5" />
<img width="493" height="355" alt="image" src="https://github.com/user-attachments/assets/84740659-024d-4fa4-9d2d-a7d96f7a35ff" />



3. ЧТЕНИЕ ТЕКСТА
Средство просмотра текста позволяет открыть и отредактировать текстовые файлы.
<img width="687" height="533" alt="image" src="https://github.com/user-attachments/assets/c5335a04-2ff2-416c-b6a5-1049aa8ca8a9" />
<img width="561" height="471" alt="image" src="https://github.com/user-attachments/assets/f52fbb27-f31d-4f05-af40-d16395f43be2" />

5. HEX-РЕДАКТОР
HEX-редактор позволяет открыть любые файлы и изменить их байты.
<img width="455" height="369" alt="image" src="https://github.com/user-attachments/assets/d073cd9d-c9ed-4db5-a171-f9eb240eec9d" />
<img width="501" height="393" alt="image" src="https://github.com/user-attachments/assets/bdcaff71-fc11-47d8-89ab-600cfacd30f1" />

### ИГРЫ
Разумеется тут буду игры. Чтобы можно было отдохнуть, поиграть, или просто заняться чем то пока стоишь в очереди куда нибудь.
<img width="703" height="497" alt="image" src="https://github.com/user-attachments/assets/747a2327-6fc8-4c94-a581-08bb1cd04ce2" />

1. ПИНГ-ПОНГ
Реализована игра в пинг-понг с ботом.
<img width="627" height="521" alt="image" src="https://github.com/user-attachments/assets/0721104b-32cb-4a94-9bef-fd7adaff6eaa" />

3. СУДОКУ 4х4
Реализовано судоку 4x4. Каждый раз генерируется случайный набор.
<img width="521" height="405" alt="image" src="https://github.com/user-attachments/assets/ef5cbcc8-838b-486e-86ea-8c4716428c08" />

### PROGRAM
Вот тут самое крутое. Program - это встроенная среда программирования. Она позволяет пользователю самостоятельно создавать, редактировать и запускать программы непосредственно на устройстве (БЕЗ обновления через прошивку!). Для этого была разработан собственный интерпретируемый язык PPC Script и программная среда для его выполнения. Благодаря разделу PROGRAM устройство не ограничивается набором заранее созданных приложений: пользователь может задавать собственные алгоритмы и выполнять их без подключения компьютера. 
<img width="651" height="483" alt="image" src="https://github.com/user-attachments/assets/991645bd-cc49-435f-8008-02080f8e5b74" />
1. ГДЕ ПИСАТЬ И КАК ЗАПУСКАТЬ
----------------------------
Главное меню → PROGRAM → РЕДАКТОР — написать или изменить программу.
PROGRAM → ЗАПУСТИТЬ — выполнить программу.
PROGRAM → СПРАВКА — встроенная краткая справка.

Программа сохраняется во Flash-памяти ESP32, в LittleFS:
/PROGRAM.PPC

Одна команда = одна строка. Команды и имена регистров можно писать
заглавными или строчными буквами. Строки нумеруются с 1; пустые
строки и строки с комментариями также считаются при переходах GOTO.

2. РЕГИСТРЫ И ЗНАЧЕНИЯ
---------------------
A, B, C, ... Z — 26 целочисленных регистров (знаковые 32-битные числа).
При каждом новом запуске все регистры обнуляются.
В качестве числового аргумента можно указать число или другой регистр:

SET A 10     — A = 10
SET B A      — B = значение A
ADD A B      — A = A + B

Для чисел не поддерживаются дробные значения (1.5 и т. п.).
Деление DIV — целочисленное, дробная часть отбрасывается.

3. ВСЕ КОМАНДЫ
--------------

ПРИСВАИВАНИЕ И АРИФМЕТИКА

SET A 10     Записать 10 в A.
SET A B      Скопировать значение B в A.
ADD A 5      Прибавить 5 к A: A = A + 5.
SUB A 5      Вычесть 5 из A: A = A - 5.
MUL A 5      Умножить A на 5: A = A * 5.
DIV A 5      Целочисленно разделить A на 5: A = A / 5.
MOD A 5      Записать остаток от деления A на 5.
INC A        Увеличить A на 1.
DEC A        Уменьшить A на 1.

Для SET, ADD, SUB, MUL, DIV и MOD вместо второго числа допустим
регистр: например, MUL A B. Деление и MOD на ноль вызывают ошибку.

ВЫВОД

PRINT A          Вывести текущее значение регистра A.
PRINT HELLO      Вывести текст HELLO.
PRINT "A"        Вывести именно букву A, а не значение регистра A.
PRINT "HELLO!"   Вывести текст без внешних кавычек.
CLS              Очистить область вывода на экране.

Если после PRINT записано только имя регистра A–Z, печатается его
значение. Чтобы вывести саму букву, заключите её в двойные кавычки.

ПЕРЕХОДЫ И УСЛОВИЯ

GOTO 4                  Безусловно перейти на строку 4.
IF A > 5 GOTO 8         Если A больше 5, перейти на строку 8.
IF A <= B GOTO 2        Если A меньше либо равно B, перейти на строку 2.

Операторы сравнения для IF:
==  равно                     !=  не равно
>   больше                    <   меньше
>=  больше или равно          <=  меньше или равно

Общий вид: IF значение1 оператор значение2 GOTO номер_строки
Вместо значения1, значения2 и номера строки допустим регистр.
При ложном условии выполняется следующая по порядку строка.
Номера строк начинаются с 1. Переход за пределы программы — ошибка.

ЗАДЕРЖКА И ЗАВЕРШЕНИЕ

WAIT 1000      Подождать 1000 мс (1 секунду) перед следующей командой.
WAIT A         Подождать число миллисекунд, записанное в A.
END            Завершить программу.
STOP           То же, что END.

WAIT принимает неотрицательное целое число; значения свыше
60 000 мс ограничиваются 60 000 мс. Во время ожидания устройство
продолжает опрашивать клавиатуру.

КОММЕНТАРИИ В ПРОГРАММЕ

REM мой комментарий      Строка-комментарий, не выполняется.
// мой комментарий       Другой вариант строки-комментария.

REM и // распознаются в начале строки как отдельные команды. Вставлять
их в конец строки с другой командой как встроенный комментарий не надо.

4. ПРИМЕР: ЦИКЛ ОТ 1 ДО 5
-------------------------

SET A 1
PRINT A
ADD A 1
IF A <= 5 GOTO 2
PRINT DONE
END

Вывод:
1
2
3
4
5
DONE
END

Построчно: сначала A=1; вывести A; увеличить A; если A<=5,
вернуться на строку 2; иначе вывести DONE и завершить программу.

5. УПРАВЛЕНИЕ ВО ВРЕМЯ ВЫПОЛНЕНИЯ
--------------------------------
A       Пауза / продолжить; после завершения — запустить снова.
C       Очистить окно вывода.
D или B Остановить программу и вернуться в меню PROGRAM.

На экране видны состояние выполнения и номер строки.
В окне вывода помещаются семь последних строк; длинные строки
обрезаются до 25 символов.

Интерфейс виртуальной среды
<img width="651" height="483" alt="image" src="https://github.com/user-attachments/assets/fb934750-b51e-4ad2-be66-01d74956563f" />

1. Редактор
Как было описано выше - он для создания и редактирования программ. На изображении приведён программы, её запуск приведён в разделе Запустить.
<img width="619" height="443" alt="image" src="https://github.com/user-attachments/assets/f0431075-231d-4520-bd07-9f4915d9333f" />

2. Запустить
Используется для запуска программ. 
<img width="531" height="417" alt="image" src="https://github.com/user-attachments/assets/ec68b7f5-81b4-4c04-a137-d36585b50a4f" />

3. Справка
Здесь пример маленькой программы.
<img width="489" height="353" alt="image" src="https://github.com/user-attachments/assets/da5a9d21-5047-4d3d-800b-d11b326b4238" />
<img width="581" height="447" alt="image" src="https://github.com/user-attachments/assets/4491b458-e218-4322-b31a-4ee96ca05fa0" />


### Настройки
В разделе настройки можно настроить систему, а также просмотреть программно-аппаратные данные оборудования
<img width="519" height="407" alt="image" src="https://github.com/user-attachments/assets/b6f87fed-4325-45e4-aa54-746dbc8ffbdf" />

1. Язык
Здесь можно выбрать язык. На данный момент доступен русский и английский
<img width="551" height="381" alt="image" src="https://github.com/user-attachments/assets/917a1930-ec47-4b0d-84d3-5c988d6affae" />
<img width="555" height="389" alt="image" src="https://github.com/user-attachments/assets/6a56b0a1-acaa-4c2d-ae1e-7bf3932bfc54" />

2. Тема
Здесь можно поменять тему. На данный момент реализовано две цветовые палитры: янтарная и классическая(более контрастная).
<img width="691" height="461" alt="image" src="https://github.com/user-attachments/assets/239f105f-e329-4003-bed2-72d1093fa7a8" />
<img width="711" height="469" alt="image" src="https://github.com/user-attachments/assets/8f7c8787-e136-4539-9a21-b956707813b1" />
<img width="761" height="505" alt="image" src="https://github.com/user-attachments/assets/927ea479-f5c0-4192-ae93-368b8007d2ca" />
<img width="697" height="505" alt="image" src="https://github.com/user-attachments/assets/35bb565f-5869-40fb-8c8b-ca829c20b1e3" />
<img width="695" height="473" alt="image" src="https://github.com/user-attachments/assets/9b37fdc7-df9e-4be2-a2d8-6a16daf44319" />
<img width="751" height="519" alt="image" src="https://github.com/user-attachments/assets/2ed32785-91f1-4797-a49b-e69fb7cd70ed" />

  
3. Рабочий стол
Здесь можно настроить отображение виджетов на рабочем столе и поменять обои.
<img width="751" height="519" alt="image" src="https://github.com/user-attachments/assets/98b985f6-7337-493c-ba53-b1eb178b1b18" />


   
4. Блокировка
Здесь можно настроить пинкод а также время до перехода в режим блокировки.
<img width="617" height="401" alt="image" src="https://github.com/user-attachments/assets/a8a5b37f-0bbd-4af9-83a0-4ac09619d288" />

   
5. О системе
Здесь можно посмотреть сведения о системе.
<img width="721" height="465" alt="image" src="https://github.com/user-attachments/assets/07673b4f-8d7c-4405-aa27-7540d2c4a858" />
<img width="656" height="408" alt="image" src="https://github.com/user-attachments/assets/d9faa5b3-9e98-4987-8000-2ce056deab2d" />


# Из чего состоит
Плата - ESP32 DevKit
<img width="780" height="453" alt="image" src="https://github.com/user-attachments/assets/e120f515-dd5f-4192-b962-b5cfba131a2b" />
К ней подключены клавиатура, дисплей, и батарейный отсек для автономной работы.
<img width="2097" height="2074" alt="image" src="https://github.com/user-attachments/assets/409f7605-be60-419a-b3ce-e10f84fef367" />

Матричная клавиатура 4x4. Клавиатура типа "липучка", работает намного лучше чем кнопочные аналоги.
# Какие провода куда подключать
К коннектору HX 543 подключены провода мама-папа нумерация слева направо.
1 -> D13
2 -> D14
3 -> D25
4 -> D26
5 -> D27
6 -> D32
7 -> D33
8 -> D23

Цветной дисплей 1.8 TFT LCD 128x160 ST7735S с SPI интерфейсом. Отличный рядовой дисплей. Использую его во многих проектах
Подключение
LED, VCC -> 3v3 (спаяны вместе)
SCK -> D18
SDA -> D5
AO -> D16
RESET -> D17
CS -> D22

<img width="1500" height="1500" alt="image" src="https://github.com/user-attachments/assets/80454591-5699-4471-b64e-752f70cb0155" />
<img width="2485" height="2167" alt="image" src="https://github.com/user-attachments/assets/1ab2572d-329d-449d-96fa-03566bdb05a8" />
<img width="2785" height="1802" alt="image" src="https://github.com/user-attachments/assets/3899a58c-2381-4227-82a9-4c6fe49d7bc1" />


Батарейный отсек.
+ -> VIN
- -> GND
<img width="2163" height="3375" alt="20260918_154742" src="https://github.com/user-attachments/assets/892550c0-eb17-4808-abf8-0764670e81d0" />
<img width="2243" height="3265" alt="20260918_154826" src="https://github.com/user-attachments/assets/be752800-2bd2-44f9-abe8-b6c6161d592d" />



