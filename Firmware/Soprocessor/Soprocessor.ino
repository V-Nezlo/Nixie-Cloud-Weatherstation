/*
В данном случае-в данном случае плата сопроцессора работает как i2c master и запрашивает данные у своего раба
*/

#include <Wire.h>
#include <GyverTimer.h>     

const char led_wifi = 8;
const char led_work = 9;
const char led_error = 10;
const char wifi_wps_pin = 11;

char send_value;//что именно отправляем
int tempin, tempout, humi, voltage;
char connection;
char wififlag, workflag, errorflag;

GTimer data_request(MS,10000);//Частота запросов для данных
GTimer data_send(MS,20000); //Частота отправления данных
GTimer led_status_update(MS,1000); //Частота отправления данных

void update_led(void){
    if (led_status_update.isReady()){
        wififlag = 0;
        errorflag = 0;
        if (workflag==0) workflag = 1;
        else workflag = 0;
    }
}

void set_indicators(){
    digitalWrite(led_wifi, wififlag);
    digitalWrite(led_work, workflag);
    digitalWrite(led_error, errorflag);
}

void wait_char(char answer[]){
int safe_counter;
while(!(Serial.find(answer))){
	safe_counter++;
	if (safe_counter>2){
		errorflag = 1;
		return;
		}
	}
delay(50); 
}

void wait_char(char answer[],int time){
int safe_counter;
while(!(Serial.find(answer))){
	safe_counter++;
	if (safe_counter>time){
		errorflag = 1;
		return;
		}
	}
delay(50); 
}

void std_connect(String ssid, String pass){
  Serial.println("AT");
  wait_char("OK");
  Serial.println("AT+CWMODE=1");
  wait_char("OK");
  Serial.println("AT+CWJAP=\"" + ssid + "\",\"" + pass + "\"");
  wait_char("WIFI CONNECTED", 10);
  wait_char("WIFI GOT IP", 10);
  wait_char("OK", 10);
  wififlag = 1;
  delay(200);
}

void send(String api_w, int field, String data){
String get = "GET /update?api_key=" + api_w + "&" + "field" + field + "=" + data;
Serial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80");
wait_char("OK");
Serial.print("AT+CIPSEND=");
Serial.println(get.length()+2); 
delay(300);
Serial.println(get);
wait_char("SEND OK");
}

void setup(){
Wire.begin();
Serial.begin(115200);
delay(500);
std_connect("Wi-Fi ZONE", "Mi246589");
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
        send("VF5VWODQICAG5BG3",1,datatosend);
        send_value = 1;
        break;
    
        case 1:
        sprintf (datatosend, "%d.%d", humi/10, humi%10); 
        send("VF5VWODQICAG5BG3",2,datatosend);
        if (connection) send_value = 2;
        else send_value = 0;
        break;

        case 2:
        sprintf (datatosend, "%d.%d", tempout/10, abs(tempout%10));
        send("VF5VWODQICAG5BG3",3,datatosend);
        send_value = 3;
        break;

        case 3:
        sprintf (datatosend, "%d.%d", voltage/100, voltage%100);
        send("VF5VWODQICAG5BG3",4,datatosend);
        send_value = 0;
        break;
    }
}
}
