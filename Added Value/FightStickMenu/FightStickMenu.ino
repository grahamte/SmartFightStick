//***THIS IS THE NEW VERSION THAT WORKS WITH THE NEW LIBRARIES!!!***
// TFTLCD.h and TouchScreen.h are from adafruit.com where you can also purchase a really nice 2.8" TFT with touchscreen :)
// 2012 Jeremy Saglimbeni - thecustomgeek.com
#include <Adafruit_GFX.h>    // Core graphics library
#include <Adafruit_ILI9341.h> // Hardware-specific library
#include <Adafruit_STMPE610.h>
#include <TouchScreen.h>
#include <SPI.h>
#include <EEPROM.h>

//Calibration data
#define TS_MINX 150
#define TS_MINY 130
#define TS_MAXX 3800
#define TS_MAXY 4000

//TFT and Touchscreen
#define TFT_CS 10
#define TFT_DC 9
#define STMPE_CS 8
Adafruit_ILI9341 tft = Adafruit_ILI9341(TFT_CS, TFT_DC);
Adafruit_STMPE610 ts = Adafruit_STMPE610(STMPE_CS);

// Color definitions - in 5:6:5
#define  BLACK           0x0000
#define BLUE            0x001F
#define RED             0xF800
#define GREEN           0x07E0
#define CYAN            0x07FF
#define MAGENTA         0xF81F
#define YELLOW          0xFFE0
#define WHITE           0xFFFF
#define TEST            0x1BF5
#define JJCOLOR         0x1CB6
#define JJORNG          0xFD03

TS_Point p;
int i = 0;
int page = 0;
int blv;
int sleep = 0;
int pulsev = 0;
int redflag = 0;
int greenflag = 0;
int redled = 2;
int greenled = A4;
int backlight = 3;
int battfill;
unsigned long sleeptime;
unsigned long battcheck = 10000; // the amount of time between voltage check and battery icon refresh
unsigned long prevbatt;
int battv;
int battold;
int battpercent;
int barv;
int prevpage;
int sleepnever;
int esleep;
int backlightbox;
int antpos = 278;
unsigned long awakeend;
unsigned long currenttime;
unsigned long ssitime;
char voltage[10];
char battpercenttxt [10];
long readVcc() {
  long result;
  // Read 1.1V reference against AVcc
  ADMUX = _BV(REFS0) | _BV(MUX3) | _BV(MUX2) | _BV(MUX1);
  delay(2); // Wait for Vref to settle
  ADCSRA |= _BV(ADSC); // Convert
  while (bit_is_set(ADCSRA, ADSC));
  result = ADCL;
  result |= ADCH << 8;
  result = 1126400L / result; // Back-calculate AVcc in mV
  return result;
}
void setup(void) {
  pinMode(3, OUTPUT);
  pinMode(redled, OUTPUT);
  pinMode(greenled, OUTPUT);
  esleep = EEPROM.read(1);
  blv = EEPROM.read(2);
  ////esleep = 3; // uncomment this and run once if you have not used the EEPROM before on your Arduino! Comment and reload after that.
  ////blv = 255; // uncomment this and run once if you have not used the EEPROM before on your Arduino! Comment and reload after that.c
  //  if (esleep == 1) {
  //    sleeptime = 10000;
  //  }
  //  if (esleep == 2) {
  //    sleeptime = 20000;
  //  }
  //  if (esleep == 3) {
  //    sleeptime = 30000;
  //  }
  //  if (esleep == 4) {
  //    sleeptime = 60000;
  //  }
  //  if (esleep == 5) {
  //    sleeptime = 120000;
  //  }
  //  if (esleep == 6) {
  //    sleeptime = 300000;
  //  }
  //  if (esleep == 7) {
  //    sleeptime = 600000;
  //  }
  //  if (esleep == 8) {
  //    sleeptime = 1200000;
  //  }
  //  if (esleep == 9) {
  //    sleeptime = 1800000;
  //  }
  //  if (esleep == 10) {
  //    sleeptime = 3600000;
  //  }
  //  if (esleep == 11) {
  //    sleeptime = 14400000;
  //  }
  //  if (esleep == 12) {
  //    sleepnever = 1;
  //  }
  //  awakeend = sleeptime + 1000; // set the current sleep time based on what the saved settings in EEPROM were
  pinMode(backlight, OUTPUT);
  Serial.begin(9600);
  Serial.println("JOS");
  Serial.println("Jeremy Saglimbeni  -  2011");

  tft.begin();
  if (!ts.begin()) {
    Serial.println("Unable to start touchscreen.");
  }
  else {
    Serial.println("Touchscreen started.");
  }
  tft.fillScreen(BLACK);
  tft.setRotation(1);
  tft.fillRect(71, 70, 50, 100, JJCOLOR);
  tft.fillRect(134, 70, 50, 100, JJCOLOR);
  tft.fillRect(197, 70, 50, 100, JJCOLOR);
  tft.drawRect(46, 45, 228, 150, WHITE);
  for (i = 0 ; i <= blv; i += 1) {
    analogWrite(backlight, i);
    delay(2);
  }
  delay(250);
  tft.setCursor(85, 100);
  tft.setTextSize(5);
  tft.setTextColor(WHITE);
  tft.print("J");
  delay(250);
  tft.setCursor(147, 100);
  tft.print("O");
  delay(250);
  tft.setCursor(210, 100);
  tft.print("S");
  delay(500);
  tft.setCursor(84, 210);
  tft.setTextSize(1);
  tft.print("Jeremy Saglimbeni  -  2012");
  tft.setCursor(108, 230);
  tft.print("thecustomgeek.com");
  delay(500);
  tft.fillScreen(BLACK);
  tft.fillRect(0, 0, 320, 10, JJCOLOR); // status bar
  drawhomeicon(); // draw the home icon
  tft.setCursor(1, 1);
  tft.print("Choose your game and character!");
  tft.drawRect(297, 1, 20, 8, WHITE); //battery body
  tft.fillRect(317, 3, 2, 4, WHITE); // battery tip
  tft.fillRect(298, 2, 18, 6, BLACK); // clear the center of the battery
  drawbatt();
  homescr(); // draw the homescreen
  tft.drawRect(0, 200, 245, 40, WHITE); // message box
  pinMode(13, OUTPUT);
}
#define MINPRESSURE 10
#define MAXPRESSURE 1000


void loop() {
  if (!ts.bufferEmpty()) {
    p = ts.getPoint();

    currenttime = millis();
    unsigned long currentawake = millis();

    // we have some minimum pressure we consider 'valid'
    // pressure of 0 means no pressing!
    if (p.z > MINPRESSURE && p.z < MAXPRESSURE) {
      awakeend = currenttime + sleeptime; //set the sleep time when screen is pressed
      if (sleep == 1) { // if asleep, then fade the backlight up
        for (i = 0 ; i <= blv; i += 1) {
          analogWrite(backlight, i);
          delay(1);
        }
        sleep = 0; // change the sleep mode to "awake"
        return;
      }
      /*
        Serial.print("X = ");
        Serial.print(p.x);
        Serial.print("\tY = ");
        Serial.print(p.y);
        Serial.print("\tPressure = ");
        Serial.println(p.z);
      */
      // turn from 0->1023 to tft.width
      p.x = map(p.x, TS_MINX, TS_MAXX, 0, 240);
      p.y = map(p.y, TS_MINY, TS_MAXY, 0, 320);

      Serial.print("p.y:"); // this code will help you get the y and x numbers for the touchscreen
      Serial.print(p.y);
      Serial.print("   p.x:");
      Serial.println(p.x);
    }
    // area 1
    if (p.y > 0 && p.y < 146 && p.x > 178 && p.x < 226) { // if this area is pressed
      if (page == 5) { // and if page 5 is drawn on the screen
        m5b1action(); // do whatever this button is
        tft.setTextColor(RED);
        tft.setTextSize(2);
        tft.setCursor(12, 213);
        tft.print("n/a"); // display the command in the "message box"
        //yled(550); // flash the LED yellow for a bit - change the 550 value to change LED time on
        clearmessage(); // after the LED goes out, clear the message
      }
      if (page == 4) {
        m4b1action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 3) {
        m3b1action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 2) {
        m2b1action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 1) {
        m1b1action();
        tft.setCursor(12, 213);
        tft.print("Beowulf");
        //yled(550);
        clearmessage();
      }
      if (page == 0) { // if you are on the "home" page (0)
        page = 1; // then you just went to the first page
        redraw(); // redraw the screen with the page value 1, giving you the page 1 menu
      }
    }
    // area 2
    if (p.y > 168 && p.y < 320 && p.x > 180 && p.x < 226) {
      if (page == 5) {
        m5b2action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 4) {
        m4b2action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        ////yled(550);
        clearmessage();
      }
      if (page == 3) {
        m3b2action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 2) {
        m2b2action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 1) {
        m1b2action();
        tft.setCursor(12, 213);
        tft.print("Cerebella");
        //yled(550);
        clearmessage();
      }
      if (page == 0) {
        page = 2;
        redraw();
      }
    }
    // area 3
    if (p.y > 0 && p.y < 146 && p.x > 120 && p.x < 168) {
      if (page == 5) {
        m5b3action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 4) {
        m4b3action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 3) {
        m3b3action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 2) {
        m2b3action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 1) {
        m1b3action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 0) {
        page = 3;
        redraw();
      }
    }
    // area 4
    if (p.y > 167 && p.y < 320 && p.x > 120 && p.x < 168) {
      if (page == 5) {
        m5b4action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 4) {
        m4b4action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 3) {
        m3b4action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 2) {
        m2b4action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 1) {
        m1b4action();
        tft.setCursor(12, 213);
        tft.print("n/a");
        //yled(550);
        clearmessage();
      }
      if (page == 0) {
        page = 4;
        redraw();
      }
    }
    // area 5
    if (p.y > 0 && p.y < 146 && p.x > 54 && p.x < 104) {
      if (page == 5) {
        m5b5action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 4) {
        m4b5action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 3) {
        m3b5action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 2) {
        m2b5action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 1) {
        m1b5action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 0) {
        page = 5;
        redraw();
      }
    }
    // area 6
    if (p.y > 168 && p.y < 320 && p.x > 54 && p.x < 104) {
      if (page == 5) {
        m5b6action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 4) {
        m4b6action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 3) {
        m3b6action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 2) {
        m2b6action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 1) {
        m1b6action();
        tft.setCursor(12, 213);
        tft.print("n/a");;
        //yled(550);
        clearmessage();
      }
      if (page == 0) {
        page = 6;
        redraw();
      }
    }

    // home
    if (p.y > 280 && p.y < 340 && p.x > 0 && p.x < 48) { // if the home icon is pressed
      if (page == 6) { // if you are leaving the settings page
        clearmessage(); // clear the battery voltage out of the message box
        tft.setTextSize(2);
        tft.setTextColor(YELLOW);
        tft.setCursor(12, 213);
        tft.print("Settings Saved"); // display settings saved in message box
        EEPROM.write(1, esleep); // write the sleep value to EEPROM, so it will not lose settings without power
        EEPROM.write(2, blv); // write the backlight value to EEPROM, so it will not lose settings without power
        clearsettings(); // erase all the drawings on the settings page
      }
      if (page == 0) { // if you are already on the home page
        drawhomeiconred(); // draw the home icon red
        delay(250); // wait a bit
        drawhomeicon(); // draw the home icon back to white
        return; // if you were on the home page, stop.
      }
      else { // if you are not on the settings, home, or keyboard page
        page = prevpage; // a value to keep track of what WAS on the screen to redraw/erase only what needs to be
        page = 0; // make the current page home
        redraw(); // redraw the page
        tft.fillScreen(BLACK);
        tft.fillRect(0, 0, 320, 10, JJCOLOR); // status bar
        drawhomeicon(); // draw the home icon
        tft.setCursor(1, 1);
        tft.setTextSize(1);
        tft.print("Choose your game and character!");
        tft.drawRect(297, 1, 20, 8, WHITE); //battery body
        tft.fillRect(317, 3, 2, 4, WHITE); // battery tip
        tft.fillRect(298, 2, 18, 6, BLACK); // clear the center of the battery
        drawbatt();
        homescr(); // draw the homescreen
        tft.drawRect(0, 200, 245, 40, WHITE); // message box
      }
    }
    // message area
    if (p.y > 0 && p.y < 246 && p.x > 4 && p.x < 44) {
      clearmessage(); // erase the message
    }
    // backlight buttons
    if (p.y > 0 && p.y < 56 && p.x > 176 && p.x < 226) {
      if (page == 6) {
        blightdown();
      }
    }
    if (p.y > 260 && p.y < 320 && p.x > 180 && p.x < 230) {
      if (page == 6) {
        blightup();
      }
    }
    /*
      // optional buttons
      if (p.y > 3 && p.y < 66 && p.x > 72 && p.x < 126) {
      if (page == 6) {
      option3down();
      }
      }
      if (p.y > 269 && p.y < 324 && p.x > 72 && p.x < 126) {
      if (page == 6) {
      option3up();
      }
      }
    */
  }
  if (currenttime - prevbatt > battcheck) {
    drawbatt();
    prevbatt = currenttime;

  }
}

void redraw() { // redraw the page
  if ((prevpage != 6) || (page != 7)) {
    clearcenter();
  }
  if (page == 0) {
    homescr();
  }
  if (page == 1) {
    menu1();
  }
  if (page == 2) {
    menu2();
  }
  if (page == 3) {
    menu3();
  }
  if (page == 4) {
    menu4();
  }
  if (page == 5) {
    menu5();
  }
  if (page == 6) {
    settingsscr();
  }
}
void clearcenter() { // the reason for so many small "boxes" is that it's faster than filling the whole thing
  tft.drawRect(0, 20, 150, 50, BLACK);
  tft.drawRect(170, 20, 150, 50, BLACK);
  tft.drawRect(0, 80, 150, 50, BLACK);
  tft.drawRect(170, 80, 150, 50, BLACK);
  tft.drawRect(0, 140, 150, 50, BLACK);
  tft.drawRect(170, 140, 150, 50, BLACK);
  tft.fillRect(22, 37, 106, 16, BLACK);
  tft.fillRect(192, 37, 106, 16, BLACK);
  tft.fillRect(22, 97, 106, 16, BLACK);
  tft.fillRect(192, 97, 106, 16, BLACK);
  tft.fillRect(22, 157, 106, 16, BLACK);
  tft.fillRect(192, 157, 106, 16, BLACK);
}
void clearsettings() { // this is used to erase the extra drawings when exiting the settings page
  tft.fillRect(0, 20, 320, 110, BLACK);
  delay(500);
  clearmessage();
}
void homescr() {
  tft.setTextColor(WHITE);
  tft.setTextSize(1);
  boxes();
  tft.setCursor(41, 37);
  tft.print("Skullgirls");
  tft.setTextSize(2);
  tft.setCursor(210, 37);
  tft.print("n/a");
  tft.setCursor(41, 97);
  tft.print("n/a");
  tft.setCursor(210, 97);
  tft.print("n/a");
  tft.setCursor(41, 157);
  tft.print("n/a");
  tft.setCursor(200, 157);
  tft.print("Settings");
}
void menu1() {
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  boxes();
  tft.setCursor(22, 37);
  tft.print("Beowulf");
  tft.setCursor(192, 37);
  tft.print("Cerebella");
  tft.setCursor(22, 97);
  tft.print("n/a");
  tft.setCursor(192, 97);
  tft.print("n/a");
  tft.setCursor(22, 157);
  tft.print("n/a");
  tft.setCursor(192, 157);
  tft.print("n/a");
}
void menu2() {
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  boxes();
  tft.setCursor(22, 37);
  tft.print("n/a");
  tft.setCursor(192, 37);
  tft.print("n/a");
  tft.setCursor(22, 97);
  tft.print("n/a");
  tft.setCursor(192, 97);
  tft.print("n/a");
  tft.setCursor(22, 157);
  tft.print("n/a");
  tft.setCursor(192, 157);
  tft.print("n/a");
}
void menu3() {
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  boxes();
  tft.setCursor(22, 37);
  tft.print("n/a");
  tft.setCursor(192, 37);
  tft.print("n/a");
  tft.setCursor(22, 97);
  tft.print("n/a");
  tft.setCursor(192, 97);
  tft.print("n/a");
  tft.setCursor(22, 157);
  tft.print("n/a");
  tft.setCursor(192, 157);
  tft.print("n/a");
}
void menu4() {
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  boxes();
  tft.setCursor(22, 37);
  tft.print("n/a");
  tft.setCursor(192, 37);
  tft.print("n/a");
  tft.setCursor(22, 97);
  tft.print("n/a");
  tft.setCursor(192, 97);
  tft.print("n/a");
  tft.setCursor(22, 157);
  tft.print("n/a");
  tft.setCursor(192, 157);
  tft.print("n/a");
}
void menu5() {
  tft.setTextColor(WHITE);
  tft.setTextSize(2);
  boxes();
  tft.setCursor(22, 37);
  tft.print("n/a");
  tft.setCursor(192, 37);
  tft.print("n/a");
  tft.setCursor(22, 97);
  tft.print("n/a");
  tft.setCursor(192, 97);
  tft.print("n/a");
  tft.setCursor(22, 157);
  tft.print("n/a");
  tft.setCursor(192, 157);
  tft.print("n/a");
}
void settingsscr() {
  // backlight level
  tft.setTextColor(WHITE);
  tft.setTextSize(3);
  tft.fillRect(0, 20, 60, 50, RED);
  tft.drawRect(0, 20, 60, 50, WHITE);
  tft.drawRect(80, 20, 160, 50, JJCOLOR);
  tft.fillRect(260, 20, 60, 50, GREEN);
  tft.drawRect(260, 20, 60, 50, WHITE);
  tft.setCursor(22, 33);
  tft.print("-");
  tft.setCursor(282, 33);
  tft.print("+");
  tft.setTextSize(1);
  tft.setCursor(120, 31);
  tft.print("Backlight Level");
  tft.drawRect(110, 48, 100, 10, WHITE);
  blbar();
  //?? uncomment this if you want a third adjustable option
  /*
    tft.fillRect(0, 140, 60, 50, RED);
    tft.drawRect(0, 140, 60, 50, WHITE);
    tft.drawRect(80, 140, 160, 50, JJCOLOR);
    tft.fillRect(260, 140, 60, 50, GREEN);
    tft.drawRect(260, 140, 60, 50, WHITE);
    tft.print(22, 153, "-", WHITE, 3);
    tft.print(130, 151, "Thing #3", WHITE);
    tft.print(282, 153, "+", WHITE, 3);
    tft.drawRect(110, 168, 100, 10, WHITE);
  */
  battv = readVcc(); // read the voltage
  itoa (battv, voltage, 10);
  tft.setTextColor(YELLOW);
  tft.setTextSize(2);
  tft.setCursor(12, 213);
  tft.print(voltage);
  tft.setCursor(60, 213);
  tft.print("mV");
  /*
    battpercent = (battv / 5000) * 100, 2;
    itoa (battpercent, battpercenttxt, 10);
    tft.print(102, 213, battpercenttxt, YELLOW, 2);
  */
}

void option3down() { // adjust option 3 down in the settings screen
}
void option3up() { // adjust option 3 up in the settings screen
}
//custom defined actions - this is where you put your button functions
void m1b1action() {
  beoMove();
}
void m1b2action() {
  cereMove();
}
void m1b3action() {
}
void m1b4action() {
}
void m1b5action() {
}
void m1b6action() {
}
void m2b1action() {
}
void m2b2action() {
}
void m2b3action() {
}
void m2b4action() {
}
void m2b5action() {
}
void m2b6action() {
}
void m3b1action() {
}
void m3b2action() {
}
void m3b3action() {
}
void m3b4action() {
}
void m3b5action() {
}
void m3b6action() {
}
void m4b1action() {
}
void m4b2action() {
}
void m4b3action() {
}
void m4b4action() {
}
void m4b5action() {
}
void m4b6action() {
}
void m5b1action() {
}
void m5b2action() {
}
void m5b3action() {
}
void m5b4action() {
}
void m5b5action() {
}
void m5b6action() {
}
void blightup() { // increase the backlight brightness
  blv = blv + 5;
  if (blv >= 255) {
    blv = 255;
  }
  analogWrite(backlight, blv);
  blbar();
}
void blightdown() { // decrease the backlight brightness
  blv = blv - 5;
  if (blv <= 5) {
    blv = 5;
  }
  analogWrite(backlight, blv);
  blbar();
}
void blbar() { // this function fills the yellow bar in the backlight brightness adjustment
  if (blv < barv) {
    tft.fillRect(111, 49, 98, 8, BLACK);
  }
  backlightbox = map(blv, 1, 255, 0, 98);
  tft.fillRect(111, 49, backlightbox, 8, YELLOW);
  barv = blv;
  delay(25);
}
void boxes() { // redraw the button outline boxes
  tft.drawRect(0, 20, 150, 50, JJCOLOR);
  tft.drawRect(170, 20, 150, 50, JJCOLOR);
  tft.drawRect(0, 80, 150, 50, JJCOLOR);
  tft.drawRect(170, 80, 150, 50, JJCOLOR);
  tft.drawRect(0, 140, 150, 50, JJCOLOR);
  tft.drawRect(170, 140, 150, 50, JJCOLOR);
}

void beoMove() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE); tft.setTextSize(3);
  tft.println("Beowulf Move List");
  tft.println();
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(1);
  tft.println("Hurting Hurl: 236 + P");
  tft.println("Wulf Blitzer: 236 + L (Air OK)");
  tft.println("Wulf Shoot: 236 + LP+LK");
  tft.println("Take a Seat: K+K (No Chair)");
  tft.println("Gigantic Arm (Lvl 1): 236 + P+P");
  tft.println("Airwulf (Lvl 1): 214 + K+K");
  tft.println("Three Wulf Moonsault (Lvl 3): 214 + P+P");
  tft.println("Wulfamania (Lvl 3): 6321479 (Full Circle) + LP+LK");
  tft.setTextColor(ILI9341_WHITE); tft.setTextSize(1);
  tft.println("Grab Specials");
  tft.setTextColor(ILI9341_GREEN);
  tft.println("Berserker Headbutt: P");
  tft.println("Naegling Knee Lift: K");
  tft.println("Wulf Press Slam: Forward/Back + P");
  tft.println("Diving Wulfdog: Forward + K (Air OK)");
  tft.println("Da Grendel Killa: Back + K (Air OK)");
  tft.println("Geatish Trepak: Down + K (With Chair)");
  tft.println("Canis Major Press: Down + P (Air Only)");
  tft.println("Into the Trash (Lvl 1): 236 + MP+MK/HP+HK");
  drawhomeicon();
}

void cereMove() {
  tft.fillScreen(ILI9341_BLACK);
  tft.setCursor(0, 0);
  tft.setTextColor(ILI9341_WHITE); tft.setTextSize(3);
  tft.println("Cere Move List");
  tft.println();
  tft.setTextColor(ILI9341_GREEN);
  tft.setTextSize(1);
  tft.println("Lock n' Load: 236 + P");
  tft.println("Diamond Deflector: 623 + LP");
  tft.println("Devil Horns: 623 + MP");
  tft.println("Cerecopter: 623 + HP");
  tft.println("Diamond Drop: 236 + LP+LK");
  tft.println("Merry Go-Rilla: 214 + LP+LK");
  tft.println("Excellebella: 623 + LP+LK");
  tft.println("Grab Bag: 236 + LP+LK (Air Only)");
  tft.println("Tumbling Run: Forward + K");
  tft.println("   Run Stop (During Tumbling Run): LP/LK");
  tft.println("   Kanchou (During Tumbling Run): MP/MK");
  tft.println("   Battle Butt (During Tumbling Run): HP/HK");
  tft.println("   Pummel Horse (During Tumbling Run): LP+LK");
  tft.println("Diamond Dynamo (Lvl 1): 236 + P+P");
  tft.println("Ultimate Showstopper (Lvl 1): 63214789 (Full Circle) + LP+LK");
  tft.println("Diamonds are Forever (Lvl 3): 214 + P+P");
  drawhomeicon();
}


void drawhomeicon() { // draws a white home icon
  tft.drawLine(280, 219, 299, 200, WHITE);
  tft.drawLine(300, 200, 304, 204, WHITE);
  tft.drawLine(304, 203, 304, 200, WHITE);
  tft.drawLine(305, 200, 307, 200, WHITE);
  tft.drawLine(308, 200, 308, 208, WHITE);
  tft.drawLine(309, 209, 319, 219, WHITE);
  tft.drawLine(281, 219, 283, 219, WHITE);
  tft.drawLine(316, 219, 318, 219, WHITE);
  tft.drawRect(284, 219, 32, 21, WHITE);
  tft.drawRect(295, 225, 10, 15, WHITE);
}
void drawhomeiconred() { // draws a red home icon
  tft.drawLine(280, 219, 299, 200, RED);
  tft.drawLine(300, 200, 304, 204, RED);
  tft.drawLine(304, 203, 304, 200, RED);
  tft.drawLine(305, 200, 307, 200, RED);
  tft.drawLine(308, 200, 308, 208, RED);
  tft.drawLine(309, 209, 319, 219, RED);
  tft.drawLine(281, 219, 283, 219, RED);
  tft.drawLine(316, 219, 318, 219, RED);
  tft.drawRect(284, 219, 32, 21, RED);
  tft.drawRect(295, 225, 10, 15, RED);
}
void clearmessage() {
  tft.fillRect(12, 213, 226, 16, BLACK); // black out the inside of the message box
}
void drawbatt() {
  battv = readVcc(); // read the voltage
  if (battv < battold) { // if the voltage goes down, erase the inside of the battery
    tft.fillRect(298, 2, 18, 6, BLACK);
  }
  battfill = map(battv, 3000, 4850, 2, 18); // map the battery voltage 3000 nis the low, 4150 is the high
  if (battfill > 7) { // if the battfill value is between 8 and 18, fill with green
    tft.fillRect(298, 2, battfill, 6, GREEN);
  }
  else { // if the battfill value is below 8, fill with red
    tft.fillRect(298, 2, battfill, 6, RED);
  }
  battold = battv; // this helps determine if redrawing the battfill area is necessary
}
