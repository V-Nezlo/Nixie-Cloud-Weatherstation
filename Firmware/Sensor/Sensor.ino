#include <iarduino_RF433_Transmitter.h>                   // Подключаем библиотеку для работы с передатчиком FS1000A
iarduino_RF433_Transmitter radio(8);                     // Создаём объект radio для работы с библиотекой iarduino_RF433, указывая номер вывода к которому подключён передатчик
int inputdata[2];

void setup()
{
    radio.begin();                                        // Инициируем работу передатчика FS1000A (в качестве параметра можно указать скорость ЧИСЛО бит/сек, тогда можно не вызывать функцию setDataRate)
    radio.setDataRate     (i433_5KBPS);                   // Указываем скорость передачи данных (i433_5KBPS, i433_4KBPS, i433_3KBPS, i433_2KBPS, i433_1KBPS, i433_500BPS, i433_100BPS), i433_1KBPS - 1кбит/сек
    radio.openWritingPipe (5);                            // Открываем 5 трубу для передачи данных (передатчик может передавать данные только по одной из труб: 0...7)
}                                                         // Если повторно вызвать функцию openWritingPipe указав другой номер трубы, то передатчик начнёт передавать данные по вновь указанной трубе
void loop()
{
  inputdata[0]=100;
  inputdata[1]=200;
    radio.write(&inputdata, sizeof(inputdata));
    delay(50);
    radio.write(&inputdata, sizeof(inputdata));
    delay(50);
    radio.write(&inputdata, sizeof(inputdata));
    delay(50);
    radio.write(&inputdata, sizeof(inputdata));
    delay(50);
    radio.write(&inputdata, sizeof(inputdata));
    delay(50);
    radio.write(&inputdata, sizeof(inputdata));
    delay(50);
    radio.write(&inputdata, sizeof(inputdata));
    delay(50);
    radio.write(&inputdata, sizeof(inputdata));
    delay(50);
    radio.write(&inputdata, sizeof(inputdata));
    delay(50);
    radio.write(&inputdata, sizeof(inputdata));
    delay(50);
    delay(1000);                                            // пауза между пакетами
}
