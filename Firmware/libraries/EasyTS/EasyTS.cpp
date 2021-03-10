#include "EasyTS.h"

char wififlag, workflag, errorflag;

EasyTS easyts = EasyTS();

void EasyTS::reset(void){
  Serial.println("AT+RST");
  wait_char("OK");
}

void EasyTS::wait_char(char answer[]){
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

void EasyTS::wait_char(char answer[],int time){
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

void EasyTS::wps_connect(void){
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
	wififlag = 1;
}

void EasyTS::std_connect(String ssid, String pass){
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

void EasyTS::sntp_config(int state, unsigned int timezone){
	delay(10);
	String config = "AT+CIPSNTPCFG="; config += state; config += ","; config += timezone;
	Serial.println(config);
	wait_char("OK");
}

String EasyTS::sntp_get(){
	String answer; 	
	delay(10);
	Serial.println("AT+CIPSNTPTIME?");
	char buffer[56];
	Serial.readBytes(buffer, 56);
	Serial.println(buffer);

	int i;
	int a;
	for (;i<56;i++){
		if (buffer[i] == 58){
			i++;
			for (;a<24;a++)
			{
			answer += buffer[i];
			i++;
			}
			return answer;
		    
		}
	}
	errorflag = 1;
	return "error";
}

void EasyTS::send(String api_w, int field, String data){
String get = "GET /update?api_key=" + api_w + "&" + "field" + field + "=" + data;
Serial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80");
wait_char("OK");
Serial.print("AT+CIPSEND=");
Serial.println(get.length()+2); 
delay(300);
Serial.println(get);
wait_char("SEND OK");
}

int EasyTS::request(String api_r, int field, String channel){
String get = "GET /channels/" + channel + "/fields/" + field + ".csv?api_key=" + api_r + "&results=1";
Serial.println("AT+CIPSTART=\"TCP\",\"api.thingspeak.com\",80");
wait_char("OK");
Serial.print("AT+CIPSEND=");
Serial.println(get.length()+2); 
delay(300);
Serial.println(get);
wait_char("SEND OK");

char buffer[70];
Serial.readBytes(buffer, 70);

int i = 0;
int a = 0;

for (;i<70;i++){
	if (buffer[i] == 44){
		a++;
	}
	if (a == 5){
		i++;

		if (buffer[i] == 49) return 1;
		else if (buffer[i] == 48) return 0;
	}
}
errorflag = 1;
return 9;

}

