/**
   This example turns the ESP32 into a Bluetooth LE keyboard that writes the words, presses Enter, presses a media key and then Ctrl+Alt+Delete
*/
#include <BleKeyboard.h>
#include <RotaryEncoder.h>
#include <PinButton.h>

const int PIN_ENCODER_A = 21;
const int PIN_ENCODER_B = 22;

RotaryEncoder encoder(PIN_ENCODER_A, PIN_ENCODER_B, RotaryEncoder::LatchMode::TWO03);
// This interrupt will do our encoder reading/checking!
void checkPosition() {
  encoder.tick(); // just call tick() to check the state.
}
int last_rotary = 0;

BleKeyboard bleKeyboard;

const int leftButtonPin = 19;
const int rightButtonPin = 17;
const int selectButton = 13;
const int upButtonPin = 18;
const int downButtonPin = 16;

//Four button on top
//const int buttonAPin = 25;
//const int buttonBPin = 26;
//const int buttonCPin = 27;
//const int buttonDPin = 14;

//debug LED
const int debugLEDPin = 33;

//battery reading
const int VBattPin = 32;


PinButton leftButton(leftButtonPin, INPUT);
PinButton downButton(downButtonPin, INPUT);
PinButton rightButton(rightButtonPin, INPUT);
PinButton upButton(upButtonPin, INPUT);

int lButtonState = 1;
int rButtonState = 1;
int selectButtonState = 1;
int upButtonState = 1;
int downButtonState = 1;
int lastState = 0;  // 0 nothing, 1 left, 2 right, 3 select, 4 up, 5 down

///////////////////////////////////////////////////////////////////////////////////////////////////////
//WEBSERVER START
#include <WiFi.h>
#include <WiFiClient.h>
#include <WebServer.h>
#include <ESPmDNS.h>
#include <Update.h>

const char* host = "esp32";
const char* ssid = "OLIN-ROBOTICS";
const char* password = "R0B0TS-RULE";

WebServer server(80);

/*
 * Login page
 */

const char* loginIndex =
 "<form name='loginForm'>"
    "<table width='20%' bgcolor='A09F9F' align='center'>"
        "<tr>"
            "<td colspan=2>"
                "<center><font size=4><b>ESP32 Login Page</b></font></center>"
                "<br>"
            "</td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
             "<td>Username:</td>"
             "<td><input type='text' size=25 name='userid'><br></td>"
        "</tr>"
        "<br>"
        "<br>"
        "<tr>"
            "<td>Password:</td>"
            "<td><input type='Password' size=25 name='pwd'><br></td>"
            "<br>"
            "<br>"
        "</tr>"
        "<tr>"
            "<td><input type='submit' onclick='check(this.form)' value='Login'></td>"
        "</tr>"
    "</table>"
"</form>"
"<script>"
    "function check(form)"
    "{"
    "if(form.userid.value=='admin' && form.pwd.value=='admin')"
    "{"
    "window.open('/serverIndex')"
    "}"
    "else"
    "{"
    " alert('Error Password or Username')/*displays error message*/"
    "}"
    "}"
"</script>";

/*
 * Server Index Page
 */

char* serverIndex =
"<script src='https://ajax.googleapis.com/ajax/libs/jquery/3.2.1/jquery.min.js'></script>"
"<form method='POST' action='#' enctype='multipart/form-data' id='upload_form'>"
   "<input type='file' name='update'>"
        "<input type='submit' value='Update'>"
    "</form>"
 "<div id='prg'>progress: 0%</div>"
 "<script>"
  "$('form').submit(function(e){"
  "e.preventDefault();"
  "var form = $('#upload_form')[0];"
  "var data = new FormData(form);"
  " $.ajax({"
  "url: '/update',"
  "type: 'POST',"
  "data: data,"
  "contentType: false,"
  "processData:false,"
  "xhr: function() {"
  "var xhr = new window.XMLHttpRequest();"
  "xhr.upload.addEventListener('progress', function(evt) {"
  "if (evt.lengthComputable) {"
  "var per = evt.loaded / evt.total;"
  "$('#prg').html('progress: ' + Math.round(per*100) + '%');"
  "}"
  "}, false);"
  "return xhr;"
  "},"
  "success:function(d, s) {"
  "console.log('success!')"
 "},"
 "error: function (a, b, c) {"
 "}"
 "});"
 "});"
 "</script>";

bool WiFiConnected = false;

void setup() {
  Serial.begin(115200);
  Serial.println("Starting BLE work!");
  bleKeyboard.setName("STEP Controller");
  bleKeyboard.begin();
  pinMode(downButtonPin, INPUT);
  pinMode(upButtonPin, INPUT);
  pinMode(leftButtonPin, INPUT);
  pinMode(rightButtonPin, INPUT);

  pinMode(selectButton, INPUT);
  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_A), checkPosition, CHANGE);
  attachInterrupt(digitalPinToInterrupt(PIN_ENCODER_B), checkPosition, CHANGE);

//  Serial.println("WifiStart");
//        // Connect to WiFi network
//          WiFi.begin(ssid, password);
//       
//          // Wait for connection for 3 secs
//          uint8_t wifiTimeout = 0;
//          while (WiFi.status() != WL_CONNECTED || wifiTimeout < 30) {
//            delay(100);
//            wifiTimeout++;
//          }
//
//          if(WiFi.status() == WL_CONNECTED) {
//            Serial.println("InINIT");
//            WiFiConnected = true;
//        
//            /*use mdns for host name resolution*/
//            if (!MDNS.begin(host)) { //http://esp32.local
//              while (1) {
//                delay(1000);
//              }
//            }
//            // Serial.println("mDNS responder started");
//            /*return index page which is stored in serverIndex */
//            server.on("/", HTTP_GET, []() {
//              server.sendHeader("Connection", "close");
//              server.send(200, "text/html", loginIndex);
//            });
//            server.on("/serverIndex", HTTP_GET, []() {
//              server.sendHeader("Connection", "close");
//              server.send(200, "text/html", serverIndex);
//            });
//            /*handling uploading firmware file */
//            server.on("/update", HTTP_POST, []() {
//              server.sendHeader("Connection", "close");
//              server.send(200, "text/plain", (Update.hasError()) ? "FAIL" : "OK");
//              ESP.restart();
//            }, []() {
//              HTTPUpload& upload = server.upload();
//              if (upload.status == UPLOAD_FILE_START) {
//                // Serial.printf("Update: %s\n", upload.filename.c_str());
//                if (!Update.begin(UPDATE_SIZE_UNKNOWN)) { //start with max available size
//                  Update.printError(Serial);
//                }
//              } else if (upload.status == UPLOAD_FILE_WRITE) {
//                /* flashing firmware to ESP*/
//                if (Update.write(upload.buf, upload.currentSize) != upload.currentSize) {
//                  Update.printError(Serial);
//                }
//              } else if (upload.status == UPLOAD_FILE_END) {
//                if (Update.end(true)) { //true to set the size to the current progress
//                  // Serial.printf("Update Success: %u\nRebooting...\n", upload.totalSize);
//                } else {
//                  Update.printError(Serial);
//                }
//              }
//            });
//            server.begin();
//          }
}

void loop() {
//  Serial.print("aaa");
//  //Only try to do wifi stuff if we're connected
//  if(WiFiConnected) {
//    server.handleClient();
//  }
   delay(1);
//   Serial.print("haha");
  if (bleKeyboard.isConnected()) {
//    Serial.print("lmao");
    int curr_rotary = encoder.getPosition();
    bool didScroll = false;
    RotaryEncoder::Direction direction = encoder.getDirection();

    if (curr_rotary != last_rotary) {
      Serial.print("Encoder value: ");
      Serial.print(curr_rotary);
      Serial.print(" direction: ");
      Serial.println((int)direction);
      // Note: this doesn't work properly when not in QuickNav
      if ((int)direction == -1) {
        //bleKeyboard.press(KEY_LEFT_CTRL);
        //bleKeyboard.press(KEY_LEFT_ALT);
        bleKeyboard.write(KEY_DOWN_ARROW);
        delay(20);
        didScroll = true;
      } else if ((int)direction == 1) {
        //bleKeyboard.press(KEY_LEFT_CTRL);
        //bleKeyboard.write(KEY_LEFT_ALT);
        bleKeyboard.write(KEY_UP_ARROW);
        delay(20);
        didScroll = true;
      }
    }
    last_rotary = curr_rotary;

    leftButton.update();
    if (leftButton.isDoubleClick()) {
      Serial.println("left");
      lastState = 1;
      bleKeyboard.releaseAll();
      bleKeyboard.write(KEY_ESC);
      delay(20);
    } else if (leftButton.isClick()) {
//      Serial.println("eject button");
      Serial.print("left");
      lastState = 4;
      bleKeyboard.releaseAll();
//      bleKeyboard.write(KEY_MEDIA_EJECT);
      bleKeyboard.write(KEY_LEFT_ARROW);
      delay(100);
    }

    rightButton.update();
    if (rightButton.isClick()) {
      lastState = 2;
       Serial.println("right");
//      bleKeyboard.press(KEY_LEFT_CTRL);
//`      bleKeyboard.press(KEY_LEFT_ALT);
      bleKeyboard.write(KEY_RIGHT_ARROW);
//        bleKeyboard.write(0x68);
      delay(100);
    }

    selectButtonState = digitalRead(selectButton);
    if (selectButtonState == 0 && lastState != 3) {
      Serial.println("selected button");
      lastState = 3;
      bleKeyboard.releaseAll();
//      bleKeyboard.press(KEY_LEFT_CTRL);
//      bleKeyboard.press(KEY_LEFT_ALT);
      bleKeyboard.write(0x20);
      delay(100);
    }

    upButton.update();
    if (upButton.isDoubleClick()) {
      lastState = 4;
      bleKeyboard.releaseAll();
      bleKeyboard.press(KEY_LEFT_CTRL);
      bleKeyboard.press(KEY_LEFT_ALT);
      bleKeyboard.press(KEY_LEFT_GUI);
      bleKeyboard.write(KEY_LEFT_ARROW);
      Serial.println("double up");
    } else if (upButton.isClick()) {
      lastState = 4;
      Serial.println("up");
      bleKeyboard.releaseAll();
      bleKeyboard.write(KEY_UP_ARROW);
      delay(20);
      // old code for hide / show keyboard
    }


    downButton.update();
    if (downButton.isDoubleClick()) {
      lastState = 5;
      bleKeyboard.releaseAll();
//      bleKeyboard.press(KEY_LEFT_CTRL);
//      bleKeyboard.press(KEY_LEFT_ALT);
      bleKeyboard.press(KEY_LEFT_GUI);
      bleKeyboard.write(0x8B);
      Serial.println("double down");
    } else if (downButton.isClick()) {
      lastState = 5;
      Serial.println("down");
      bleKeyboard.releaseAll();
      bleKeyboard.write(KEY_DOWN_ARROW);
      delay(20);
    }

    //
    //    downButtonState = digitalRead(downButton);
    //    if (downButtonState == 0 && lastState != 5) {
    //      Serial.println("down button");
    //      lastState = 5;
    //      bleKeyboard.releaseAll();
    //      bleKeyboard.press(KEY_LEFT_CTRL);
    //      bleKeyboard.press(KEY_LEFT_ALT);
    //      bleKeyboard.write(0x68);
    //      delay(100);
    //    }


    if (selectButtonState == 1 && rButtonState == 1 && lButtonState == 1 && upButtonState == 1 && downButtonState == 1 && lastState != 0 && !didScroll) {
      lastState = 0;
      bleKeyboard.releaseAll();
    }
  }
}
