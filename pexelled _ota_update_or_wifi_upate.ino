
#include <ESP8266WebServer.h>
#include <ESP8266mDNS.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>
#include <Adafruit_NeoPixel.h>
#ifdef __AVR__
#include <avr/power.h> 
#endif
#define LED_PIN D4

#define LED_COUNT 50
#define sensorPin D7
ESP8266WebServer server(80);
int red = 255;
int green = 128;
int blue = 0;  
int volume =1000;  
int pass = 876543210;               

int i=0;
bool c1 = true;
bool c2 = false;
// Declare our NeoPixel strip object:
Adafruit_NeoPixel strip(LED_COUNT, LED_PIN, NEO_GRB + NEO_KHZ800);

const char MAIN_page[] PROGMEM = R"=====(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1.0"><script>window.onload=function(){const urlParams=new URLSearchParams(window.location.search); const color=urlParams.get('head1');  if(color ) {
        document.getElementById("color").value = color;
        hexTorgb(color);
        } else {
            hexTorgb('#f2bb30');
        }};function hexTorgb(hex){document.getElementById("red").value='0x' + hex[1] + hex[2] | 0; document.getElementById("green").value='0x' + hex[3] + hex[4] | 0; document.getElementById("blue").value='0x' + hex[5] + hex[6] | 0;}function getRGBColor(that){let rgbV=hexTorgb(that.value);}</script><style>h1,.picker{color:white;}label{display: block;}.red{color:red;}.green{color:green;}.blue{color:blue;}body{background-color:#222;}input[type="color"]{margin: 8px 0; -webkit-appearance: none; -webkit-appearance: none; width: 105px; height: 41px; margin: 8px 0; border: 1px 2 lime; border-radius: 12px; box-sizing: border-box; background: #2a2a2a; padding: 7px;}input[type="color"]::-webkit-color-swatch-wrapper{padding: 0;}input[type="color"]::-webkit-color-swatch{border: none;}input[type=number], select{padding: 12px 20px; margin: 8px 0; border: 1px solid #ccc; border-radius: 12px; color: white; box-sizing: border-box; background: #2a2a2a;}input[type=submit]{width: 200px; display: block; background-color: #4CAF50; color: white; padding: 14px 20px; margin: auto 0;margin-top: 28px; border: none; border-radius: 4px; cursor: pointer; height: 39px;}input[type=submit]:hover{background-color: #45a049;}.di{border-radius: 5px; background-color: #1f1f1f; padding: 20px; display: flex; flex-wrap: wrap; gap:5px}.pi{width: unset;}.topnav{background-color: #333; overflow: hidden;}.topnav a{float: left; color: #f2f2f2; text-align: center; padding: 14px 16px; text-decoration: none; font-size: 17px;}.topnav a:hover{background-color: #ddd; color: black;}.topnav a.active{ background-color: #1d1d1d; color: white;}@media (max-width: 600px){.pi{width: 100%;}input[type=number]{width: 100%;}}</style></head><body><div class="topnav"> <a class="active">Picker</a> <a href="/music">Music</a>  <a href="/mode?head1=%23000000&color1=%23000000&modes=colorWipe&red=0&green=0&blue=0&red1=0&green1=0&blue1=0&delay=0">Mode</a>
      <a  href = "/ota" >ota</a></div><form action="/action_page"><div class="di"><div class="pi"> <label class="picker" >picker </label> <input type="color" id="color" name="head1" value="rgb(243 22 22)" onchange="getRGBColor(this);this.form.submit();"> </div><div class="pi"> <label class="red" >Red</label> <input type="number" id="red" name="red" readonly> </div><div class="pi"> <label class="green" >Green</label> <input type="number" id="green" name="green" value="" readonly> </div><div class="pi"> <label class="blue">Blue</label> <input type="number" id="blue" name="blue" value="" readonly> </div>   </div></form> </body></html>
)=====";
const char Music[] PROGMEM = R"=====(
<!DOCTYPE html><html> <head> <meta name="viewport" content="width=device-width, initial-scale=1.0"/>  <script>
       window.onload = function () {
    const urlParams = new URLSearchParams(window.location.search);
    const slider = urlParams.get("slider");
    if (slider){
      document.getElementById("slider").value = slider;
    }
       }
      
       function getValue() {
    location.search = '?slider=' + document.getElementById("slider").value ;
       }
       
    </script><style>body{background-color: #222;}.main{display: flex; justify-content: center; align-items: center; min-height: 80vh;}.music{width: 60px; height: 60px; border-width: 20px 10px 0px 10px; border-style: solid; border-color: crimson; position: relative;}.music::after, .music::before{content: ""; position: absolute; width: 30px; height: 30px; background-color: crimson; border-radius: 50px 0px 50px 50px;}.music::after{left: -30px; bottom: -22px;}.music::before{right: -10px; bottom: -22px;}.topnav{background-color: #333; overflow: hidden;}.topnav a{float: left; color: #f2f2f2; text-align: center; padding: 14px 16px; text-decoration: none; font-size: 17px;}.topnav a:hover{background-color: #ddd; color: black;}.topnav a.active{background-color: #1d1d1d; color: white;}.centre{display: flex;justify-content: center;}.setSlider{padding-top: 100px;margin-left: -60px;}</style> </head> <body> <div class="topnav"> <a href="/">Picker</a> <a class="active">Music</a> <a href="/mode?head1=%23000000&color1=%23000000&modes=colorWipe&red=0&green=0&blue=0&red1=0&green1=0&blue1=0&delay=0" >Mode</a > 
      <a  href = "/ota" >ota</a></div><div class="main"><div class="music"> <div class="setSlider"> <input type="range" id="slider" min="500" max="2000" value="500" onchange="getValue()"> </div></div></div></body></html>
)=====";

const char Mode[] PROGMEM =
    R"=====(
<!DOCTYPE html><html><head><meta name="viewport" content="width=device-width, initial-scale=1.0"><meta name="viewport" content="width=device-width, initial-scale=1.0"/><script>window.onload=function (){const urlParams=new URLSearchParams(window.location.search); const color=urlParams.get("head1"); const color1=urlParams.get("color1"); if (urlParams.get("modes")){document.getElementById("modes").value=urlParams.get("modes");}if (urlParams.get("delay")){document.getElementById("delay").value=urlParams.get("delay");}document.getElementById("color").value=color; document.getElementById("color1").value=color1; if (color){hexTorgb(color);}if (color1){hexTorgb1(color1);}}; function hexTorgb(hex){document.getElementById("red").value=("0x" + hex[1] + hex[2]) | 0; document.getElementById("green").value=("0x" + hex[3] + hex[4]) | 0; document.getElementById("blue").value=("0x" + hex[5] + hex[6]) | 0;}function hexTorgb1(hex){document.getElementById("red1").value=("0x" + hex[1] + hex[2]) | 0; document.getElementById("green1").value=("0x" + hex[3] + hex[4]) | 0; document.getElementById("blue1").value=("0x" + hex[5] + hex[6]) | 0;}function getRGBColor(that){hexTorgb(that.value);}function getRGBColor1(that){hexTorgb1(that.value);}</script><style>body{background-color: #222 ;}.topnav{background-color: #333; overflow: hidden;}.topnav a{float: left; color: #f2f2f2; text-align: center; padding: 14px 16px; text-decoration: none; font-size: 17px;}.topnav a:hover{background-color: #ddd; color: black;}.topnav a.active{background-color: #1d1d1d; color: white;}input[type="color"]{margin: 8px 0; -webkit-appearance: none; -webkit-appearance: none; width: 105px; height: 41px; margin: 8px 0; border: 1px 2 rgb(56, 56, 56); border-radius: 12px; box-sizing: border-box; background: #2a2a2a; padding: 7px;}input[type="color"]::-webkit-color-swatch-wrapper{padding: 0;}input[type="color"]::-webkit-color-swatch{border: none;}.di{border-radius: 5px; background-color: #1f1f1f; padding: 20px; display: flex; flex-wrap: wrap; gap: 5px;}.pi{width: unset;}.picker,.pi{color: white;}label{display: block;}input[type="number"], select{padding: 12px 20px; margin: 8px 0; border: 1px solid #ccc; border-radius: 12px; color: white; box-sizing: border-box; background: #2a2a2a;}input[type="submit"]{width: 200px; display: block; background-color: #0d4aa7; color: white; padding: 14px 20px; margin: auto 0; margin-top: 28px; border: none; border-radius: 4px; cursor: pointer; height: 39px;}input[type="submit"]:hover{background-color: #1f82c4;}@media (max-width: 600px){.pi{width: 100%;}}</style></head><body><div class="topnav"> <a href="/">Picker</a> <a href="/music">Music</a> <a class="active">Mode</a>
      <a  href = "/ota" >ota</a></div><form action="/mode"> <div class="di"> <div class="pi"> <label class="picker">picker </label> <input type="color" id="color" name="head1" value="rgb(243 22 22)" onchange="getRGBColor(this)"/> </div><div class="pi"> <label class="picker">picker2 </label> <input type="color" id="color1" name="color1" value="rgb(243 22 22)" onchange="getRGBColor1(this)"/> </div><div class="pi"> <label for="cars">Choose a Mode</label> <select name="modes" id="modes"> <option value="rainbow">rainbow</option> <option value="RandomChase">RandomChase</option> <option value="Chase">Chase</option> <option value="colorWipe">colorWipe</option> </select> </div><input type="number" id="red" name="red" value=0 hidden/> <input type="number" id="green" name="green" value=0 hidden/> <input type="number" id="blue" name="blue" value=0 hidden/> <input type="number" id="red1" name="red1" value=0 hidden/> <input type="number" id="green1" name="green1" value=0 hidden/> <input type="number" id="blue1" name="blue1" value=0 hidden/> <div class="pi"> <label class="blue">Delay</label> <input type="number" id="delay" name="delay" value="0"/> </div><input type="submit" value="Submit"> </div></form></body></html>
)=====";
const char otaHtml [] PROGMEM = R"=====(<!DOCTYPE html><html> <head> <meta name="viewport" content="width=device-width, initial-scale=1.0"/> <script>window.onload=function (){const urlParams=new URLSearchParams(window.location.search); const slider=urlParams.get("slider"); if (slider){document.getElementById("slider").value=slider;}}function getValue(){location.search='?slider=' + document.getElementById("slider").value ;}</script> <style>body{background-color: #222;}.main{display: flex; justify-content: center; align-items: center; min-height: 50vh;}.arrow{width: 120px; justify-content: center; display: flex; flex-wrap: wrap;}.line{margin-top: 14px; width: 200px; background: white; height: 35px;}.point{width: 0; height: 0; border-left: 120px solid transparent; border-right: 120px solid transparent; border-bottom: 116px solid white;}.topnav{background-color: #333; overflow: hidden;}.topnav a{float: left; color: #f2f2f2; text-align: center; padding: 14px 16px; text-decoration: none; font-size: 17px;}.topnav a:hover{background-color: #ddd; color: black;}.topnav a.active{background-color: #1d1d1d; color: white;}.centre{display: flex;justify-content: center;color: white;}input[type="number"], select{padding: 12px 20px; margin: 8px 0; border: 1px solid #ccc; border-radius: 12px; color: white; box-sizing: border-box; background: #2a2a2a;}input[type="submit"]{width: 200px; display: block; background-color: #0d4aa7; color: white; padding: 14px 20px; margin: auto 0; margin-top: 28px; border: none; border-radius: 4px; cursor: pointer; height: 39px;}input[type="submit"]:hover{background-color: #1f82c4;}.di{border-radius: 20px; border: 1px solid; padding: 20px;}</style> </head> <body> <div class="topnav"> <a href="/">Picker</a> <a href="/music" >Music</a> <a href="/mode?head1=%23000000&color1=%23000000&modes=colorWipe&red=0&green=0&blue=0&red1=0&green1=0&blue1=0&delay=0" >Mode</a > <a class="active">ota</a> </div><div class="main"><div class="arrow"> <div class="point"></div><div class="line"></div><div class="line"></div><div class="line"></div></div></div><div class="centre"> <form action="/ota"> <div class="di"> <div class="pi"> <label class="blue">Enter the password : </label> <input type="number" id="password" name="pass" value="0"/> </div><input type="submit" value="Submit"> </div></form></div></body></html>)=====";
const char otaMode[] PROGMEM = R"=====(<!DOCTYPE html><html> <head> <meta name="viewport" content="width=device-width, initial-scale=1.0"/> <script></script> <style>body{background-color: #222;}.main{display: flex; justify-content: center; align-items: center; min-height: 80vh;}.arrow{width: 120px; justify-content: center; display: flex; flex-wrap: wrap;}.line{margin-top: 14px; width: 200px; background: white; height: 35px;}.point{width: 0; height: 0; border-left: 120px solid transparent; border-right: 120px solid transparent; border-bottom: 116px solid white;}.centre{display: flex;justify-content: center;color: white;}</style> </head> <body> <div class="main"><div class="arrow"> <div class="point"></div><div class="line"></div><div class="line"></div><div class="line"></div></div></div><div class="centre"> update mode on don't restart the system</div></body></html>)=====";
const char *ssid = "samsung2";
const char *password = "step81570";

int valuedelay = 0;
int sampleBufferValue = 0;
int signalvalue = 0;
uint32_t colors;
uint32_t colors2;
int delays = 10;

const int SAMPLE_TIME = 115;
int  time_elapsed =0;
unsigned long millisCurrent;
unsigned long millisLast = 0;
unsigned long millisElapsed = 0;

bool rec = true;
bool lig = false;
bool mus = false;
bool mod = false;
bool ota = false;

String modes;


int long mapfn(long x, long in_min, long in_max, long out_min, long out_max)
{
  return (x - in_min) * (out_max - out_min) / (in_max - in_min) + out_min;
}

void handleRoot() {
  lig = false;
  mus = false;
  mod = false;
  String  s= MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}
void handleRootint() {
  lig = false;
  mus = false;
  mod = false;
  String  s= MAIN_page; //Read HTML contents
 server.send(200, "text/html", s); //Send web page
}

void theaterChase(uint32_t color, int wait) {
  for(int a=0; a<10; a++) {  
    for(int b=0; b<3; b++) { 
      strip.clear();         
     
      for(int c=b; c<strip.numPixels(); c += 3) {

        strip.setPixelColor(c, color); 
      }
      strip.show(); 
      delay(wait);  
    }
  }
}
void colorWipeMusic(uint32_t color)
{
  for (int i = 2; i < signalvalue; i++)
  {
    strip.setPixelColor(i, color);
    strip.setPixelColor((42 - i), color);
    strip.show();
    delay(valuedelay);
  }
}
void RGBColor(int Rcolor, int Gcolor, int Bcolor)
{
  valuedelay = 10;
  colorWipeMusic(strip.Color(Rcolor, Gcolor, Bcolor));
  signalvalue = 50;
  valuedelay = 0;
  colorWipeMusic(strip.Color(0, 0, 0));
}

void colourSelection()
{

  if (signalvalue == 0)
  {
    RGBColor(0, 0, 0);
  }
  else
  {
    switch (random(10))
    {
    case 1:
      RGBColor(0, 0, 255);
      break;
    case 2:
      RGBColor(0, 255, 255);
      break;
    case 3:
      RGBColor(0, 127, 255);
      break;
    case 4:
      RGBColor(0, 255, 127);
      break;
    case 5:
      RGBColor(0, 255, 0);
      break;
    case 6:
      RGBColor(255, 0, 255);
      break;
    case 7:
      RGBColor(255, 255, 0);
      break;
    case 8:
      RGBColor(255, 0, 127);
      break;
    case 9:
      RGBColor(255, 0, 0);
      break;
    case 10:
      RGBColor(0, 0, 255);
      break;
    default:
      RGBColor(255, 127, 0);
      break;
    }
  }
}

void sound()
{
  millisCurrent = millis();

  millisElapsed = millisCurrent - millisLast;
  if (digitalRead(sensorPin) == LOW)
  {
    sampleBufferValue++;
  }

  if (millisElapsed > SAMPLE_TIME)
  {
    millisLast = millisCurrent;
    signalvalue = sampleBufferValue;
    signalvalue = mapfn(signalvalue, 0, volume, 0, 21);
    colourSelection();
    sampleBufferValue = 0;
  }
}



void colorWipe(uint32_t color, int wait)
{
  for (int i = 0; i < strip.numPixels(); i++)
  {
    strip.setPixelColor(i, color);
    strip.show();
    delay(wait);
  }
}

void music()
{ lig = false;
  mus = true;
  mod = false;
  ota = false;
  if (server.arg("slider").toInt() ) {
    volume = server.arg("slider").toInt();
  } else {
    volume = 1000;
  }
  String s = Music;
  server.send(200, "text/html", s);
}


void ledMode()
{
  red = server.arg("red").toInt();
  green = server.arg("green").toInt();
  blue = server.arg("blue").toInt();
  const int red1 =server.arg("red1").toInt();
   const int green1 = server.arg("green1").toInt();
   const int blue1 = server.arg("blue1").toInt();
  delays  =  server.arg("delay").toInt();
  modes = server.arg("modes");
  lig = false;
  mus = false;
  ota = false;
  mod = true;
  i = 0;
  String s = Mode;
  server.send(200, "text/html", s);
    colors = strip.Color(green, red, blue);
  colors2 =  strip.Color(green1, red1, blue1);
  
}
void handleForm()
{ 
  red = server.arg("red").toInt();
  green = server.arg("green").toInt();
  blue = server.arg("blue").toInt();
  colorWipe(strip.Color(green, red, blue), 0);
  handleRoot();
}
void otaHandle()
{ lig = false;
  mus = false;
  mod = false;
  if (server.arg("pass").toInt() == pass ) {
    ota = true;
     time_elapsed = 0;
     colorWipe(strip.Color(255, 0, 255), 0);
  String s = otaMode;
  server.send(200, "text/html", s);
  } else {
    ota = false;
    colorWipe(strip.Color(0, 0, 255), 0);
    String s = otaHtml;
    server.send(200, "text/html", s);
  }
}
void request()
{
  server.on("/",  handleRootint);
  server.on("/action_page", handleForm);
  server.on("/music", music);
  server.on("/mode", ledMode);
   server.on("/ota", otaHandle);
}

void setup(void)
{
  strip.begin();            // INITIALIZE NeoPixel strip object (REQUIRED)
  strip.show();             // Turn OFF all pixels ASAP
  strip.setBrightness(255); // Set BRIGHTNESS to about 1/5 (max = 255)   
 // WiFi.softAP("led",""); To turn on wifi
  WiFi.mode(WIFI_STA);
  WiFi.begin(ssid, password); //Connect to your WiFi router

  while (WiFi.status() != WL_CONNECTED)
  {
    colorWipe(strip.Color(green, red, blue), 0);
    delay(500);
    colorWipe(strip.Color(0, 0, 0), 0);
  }
  
  Serial.print("IP address: ");
WiFi.setAutoReconnect(true);
WiFi.persistent(true);
MDNS.begin("led");
  request();
  server.begin();
  // These lines are specifically to support the Adafruit Trinket 5V 16 MHz.
  // Any other board, you can remove this part (but no harm leaving 
// #if defined(__AVR_ATtiny85__) && (F_CPU == 16000000)
 // clock_prescale_set(clock_div_1);
//#endif
 ArduinoOTA.setHostname("ledesp");
 // ArduinoOTA.setPassword("admin");
 ArduinoOTA.setPasswordHash("94ce92540b9e31cca304d8bd6d7fd65d");
 
 ArduinoOTA.onStart([]() {
    String type;
    if (ArduinoOTA.getCommand() == U_FLASH) {
      type = "sketch";
    } else { // U_FS
      type = "filesystem";
    }
     });
   ArduinoOTA.onEnd([]() {
    colorWipe(strip.Color(255, 255, 0), 0);
  });
 ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
  int loading = progress / (total / LED_COUNT);
    if(loading <  LED_COUNT ) {
      strip.setPixelColor(loading, strip.Color(0, 255, 255));
       strip.show();
    } else {
      colorWipe(strip.Color(255, 0, 0), 0);
    }
    
  });
   ArduinoOTA.onError([](ota_error_t error) {
    if (error) {
     colorWipe(strip.Color(0, 255, 0), 0);
    } 
  });
  ArduinoOTA.begin();
  colorWipe(strip.Color(green, red, blue), 0);
  
}

void loop(void)
{
if (ota) { 
   ArduinoOTA.handle();
}else {
  server.handleClient(); 
  if(mod) {
    if(modes == "rainbow") {
       signalvalue = 50;
    colourSelection();
    }
if(modes == "RandomChase") {
       if( i >= 50)
      {
        i=0;
        }
      strip.setPixelColor(i, colors);
      delay(delays );
      strip.setPixelColor(i+1, colors2);
     strip.setPixelColor(i+2, colors2);
      strip.setPixelColor(i+3, colors2);
       delay(delays );
      strip.show();
      i = i+1;
    }
    
    
    if(modes == "Chase") {
    theaterChase(colors,  delays);
    }
    if(modes == "colorWipe" ) {
    
    if(c1) {
    strip.setPixelColor(i, colors2);
    if(i == 50) {
      i = 0;
      c1 = false;
      c2 = true;
    }
    
    }
    
    if(c2) {
     strip.setPixelColor(i, colors);
    if(i == 50) {
      i = 0;
      c2 = false;
      c1 = true;
       strip.setPixelColor(0, colors2);
    }
   
    }
    strip.show();
    delay(delays );
    i++;
    }
  }
  if(mus) {
       sound();
  }
}
}
