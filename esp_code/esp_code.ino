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
#include "virtualkeyboard.h"
#include "physicalkeyboard.h"
#include "license.h";

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

int DelayLength = 600;
String webString;
char autopayload[64];
int livepayloaddelay;
char local_IPstr[16];

File fsUploadFile;

ESP8266WebServer server(80);
FtpServer ftpSrv;

const String HTML_CSS_STYLING = "<html> <head>    <title>BadUsb_PControl</title>    <meta charset=\"utf-8\">      </head> <body>    <div id=\"header\">     <h1>BadUSB - WiFi Panel Control<h1>";
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
  String FileList = HTML_CSS_STYLING + HTML_BACK_TO_INDEX + "<h><b>Choose Payload:</b></h><br><br>";
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

  Serial.begin(115200);
  delay(2000);
  SPIFFS.begin();

  WiFi.mode(WIFI_AP);
  WiFi.softAP(ssid, password);
  WiFi.softAPConfig(local_IP, gateway,subnet);

    if (SPIFFS.begin()){
      ftpSrv.begin(ftp_username,ftp_password);
    } 

  server.on("/", []() {
    server.send_P(200, "text/html", Virtual);
  });

  server.on("/virtualkeyboard", []() {
    server.send_P(200, "text/html", Virtual);
  });

  server.on("/physicalkeyboard", [](){
    server.send_P(200, "text/html", Physical);    
  });

  server.on("/livepayload", [](){
    server.send(200, "text/html", HTML_CSS_STYLING + "<FORM action=\"/runlivepayload\" method=\"post\" id=\"live\" target=\"iframe\">Payload: <br><br><textarea style =\"width: 100%;\" form=\"live\" rows=\"4\" cols=\"50\" name=\"livepayload\"></textarea><br><br><br><br><br><br><INPUT type=\"radio\" name=\"livepayloadpresent\" value=\"1\" hidden=\"1\" checked=\"checked\"><INPUT type=\"submit\" value=\"Run Payload\"></form><br><hr><br><iframe style =\"visibility: hidden;\" src=\"http://\")+local_IPstr+\"/runlivepayload\" name=\"iframe\"></iframe></body></html>" + HTML_BACK_TO_INDEX);
  });

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

//         Serial.println(String()+"cmd:"+cmd);
//         Serial.println(String()+"cmdin:"+cmdinput);
         
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

  server.on("/uploadpayload", []() {
    server.send(200, "text/html", HTML_CSS_STYLING + "<b><h2>Upload Payload:</h2></b><br><br><form method='POST' action='/upload' enctype='multipart/form-data'><input type='file' name='upload'><input type='submit' value='Upload'></form><br><br><div id=\"menu\">       <ul>          <li><a class=\"myButton\" href=\"/format\">Delete uploaded payloads</a></li><br></body></html>" + HTML_BACK_TO_INDEX);
  });
    
  server.on("/listpayloads", ListPayloads);
    
  server.onFileUpload(handleFileUpload);
    
  server.on("/upload", HTTP_POST, []() {
    server.send(200, "text/html", HTML_CSS_STYLING + "<h2>Upload Successful!</h2><br><br></body></html>" + HTML_BACK_TO_INDEX);
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

  server.on("/format", [](){
    server.send(200, "text/html", HTML_CSS_STYLING +  "<h2>You will delete all Payloads! Are you sure?</h2><br><br><div id=\"menu\">       <ul>          <li><a class=\"myButton\" href=\"/format/yes\">YES</a></li> - <li><a class=\"myButton\" href=\"/\">NO</a></li></div></body></html>" + HTML_BACK_TO_INDEX);
  });

  server.on("/format/yes", [](){
    server.send(200, "text/html", HTML_CSS_STYLING  + "<h2>Formatting file system: Reload Choose Payload Page</h2>" + HTML_BACK_TO_INDEX);
    SPIFFS.format();
  });

 /* server.on("/license", []() {
    server.send_P(200, "text/html", License);
  });*/

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
