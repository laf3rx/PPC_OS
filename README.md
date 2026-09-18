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


### Файлы //отчёт в разработке
1. МЕНЕДЖЕР ФАЙЛОВ
2. ЧТЕНИЕ ТЕКСТА
3. HEX-РЕДАКТОР

### ИГРЫ
1. ПИНГ-ПОНГ
2. СУДОКУ 4х4

### PROGRAM
Интерфейс виртуальной среды
1. Редактор
2. Запустить
3. Справка

### Настройки
1. Язык
2. Тема
3. Рабочий стол
4. Блокировка
5. О системе

# Из чего состоит
Плата - ESP32 DevKit
<img width="780" height="453" alt="image" src="https://github.com/user-attachments/assets/e120f515-dd5f-4192-b962-b5cfba131a2b" />

Модули
К ней подключены клавиатура, дисплей, и батарейный отсек для автономной работы.
<img width="2097" height="2074" alt="image" src="https://github.com/user-attachments/assets/409f7605-be60-419a-b3ce-e10f84fef367" />

Матричная клавиатура 4x4. Клавиатура типа "липучка", работает намного лучше чем кнопочные аналоги.
Подключение
К коннектору HX 543 подключены провода мама-папа нумерация слева направо.
1 -> D13
2 -> D14
3 -> D25
4 -> D26
5 -> D27
6 -> D32
7 -> D33
8 -> D23

<img width="1500" height="1500" alt="image" src="https://github.com/user-attachments/assets/80454591-5699-4471-b64e-752f70cb0155" />
<img width="2485" height="2167" alt="image" src="https://github.com/user-attachments/assets/1ab2572d-329d-449d-96fa-03566bdb05a8" />
<img width="2785" height="1802" alt="image" src="https://github.com/user-attachments/assets/3899a58c-2381-4227-82a9-4c6fe49d7bc1" />

Цветной дисплей 1.8 TFT LCD 128x160 ST7735S с SPI интерфейсом. Отличный рядовой дисплей. Использую его во многих проектах
Подключение
LED, VCC -> 3v3 (спаяны вместе)
SCK -> D18
SDA -> D5
AO -> D16
RESET -> D17
CS -> D22
<img width="2163" height="3375" alt="20260918_154742" src="https://github.com/user-attachments/assets/892550c0-eb17-4808-abf8-0764670e81d0" />
<img width="2243" height="3265" alt="20260918_154826" src="https://github.com/user-attachments/assets/be752800-2bd2-44f9-abe8-b6c6161d592d" />




Батарейный отсек.
+ -> VIN
- -> GND


