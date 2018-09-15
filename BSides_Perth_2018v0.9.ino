

//Libraries
#include <Scheduler.h>
#include <ESP8266WiFi.h>
#include <ESP8266WiFiMulti.h>
#include <WiFiClientSecure.h>
#include <Adafruit_GFX.h>
#include <TFT_ILI9163C.h>
#define FS_NO_GLOBALS
#include <FS.h>
#include <JPEGDecoder.h>
#include <SPI.h>

//Hardware
#define CS D4
#define DC D2
#define RST D1
#define TFT_BLACK 0
TFT_ILI9163C tft = TFT_ILI9163C(CS, RST, DC);
int ledPin = D8;           // the PWM pin the LED is attached to
int brightness = 0;    // how bright the LED is
int fadeAmount = 5;    // how many points to fade the LED by
ESP8266WiFiMulti WiFiMulti;
#define FLAG flag{fladge_on_the_badge}


/*TFT to NodeMCU

LED to 3V3
SCK to D5
SDA to D7
A0/DC to D1
RST to D2
CS to D4
GND to GND
VCC to 3V3
*/




/*====================================================================================
  This sketch contains support functions for the ESP6266 SPIFFS filing system

  Created by Bodmer 15th Jan 2017
  ==================================================================================*/
 
//====================================================================================
//                 Print a SPIFFS directory list (root directory)
//====================================================================================

void listFiles(void) {
  Serial.println();
  Serial.println("SPIFFS files found:");

  fs::Dir dir = SPIFFS.openDir("/"); // Root directory
  String  line = "=====================================";

  Serial.println(line);
  Serial.println("  File name               Size");
  Serial.println(line);

  while (dir.next()) {
    String fileName = dir.fileName();
    Serial.print(fileName);
    int spaces = 25 - fileName.length(); // Tabulate nicely
    while (spaces--) Serial.print(" ");
    fs::File f = dir.openFile("r");
    Serial.print(f.size()); Serial.println(" bytes");
  }

  Serial.println(line);
  Serial.println();
  delay(1000);
}
//====================================================================================

/*====================================================================================
  This sketch contains support functions to render the Jpeg images.

  Created by Bodmer 15th Jan 2017
  ==================================================================================*/

// Return the minimum of two values a and b
#define minimum(a,b)     (((a) < (b)) ? (a) : (b))

//====================================================================================
//   Opens the image file and prime the Jpeg decoder
//====================================================================================
void drawJpeg(const char *filename, int xpos, int ypos) {

  Serial.println("===========================");
  Serial.print("Drawing file: "); Serial.println(filename);
  Serial.println("===========================");

  // Open the named file (the Jpeg decoder library will close it after rendering image)
  fs::File jpegFile = SPIFFS.open( filename, "r");    // File handle reference for SPIFFS
  //  File jpegFile = SD.open( filename, FILE_READ);  // or, file handle reference for SD library
 
  if ( !jpegFile ) {
    Serial.print("ERROR: File \""); Serial.print(filename); Serial.println ("\" not found!");
    return;
  }

  // Use one of the three following methods to initialise the decoder:
  //boolean decoded = JpegDec.decodeFsFile(jpegFile); // Pass a SPIFFS file handle to the decoder,
  //boolean decoded = JpegDec.decodeSdFile(jpegFile); // or pass the SD file handle to the decoder,
  boolean decoded = JpegDec.decodeFsFile(filename);  // or pass the filename (leading / distinguishes SPIFFS files)
                                   // Note: the filename can be a String or character array type
  if (decoded) {
    // print information about the image to the serial port
    jpegInfo();

    // render the image onto the screen at given coordinates
    jpegRender(xpos, ypos);
  }
  else {
    Serial.println("Jpeg file format not supported!");
  }
}

//====================================================================================
//   Decode and render the Jpeg image onto the TFT screen
//====================================================================================
void jpegRender(int xpos, int ypos) {

  // retrieve infomration about the image
  uint16_t  *pImg;
  uint16_t mcu_w = JpegDec.MCUWidth;
  uint16_t mcu_h = JpegDec.MCUHeight;
  uint32_t max_x = JpegDec.width;
  uint32_t max_y = JpegDec.height;

  // Jpeg images are draw as a set of image block (tiles) called Minimum Coding Units (MCUs)
  // Typically these MCUs are 16x16 pixel blocks
  // Determine the width and height of the right and bottom edge image blocks
  uint32_t min_w = minimum(mcu_w, max_x % mcu_w);
  uint32_t min_h = minimum(mcu_h, max_y % mcu_h);

  // save the current image block size
  uint32_t win_w = mcu_w;
  uint32_t win_h = mcu_h;

  // record the current time so we can measure how long it takes to draw an image
  uint32_t drawTime = millis();

  // save the coordinate of the right and bottom edges to assist image cropping
  // to the screen size
  max_x += xpos;
  max_y += ypos;

  // read each MCU block until there are no more
  while ( JpegDec.read()) {

    // save a pointer to the image block
    pImg = JpegDec.pImage;

    // calculate where the image block should be drawn on the screen
    int mcu_x = JpegDec.MCUx * mcu_w + xpos;
    int mcu_y = JpegDec.MCUy * mcu_h + ypos;

    // check if the image block size needs to be changed for the right and bottom edges
    if (mcu_x + mcu_w <= max_x) win_w = mcu_w;
    else win_w = min_w;
    if (mcu_y + mcu_h <= max_y) win_h = mcu_h;
    else win_h = min_h;

    // calculate how many pixels must be drawn
    uint32_t mcu_pixels = win_w * win_h;

    // draw image MCU block only if it will fit on the screen
    if ( ( mcu_x + win_w) <= tft.width() && ( mcu_y + win_h) <= tft.height())
  {
      // Now set a MCU bounding window on the TFT to push pixels into (x, y, x + width - 1, y + height - 1)
      tft.setAddrWindow(mcu_x, mcu_y, mcu_x + win_w - 1, mcu_y + win_h - 1);
      // Write all MCU pixels to the TFT window
      while (mcu_pixels--) tft.pushColor(*pImg++);
    }

    else if ( ( mcu_y + win_h) >= tft.height()) JpegDec.abort();

  }

  // calculate how long it took to draw the image
  drawTime = millis() - drawTime; // Calculate the time it took

  // print the results to the serial port
  Serial.print  ("Total render time was    : "); Serial.print(drawTime); Serial.println(" ms");
  Serial.println("=====================================");

}

//====================================================================================
//   Print information decoded from the Jpeg image
//====================================================================================
void jpegInfo() {

  Serial.println("===============");
  Serial.println("JPEG image info");
  Serial.println("===============");
  Serial.print  ("Width      :"); Serial.println(JpegDec.width);
  Serial.print  ("Height     :"); Serial.println(JpegDec.height);
  Serial.print  ("Components :"); Serial.println(JpegDec.comps);
  Serial.print  ("MCU / row  :"); Serial.println(JpegDec.MCUSPerRow);
  Serial.print  ("MCU / col  :"); Serial.println(JpegDec.MCUSPerCol);
  Serial.print  ("Scan type  :"); Serial.println(JpegDec.scanType);
  Serial.print  ("MCU width  :"); Serial.println(JpegDec.MCUWidth);
  Serial.print  ("MCU height :"); Serial.println(JpegDec.MCUHeight);
  Serial.println("===============");
  Serial.println("");
}

//====================================================================================
//   Open a Jpeg file and send it to the Serial port in a C array compatible format
//====================================================================================
void createArray(const char *filename) {

  // Open the named file
  fs::File jpgFile = SPIFFS.open( filename, "r");    // File handle reference for SPIFFS
  //  File jpgFile = SD.open( filename, FILE_READ);  // or, file handle reference for SD library
 
  if ( !jpgFile ) {
    Serial.print("ERROR: File \""); Serial.print(filename); Serial.println ("\" not found!");
    return;
  }

  uint8_t data;
  byte line_len = 0;
  Serial.println("");
  Serial.println("// Generated by a JPEGDecoder library example sketch:");
  Serial.println("// https://github.com/Bodmer/JPEGDecoder");
  Serial.println("");
  Serial.println("#if defined(__AVR__)");
  Serial.println("  #include <avr/pgmspace.h>");
  Serial.println("#endif");
  Serial.println("");
  Serial.print  ("const uint8_t ");
  while (*filename != '.') Serial.print(*filename++);
  Serial.println("[] PROGMEM = {"); // PROGMEM added for AVR processors, it is ignored by Due

  while ( jpgFile.available()) {

    data = jpgFile.read();
    Serial.print("0x"); if (abs(data) < 16) Serial.print("0");
    Serial.print(data, HEX); Serial.print(",");// Add value and comma
    line_len++;
    if ( line_len >= 32) {
      line_len = 0;
      Serial.println();
    }

  }

  Serial.println("};\r\n");
  jpgFile.close();
}
//====================================================================================



//Colours
#define WHITE 0xFFFF
#define BLACK 0x0000
#define BLUE 0x001F
#define RED 0xF800
#define GREEN 0x07E0
#define CYAN 0x07FF
#define MAGENTA 0xF81F
#define YELLOW 0xFFE0


class LEDBlink : public Task {
  protected:
    void loop()  {
      delay(500);
      // fade in from min to max in increments of 5 points:
      for (int fadeValue = 0 ; fadeValue <= 255; fadeValue += 5) {
        // sets the value (range from 0 to 255):
        analogWrite(ledPin, fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        delay(30);
      }

      // fade out from max to min in increments of 5 points:
      for (int fadeValue = 255 ; fadeValue >= 0; fadeValue -= 5) {
        // sets the value (range from 0 to 255):
        analogWrite(ledPin, fadeValue);
        // wait for 30 milliseconds to see the dimming effect
        delay(30);
      }
    }
} blink_task;

class PrintTask : public Task {
  protected:
    void setup() {
      tft.begin();
      delay(10000);
      //print base text first
      tft.setCursor(10, 10);
      tft.setTextColor(YELLOW);
      tft.setTextSize(1);
      tft.println("BSides Perth 2018");
      tft.setCursor(10, 30);
      tft.setTextColor(WHITE);
      tft.setTextSize(1);
      tft.println("Current Speaker:");
      tft.setCursor(10, 70);
      tft.setTextColor(MAGENTA);
      tft.setTextSize(1);
      tft.println("Next Speaker:");
      tft.setCursor(10, 110);
      tft.setTextColor(CYAN);
      tft.setTextSize(1);
      tft.println("tweet #bsidesperth");
      tft.setCursor(10, 120);
      tft.setTextColor(CYAN);
      tft.setTextSize(1);
      tft.println("tweet @bsidesper");

      delay(500);
      // We start by connecting to a WiFi network
      WiFi.mode(WIFI_STA);
      WiFiMulti.addAP("BSP_Badge", "myspace1?");

      tft.println();
      tft.println();
      tft.print("Wait for WiFi... ");

      while (WiFiMulti.run() != WL_CONNECTED) {
        tft.print(".");
        delay(500);
      }

      tft.println("");
      tft.println("WiFi connected");
      tft.println("IP address: ");
      tft.println(WiFi.localIP());

      delay(500);

    }

    void loop() {

      const uint16_t port = 80;
      const char * host = "13.238.161.58"; // ip or dns

      tft.print("connecting to ");
      tft.println(host);

      // Use WiFiClient class to create TCP connections
      WiFiClient client;

      if (!client.connect(host, port)) {

        tft.setCursor(10, 40);
        tft.setTextColor(RED);
        tft.setTextSize(1);
        tft.println("connection failed");
        //tft.println("wait 5 sec...");
        delay(5000);
        return;
      }

      // This will send the request to the server
      client.println("GET /boopsybadge.txt");
      //read back one line from server
      String line = client.readStringUntil('\r');
      tft.setCursor(10, 40);
      // tft.setTextWrap(true);
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.println(line);
      tft.setCursor(10, 40);
      tft.setTextColor(GREEN);
      tft.setTextSize(1);
      tft.println(line);
      client.stop();
      //tft.println("wait 5 mins...");
      delay(300000);
      //ESP.deepSleep(300e6); // 300e6 is 5 minutes
      // write the same text in black over the top - to make way for the next text!
      tft.setCursor(10, 40);
      tft.setTextColor(BLACK);
      tft.setTextSize(1);
      tft.println(line);
      //tft.println("closing connection");
      //client.stop();
    }




  private:
    uint8_t state;
} print_task;

class PictureTask : public Task {
  protected:
  #include <JPEGDecoder.h>
  #define FS_NO_GLOBALS
  #include <FS.h>
  

    void setup()
    {
      
      
      Serial.begin(9600); // Used for messages and the C array generator
      delay(10);
      Serial.println("BSides Perth -- Checking for images");
      tft.begin();
      tft.setRotation(0);  // 0 & 2 Portrait. 1 & 3 landscape
      tft.fillScreen(TFT_BLACK);
      if (!SPIFFS.begin()) {
        Serial.println("SPIFFS initialisation failed!");
        //while (1) yield(); // Stay here twiddling thumbs waiting
      }
      Serial.println("\r\nInitialization done.");
      listFiles(); // Lists the files so you can see what is in the SPIFFS

    }

    void loop() {
      // Note the / before the SPIFFS file name must be present, this means the file is in
      // the root directory of the SPIFFS, e.g. "/Tiger.rjpg" for a file called "Tiger.jpg"

      tft.setRotation(0);  // portrait
      //tft.fillScreen(random(0xFFFF));

      drawJpeg("/boops.jpg", 0, 0);
      delay(2500);
      drawJpeg("/sponsors.jpg", 0, 0);
      delay(2500);

      tft.fillScreen(BLACK);

      while (1) yield(); // Stay here
    }
} pic_task;

void setup() {
  Serial.begin(115200);

  Serial.println("");

  delay(1000);
  
  Scheduler.start(&blink_task);
  Scheduler.start(&pic_task);
  delay(5000);
  Scheduler.start(&print_task);
  
  

  Scheduler.begin();
}

void loop() {}

