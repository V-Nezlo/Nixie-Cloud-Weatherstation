/*
Библиотека для простой работы с ThingSpeak и NTP серверами с помощью ESP8266, в дальнейшем возможен рост функционала
Автор: V.Nezlo
Github: github.com/V-Nezlo
Примеры использования данной библиотеки найдете у меня на канале:
Youtube: https://www.youtube.com/channel/UC9sNwBXF4GRgg2WCf5OX7IQ

ВНИМАНИЕ! Библиотека использует Hardware Serial! Монитор порта и загрузка прошивки через бутлоадер будет недоступна при подключенном ESP!
Для отладки кода используйте отдельный USB-TTL конвертер и подключайте его к RX порту ардуино

Используемая версия прошивки ESP8266:
AT version:1.7.4.0(May 11 2020 19:13:04)
SDK version:3.0.4(9532ceb)
compile time:May 27 2020 10:12:22
Bin version(Wroom 02):1.7.4

Версия 1.0
01.12.2020
*/

#pragma once
#include <Arduino.h>

class EasyTS {
  public:
    void reset(void);                   //перезапуск модуля wifi
    void wps_connect(void);                 //запрос на подключение по wps
    void std_connect(String ssid, String pass);       //запрос на подключение по SSID и паролю (ssid это ssid. pass это пароль)
    void sntp_config(int state, unsigned int timezone);              //конфигурирование ntp (state - 1 это активировать, 0 - выключить, timezone - часовой пояс, например 3  - это москва)
    String sntp_get();                    //получить текущеее время (возвращает в виде Mon Nov 30 16:02:15 2020 )
    void send(String api_w, int field, String data);        //отправить данные на TS поле и канал (api_w это API WRITE, field - поле, data - строка с записываемым значением)
    int request(String api_r, int field, String channel);   //запросить данные с TS (возвращает одно число, и возвращает 9 в случае ошибки, будет доделываться)
  private:
    void wait_char(char answer[]);                          //ждет строку
    void wait_char(char answer[],int time);                 //ждет строку time времени
};
extern EasyTS easyts;