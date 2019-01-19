
//Kutuphaneler tanimlaniyor

#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>

#include <ESP8266HTTPClient.h>

#define USE_SERIAL Serial

ESP8266WiFiMulti WiFiMulti;  //Esp8266 hem sunucu hem de istemci olarak ayarlaniyor

void setup() {

  USE_SERIAL.begin(115200);//baudrate ayarlaniyor

  WiFi.mode(WIFI_STA);
 WiFiMulti.addAP("WIFINAME", "PASSWORD"); // access pointe baglanmak icin kullanici adi ve sifre
  
  pinMode(2,OUTPUT); //GPIO2 pini cikis olarak ayarlaniyor
  digitalWrite(2,HIGH); // GPIO2 pini set ediliyor
}

void loop() {
   String data=receivelastmessage();
   String mod=receivelastmessage2();


if(mod=="bootmode")
 { 
   digitalWrite(2,LOW);
 if((data=="guncelle") && (mod=="bootmode") )
 { 
  
  
  delay(5000);
  if ((WiFiMulti.run() == WL_CONNECTED)) {
    HTTPClient http; //http istemci olusturuluyor 
    
    http.begin("http://yazilimguncelleme.000webhostapp.com/user_app.bin");  
    int httpCode = http.GET();
    if (httpCode > 0) {
      if (httpCode == HTTP_CODE_OK) {
       int len = http.getSize();//datanin uzunlugu belirleniyor
       
       uint8_t buff[128] = { 0 };
        WiFiClient * stream = http.getStreamPtr();
        while (http.connected() && (len > 0 || len == -1)) {
          size_t size = stream->available();

          if (size) {
            //veri okunuyor
            int c = stream->readBytes(buff, ((size > sizeof(buff)) ? sizeof(buff) : size));  
           
          //okunan veri uarta yaziliyor
            USE_SERIAL.write(buff, c);
            

            if (len > 0) {
              len -= c;
            }
          }
          
        }

      }
    } else {
      USE_SERIAL.printf("[HTTP] GET... failed, error: %s\n", http.errorToString(httpCode).c_str());
    }

    http.end();
  }
  
  delay(5000);
  digitalWrite(2,HIGH);
  delay(20000);
 }
 }
}

/***********************************************************************************************************************/

//Web adresindeki veri tabanindan kontrol yapiliyor
String receivelastmessage()
{
  String lastmessage="";
  if(WiFi.status()==WL_CONNECTED)
  {
    HTTPClient http;
    String url="http://yazilimguncelleme.000webhostapp.com/guncelleVeritabani.txt";
    http.begin(url);
    http.addHeader("Content-Type","text/plain");
    int httpCode=http.GET();
    String data=http.getString();
    lastmessage=getlastline(data);
    http.end();
  }
  else
  {
    lastmessage="";
  }
  return lastmessage;
}
/*--------------------------------------------------------------------------------------------------------------------*/
String receivelastmessage2()
{
  String lastmessage="";
  if(WiFi.status()==WL_CONNECTED)
  {
    HTTPClient http2;
    String url="http://yazilimguncelleme.000webhostapp.com/modVeritabani.txt";
    http2.begin(url);
    http2.addHeader("Content-Type","text/plain");
    int httpCode=http2.GET();
    String mod=http2.getString();
    lastmessage=getlastline(mod);
    http2.end();
  }
  else
  {
    lastmessage="";
  }
  return lastmessage;
}

/*****************************************************************************************************************************/


//txt doayasinin icindeki verinin son satiri okunuyor
String getlastline(String str)
{
  String s="";
  int len=str.length();
  for (int i=len-2;i>=0;i--)
  {
    if (str[i]=='\n')
    {
      break;
    }
    else
    {
      s=s+str[i];
    }
  }
  String rs="";
  for (int i=s.length()-1;i>=0;i--)
  {
    rs=rs+s[i];
  }
  return rs;
}



