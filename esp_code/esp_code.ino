/*
 * ESPloitV2
 * WiFi controlled HID Keyboard Emulator
 * By Corey Harding of www.Exploit.Agency / www.LegacySecurityGroup.com
 * Special thanks to minkione for helping port/test original V1 code to the Cactus Micro rev2
 * ESPloit is distributed under the MIT License. The license and copyright notice can not be removed and must be distributed alongside all future copies of the software.
 * MIT License
    
    Copyright (c) [2017] [Corey Harding]
    
    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:
    
    The above copyright notice and this permission notice shall be included in all
    copies or substantial portions of the Software.
    
    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
    SOFTWARE.
*/

#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include "ESP8266FtpServer.h"
#include "license.h"

#define BAUD_RATE 115200

// #########################################
// ############ Configuration ##############
// #########################################

const char *ssid = "BadUSB_Test";
const char *password = "badUSBWifi";
const char *ftp_username = "esp8266";
const char *ftp_password = "esp8266";

IPAddress local_IP(192,168,1,1);
IPAddress gateway(192,168,1,1);
IPAddress subnet(255,255,255,0);

int DelayLength = 1000;
String webString;
char autopayload[64];
int livepayloaddelay;
char local_IPstr[16];

File fsUploadFile;

ESP8266WebServer server(80);
FtpServer ftpSrv;

const String HTML_CSS_STYLING = "<html><head><title>BadUsb_PControl</title><meta charset=\"utf-8\"><style>#header{text-align: center;}#menu{margin:0 auto; padding:0; height:30px; width:100%; display:block;}#menu li{padding:0; margin:0; list-style:none; display:inline;}.myButton {-moz-box-shadow:inset 0px 0px 14px -3px #f2fadc;-webkit-box-shadow:inset 0px 0px 14px -3px #f2fadc;box-shadow:inset 0px 0px 14px -3px #f2fadc;background:-webkit-gradient(linear, left top, left bottom, color-stop(0.05, #dbe6c4), color-stop(1, #9ba892));background:-moz-linear-gradient(top, #dbe6c4 5%, #9ba892 100%);background:-webkit-linear-gradient(top, #dbe6c4 5%, #9ba892 100%);background:-o-linear-gradient(top, #dbe6c4 5%, #9ba892 100%);background:-ms-linear-gradient(top, #dbe6c4 5%, #9ba892 100%);background:linear-gradient(to bottom, #dbe6c4 5%, #9ba892 100%);filter:progid:DXImageTransform.Microsoft.gradient(startColorstr='#dbe6c4', endColorstr='#9ba892',GradientType=0);background-color:#dbe6c4;-moz-border-radius:6px;-webkit-border-radius:6px;border-radius:6px;border:1px solid #b2b8ad;display:inline-block;cursor:pointer;color:#000000;font-family:Arial;font-size:15px;font-weight:bold;padding:6px 24px;text-decoration:none;text-shadow:0px 1px 0px #ced9bf;}body {background-color: black;color:white;}h1 {color:#C4A000;}#mousePanel{text-align:center;}#mousePanel table{background-color:gray;borde:1;border-color:#C4A000;}#Teclado{padding:2%;}#sendText{text-align:center;}.Line {padding-top:2px;background-color:#C4A000;}hr{color:#C4A000;}</style></head><body><div id=\"header\"><h1>BadUSB - WiFi Panel Control<h1><div id=\"menu\"><ul><li><a class=\"myButton\" href=\"/virtualkeyboard\">Virtual Keyboard</a></li><li><a class=\"myButton\" href=\"/physicalkeyboard\">Physical Keyboard</a></li><li><a class=\"myButton\" href=\"/livepayload\">Live Payload</a></li><li><a class=\"myButton\" href=\"/uploadpayload\">Upload Payload</a></li><li><a class=\"myButton\" href=\"/listpayloads\">Choose Payload</a></li></div></div><hr>";
const String HTML_BACK_TO_INDEX = "<a href=\"/\"><- BACK TO INDEX</a><br><br>";

void runpayload() {
    File f = SPIFFS.open(autopayload, "r");
    int defaultdelay = DelayLength;
    int settingsdefaultdelay = DelayLength;
    int custom_delay;
    delay(livepayloaddelay);
    while(f.available()) {

      String line = f.readStringUntil('\n');
      line.replace("&lt;", "<");

      String fullkeys = line;
      int str_len = fullkeys.length()+1; 
      char keyarray[str_len];
      fullkeys.toCharArray(keyarray, str_len);

      char *i;
      String cmd;
      String cmdinput;
      cmd = String(strtok_r(keyarray," ",&i));
      
      delay(DelayLength);
      DelayLength = defaultdelay;
    }
    f.close();
    DelayLength = settingsdefaultdelay;
}

void handleFileUpload()
{
  if(server.uri() != "/upload") return;
  HTTPUpload& upload = server.upload();
  if(upload.status == UPLOAD_FILE_START){
    String filename = upload.filename;
    filename = "/payloads/"+filename;
    fsUploadFile = SPIFFS.open(filename, "w");
    filename = String();
  }
  else if(upload.status == UPLOAD_FILE_WRITE){
    if(fsUploadFile)
    fsUploadFile.write(upload.buf, upload.currentSize);
  }
  else if(upload.status == UPLOAD_FILE_END){
    if(fsUploadFile)
    fsUploadFile.close();
  }
}

void ListPayloads(){
  String FileList = HTML_CSS_STYLING + "<h><b>Choose Payload:</b></h><br><br>";
  Dir dir = SPIFFS.openDir("/payloads");
  while (dir.next()) {
    String FileName = dir.fileName();
    File f = dir.openFile("r");
    FileList += " ";
    FileList += "<a href=\"/showpayload?payload="+FileName+"\">"+FileName+"</a>"+"<br><br></body></html>";
  }
  server.send(200, "text/html", FileList);
}

void setup() {

  Dir dir;
  
  Serial.begin(115200);
  SPIFFS.begin();

  if (SPIFFS.begin()){
    ftpSrv.begin(ftp_username,ftp_password);
    dir = SPIFFS.openDir("/");
  }

  while (dir.next()) {
    String fileName = dir.fileName();
    size_t fileSize = dir.fileSize();
    Serial.printf("FS File: %s, size: %s\n", fileName.c_str(), String(fileSize).c_str());
    Serial.println();
  }

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_IP, gateway,subnet);

  server.serveStatic("/", SPIFFS, "/virtualkeyboard.html");
  server.serveStatic("/virtualkeyboard", SPIFFS, "/virtualkeyboard.html");
  server.serveStatic("/physicalkeyboard", SPIFFS, "/physicalkeyboard.html");
  server.serveStatic("/livepayload", SPIFFS, "/livepayload.html");
  server.serveStatic("/uploadpayload", SPIFFS, "/uploadpayload.html");
  server.serveStatic("/upload", SPIFFS, "/upload.html");
  server.serveStatic("/format", SPIFFS, "/format.html");
  server.serveStatic("/format/yes", SPIFFS, "/formatyes.html");
  server.on("/listpayloads", ListPayloads);
  server.onFileUpload(handleFileUpload);

  server.on("/runlivepayload", [](){
    String livepayload;
    livepayload += server.arg("livepayload");
    if (server.hasArg("livepayloadpresent")) {
      server.send(200, "text/html", "<pre>Running live payload: <br>"+livepayload+"</pre>");
      char* splitlines;
      int payloadlen = livepayload.length()+1;
      char request[payloadlen];
      livepayload.toCharArray(request,payloadlen);
      splitlines = strtok(request,"\r\n");
      int defaultdelay = DelayLength;
      int settingsdefaultdelay = DelayLength;
      int custom_delay;
      delay(livepayloaddelay);
      while(splitlines != NULL)
      {
         String liveline=splitlines;
         liveline.replace("&lt;", "<");
         char *i;
         String cmd;
         String cmdinput;
         cmd = String(strtok_r(splitlines," ",&i));
         
         splitlines = strtok(NULL,"\r\n");       

         Serial.println(liveline);        

         delay(DelayLength);
         DelayLength = defaultdelay;  
      }
      DelayLength = settingsdefaultdelay;
      return 0;
    }
    else {
      server.send(200, "text/html", F("Type or Paste a payload and click \"Run Payload\"."));
    }
  });
    
  server.on("/showpayload", [](){
    webString="";
    String payload;
    payload += server.arg(0);
    File f = SPIFFS.open(payload, "r");
    String webString = f.readString();
    f.close();
    server.send(200, "text/html", HTML_CSS_STYLING + "<a href=\"/dopayload?payload="+payload+"\"><button>Run Payload</button></a><h2><pre>"+payload+"\n-----\n"+webString+"</pre></h2></body></html>");
    webString="";
  });

  server.on("/dopayload", [](){
    String dopayload;
    dopayload += server.arg(0);
    server.send(200, "text/html", HTML_CSS_STYLING + "<pre><h3>Running payload: "+dopayload+"</h3></pre><br></body></html>");
    File f = SPIFFS.open(dopayload, "r");
    while(f.available()) {
      String line = f.readStringUntil('\n');
      Serial.println(line);
      delay(DelayLength); //delay between lines in payload, I found running it slower works best
    }
    f.close();
  });
    
  server.begin();
}

void loop(void) {
  server.handleClient();
  ftpSrv.handleFTP();
}
