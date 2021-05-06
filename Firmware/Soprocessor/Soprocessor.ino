/*
В данном случае-в данном случае плата сопроцессора работает как i2c master и запрашивает данные у своего раба
*/

//#include <Arduino.h>
#include <Wire.h>
#include <GyverTimer.h>
#include <avr/eeprom.h>   
#include <SoftwareSerial.h>

const char led_wifi = 8;
const char led_work = 9;
const char led_error = 10;
const char wifi_wps_pin = 11;
const char ee_flasher_pin = 12;

char send_value;//что именно отправляем
int tempin, tempout, humi, voltage;
char connection;
char wififlag, workflag, errorflag;
char wifi_con_status;

GTimer data_request(MS,10000);//Частота запросов для данных
GTimer data_send(MS,20000); //Частота отправления данных
GTimer led_status_update(MS,1000); //Частота обновления для статусного светодиода
GTimer led_error_update(MS,10000); //Частота обновления для светодиода ошибки
GTimer wifi_con_update(MS,83000);  //Частота проверки наличия подключения
GTimer wifi_reconnect(MS, 120000); //Частота переконнекта при отсутствии подключения

struct  EEDatas{
    String ssid;
    String pass;
};

EEDatas towrite;
EEDatas toread;

String api_w = "VF5VWODQICAG5BG3"; //Можно будет добавить смену ключа апи без перепрошивки

SoftwareSerial serialPC(11, 12); // RX, TX

void connect_type_selector(){
    if (digitalRead(wifi_wps_pin)){
        wps_connect();
    }
    else {
        std_connect(toread.ssid, toread.pass);
    }
}

void EE_writer_activator(void){
    if (digitalRead(ee_flasher_pin)){
        delay(8000);
        EE_writer();
        delay(2000);
        EE_reader();
    }
    else{
        EE_reader();
    }
}

void EE_writer(){
    String Data {""};
    bool data_selector = false;
    char character;
    while (serialPC.available()) //пока софтварный порт не пуст
    {
        character = serialPC.read(); //получаем 1 байт
        if ((character != "44")&&(data_selector==false)){ //пока не видим запятой и буль для выбора цели равен нулю пишем в ссид
            towrite.ssid.concat(character);
        }
        else if (character == "44"){ //если поймали запятую - пропускаем ее и переводим в режим приема пароля
            data_selector = true;
        }
        else if (data_selector==true){ //пишем пароль
            towrite.pass.concat(character);
        }
        if (character == '\n') //поймали перенос каретки - прием закончен, обнуляем дату
        {
            Data = "";
            serialPC.println("LOG+PASS APPROWD");
        }
    }
    eeprom_write_block((void*)&towrite, 10, sizeof(towrite));//пишем получившееся в еепром
    return;
}

void EE_reader(){
      eeprom_read_block((void*)&toread, 10, sizeof(toread));
}

void update_led(void){
    if (led_status_update.isReady()){
        if (workflag==0) workflag = 1;
        else workflag = 0;
    }
    if (led_error_update.isReady()){
        errorflag = 0;
    }
}

void check_connection(void){
    data_send.stop();
    Serial.println("AT+CWJAP?");
    if (Serial.find("No AP")){
        wifi_con_status = 0;
        wififlag = 0;
    }
    else{
        wifi_con_status = 1;
        wififlag = 1;
    }
    data_send.resume();
}

void set_indicators(){
    digitalWrite(led_wifi, wififlag);
    digitalWrite(led_work, workflag);
    digitalWrite(led_error, errorflag);
}

void wait_char(char answer[]){
    volatile int safe_counter;
    while(!(Serial.find(answer))){
	    safe_counter++;
	    if (safe_counter>5){
		errorflag = 1;
		return;
		}
	}
delay(50); 
}

void wait_char(char answer[],int time){
    volatile int safe_counter;
    while(!(Serial.find(answer))){
	safe_counter++;
	if (safe_counter>time){
		errorflag = 1;
		return;
		}
	}
    delay(50); 
}

void std_connect(const String &ssid, const String &pass){
  volatile int safe_counter=0;
  Serial.println("AT");
  wait_char("OK");
  Serial.println("AT+CWMODE=1");
  wait_char("OK");
  Serial.println("AT+CWJAP_CUR=\"" + ssid + "\",\"" + pass + "\"");
  delay(4000);
  if (Serial.find("WIFI DISCONNECT")){
      errorflag = 1;
      return;
  }
  wait_char("WIFI CONNECTED", 20);
  wait_char("WIFI GOT IP", 20);
  wait_char("OK", 20);
  delay(200);
  check_connection();
}

void wps_connect(void){
	delay(10);
  	Serial.println("AT");
  	wait_char("OK");
 	Serial.println("AT+CWMODE=1");
  	wait_char("OK");
	Serial.println("AT+WPS");
	wait_char("wps started");
	wait_char("OK");
	wait_char("wps success,connecting ap ...", 20);
	wait_char("WIFI CONNECTED", 20);
    delay(200);
    check_connection();
}

void send(const String &api_w, int field, const String &data){
    wifi_con_update.stop();
    String get = "GET /update?api_key=" + api_w + "&" + "field" + field + "=" + data;
    Serial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80");
    wait_char("OK");
    Serial.print("AT+CIPSEND=");
    Serial.println(get.length()+2); 
    delay(300);
    Serial.println(get);
    wait_char("SEND OK");
    wifi_con_update.resume();
}

void setup(){
    Wire.begin();
    Serial.begin(115200);
    serialPC.begin(115200);
    delay(2000);
    EE_writer_activator();
    connect_type_selector();
    pinMode(led_wifi, OUTPUT);
    pinMode(led_work, OUTPUT);
    pinMode(led_error, OUTPUT);
    pinMode(wifi_wps_pin, INPUT);
}

void loop(){

    set_indicators();
    update_led();
    if (data_request.isReady()) Wire.requestFrom(17, 9);
    if(Wire.available()) {
	    byte tempinL = Wire.read();
        byte tempinH = Wire.read();
        byte humiL = Wire.read();
        byte humiH = Wire.read();
        byte tempoutL = Wire.read();
        byte tempoutH = Wire.read();
        byte voltageL = Wire.read();
        byte voltageH = Wire.read();
        connection = Wire.read();

        tempin = tempinL|(tempinH<<8);
        humi = humiL|(humiH<<8);
        tempout = tempoutL|(tempoutH<<8);
        voltage = voltageL|(voltageH<<8);
}

if (data_send.isReady()){
    char datatosend[30];
    switch (send_value){
        case 0:
        sprintf (datatosend, "%d.%d", tempin/10, tempin%10); 
        send(api_w,1,datatosend);
        send_value = 1;
        break;
    
        case 1:
        sprintf (datatosend, "%d.%d", humi/10, humi%10); 
        send(api_w,2,datatosend);
        if (connection) send_value = 2;
        else send_value = 0;
        break;

        case 2:
        sprintf (datatosend, "%d.%d", tempout/10, abs(tempout%10));
        send(api_w,3,datatosend);
        send_value = 3;
        break;

        case 3:
        sprintf (datatosend, "%d.%d", voltage/100, voltage%100);
        send(api_w,4,datatosend);
        send_value = 0;
        break;
    }
}

if (wifi_con_update.isReady()){
    check_connection();
}
}
