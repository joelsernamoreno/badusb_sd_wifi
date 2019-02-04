#include <Keyboard.h>
#include <SPI.h>
#include <SD.h>

int i;
char myChar;
char SomeKey;
char *tmp_string2;
char command[255];

#define DEBUG_SERIAL Serial

File myFile;

void setup() {

  //Start up the ESP 8266
  pinMode(13, OUTPUT);
  digitalWrite(13, HIGH);

  Serial1.begin(115200);
  Keyboard.begin();

  if (!SD.begin(4)) {
  }
}

void loop() {

  while (Serial1.available()) {
    myChar = Serial1.read();

    if ((myChar == (13)) || (myChar == '\n')) {
      command[i] = '\x00';
      if (strlen(command) > 0) {
        delay(500);
        ProcessCommand(command);
      }
      i = 0;
    }
    else
    {
      command[i] = myChar;
      i++;
    }
  }
}

char ProcessCommand(char* command) {

  // Runwin
  if (str_start(command, "runwin")) {
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.print('r');
    delay(100);
    Keyboard.releaseAll();
  }

  // Rungnome
  if (str_start(command, "rungnome")) {
    Keyboard.press(KEY_LEFT_ALT);
    Keyboard.press(KEY_F2);
    delay(100);
    Keyboard.releaseAll();
  }

  // Runmac
  if (str_start(command, "runmac")) {
    Keyboard.press(KEY_LEFT_GUI);
    Keyboard.print(' ');
    delay(100);
    Keyboard.releaseAll();
  }

  // Exec script1.txt (example: execSD_script1)
  if (str_start(command, "execSD_script1")) {
    myFile = SD.open("script1.txt");
    delay(500);
    ExecScript();
  }

  // Exec scrip2.txt (example: execSD_script2)
  if (str_start(command, "execSD_script2")) {
    myFile = SD.open("script2.txt");
    delay(500);
    ExecScript();
  }

  // Exec script3.txt (example: execSD_script3)
  if (str_start(command, "execSD_script3")) {
    myFile = SD.open("script3.txt");
    delay(500);
    ExecScript();
  }

  // Delay X (example: delay 1000)
  if (str_start(command, "delay ")) {
    delay(atoi(command + 6));
  }

  // ReleaseAll (example: release)
  if (str_start(command, "release")) {
    delay(25);
    Keyboard.releaseAll();
  }

  // RawPress (example: rawpress KEY_RETURN)
  if (str_start(command, "rawpress ")) {
    tmp_string2 = command + 9;
    delay(150);
    ProcessTmpString();
    delay(200);
    Keyboard.print(SomeKey);
    SomeKey = 0;
  }

  // Press (example: press KEY_RETURN)
  if (str_start(command, "press ")) {
    tmp_string2 = command + 6;
    delay(150);
    ProcessTmpString();
    delay(200);
    Keyboard.press(SomeKey);
    SomeKey = 0;
  }

  // Print (example: print Hello World!)
  if (str_start(command, "print ")) {
    delay(50);
    Keyboard.print(command + 6);
  }

  // Println (example: println Hello World!)
  else if (str_start(command, "println ")) {
    delay(50);
    Keyboard.println(command + 8 );
  }
}

void ExecScript(void) {

  if (myFile) {
    i = 0;

    while (myFile.available()) {
      myChar = myFile.read();

      if ((myChar == (13)) || (myChar == '\n')) {
        command[i] = '\x00';
        if (strlen(command) > 0) {
          delay(500);
          ProcessCommand(command);
        }
        i = 0;
      }
      else
      {
        command[i] = myChar;
        i++;
      }
    }
    myFile.close();
  }
}

void ProcessTmpString(void) {

  int i;

  for (i = 0; tmp_string2[i] != 0; i++) {
  }

  if (!strcmp(tmp_string2, "KEY_RETURN")) SomeKey = 0xB0;
  if (!strcmp(tmp_string2, "KEY_LEFT_CTRL")) SomeKey = 0x80;
  if (!strcmp(tmp_string2, "KEY_LEFT_SHIFT")) SomeKey = 0x81;
  if (!strcmp(tmp_string2, "KEY_LEFT_ALT")) SomeKey = 0x82;
  if (!strcmp(tmp_string2, "KEY_LEFT_GUI")) SomeKey = 0x83;
  if (!strcmp(tmp_string2, "KEY_RIGHT_CTRL")) SomeKey = 0x84;
  if (!strcmp(tmp_string2, "KEY_RIGHT_SHIFT")) SomeKey = 0x85;
  if (!strcmp(tmp_string2, "KEY_RIGHT_ALT")) SomeKey = 0x86;
  if (!strcmp(tmp_string2, "KEY_RIGHT_GUI")) SomeKey = 0x87;
  if (!strcmp(tmp_string2, "KEY_UP_ARROW")) SomeKey = 0xDA;
  if (!strcmp(tmp_string2, "KEY_DOWN_ARROW")) SomeKey = 0xD9;
  if (!strcmp(tmp_string2, "KEY_LEFT_ARROW")) SomeKey = 0xD8;
  if (!strcmp(tmp_string2, "KEY_RIGHT_ARROW")) SomeKey = 0xD7;
  if (!strcmp(tmp_string2, "KEY_BACKSPACE")) SomeKey = 0xB2;
  if (!strcmp(tmp_string2, "KEY_TAB")) SomeKey = 0xB3;
  if (!strcmp(tmp_string2, "KEY_ESC")) SomeKey = 0xB1;
  if (!strcmp(tmp_string2, "KEY_INSERT")) SomeKey = 0xD1;
  if (!strcmp(tmp_string2, "KEY_DELETE")) SomeKey = 0xD4;
  if (!strcmp(tmp_string2, "KEY_PAGE_UP")) SomeKey = 0xD3;
  if (!strcmp(tmp_string2, "KEY_PAGE_DOWN")) SomeKey = 0xD6;
  if (!strcmp(tmp_string2, "KEY_HOME")) SomeKey = 0xD2;
  if (!strcmp(tmp_string2, "KEY_END")) SomeKey = 0xD5;
  if (!strcmp(tmp_string2, "KEY_CAPS_LOCK")) SomeKey = 0xC1;
  if (!strcmp(tmp_string2, "KEY_F1")) SomeKey = 0xC2;
  if (!strcmp(tmp_string2, "KEY_F2")) SomeKey = 0xC3;
  if (!strcmp(tmp_string2, "KEY_F3")) SomeKey = 0xC4;
  if (!strcmp(tmp_string2, "KEY_F4")) SomeKey = 0xC5;
  if (!strcmp(tmp_string2, "KEY_F5")) SomeKey = 0xC6;
  if (!strcmp(tmp_string2, "KEY_F6")) SomeKey = 0xC7;
  if (!strcmp(tmp_string2, "KEY_F7")) SomeKey = 0xC8;
  if (!strcmp(tmp_string2, "KEY_F8")) SomeKey = 0xC9;
  if (!strcmp(tmp_string2, "KEY_F9")) SomeKey = 0xCA;
  if (!strcmp(tmp_string2, "KEY_F10")) SomeKey = 0xCB;
  if (!strcmp(tmp_string2, "KEY_F11")) SomeKey = 0xCC;
  if (!strcmp(tmp_string2, "KEY_F12")) SomeKey = 0xCD;
}

int str_start(char*strlong, char const*strshort) {
  int i, j = 1;
  for (i = 0; i < strlen(strshort); i++) {
    if (strlong[i] != strshort[i]) j = 0;
  }
  return j;
}
