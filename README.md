# PPC_OS
Портативный программируемый компьютер с собственной операционной системой и своим языком программирования. Кибердека на максималках.
<img width="320" height="568" alt="video_10mb" src="https://github.com/user-attachments/assets/0058916d-9089-4b7c-80d6-13098b6fa469" />






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


