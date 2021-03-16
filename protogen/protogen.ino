//imports custom bitmaps
#include "idle.h"
#include "eyes.h"
#include "mouth.h"
#include "nose.h"

//import libraries
#include <Wire.h>
#include "Adafruit_MCP23017.h"
Adafruit_MCP23017 io;

#include <Adafruit_SSD1306.h>
#define OLED_WIDTH  128 // OLED display width, in pixels
#define OLED_HEIGHT 32 // OLED display height, in pixels
#define OLED_RESET  13 // Reset pin # (or -1 if sharing Arduino reset pin)
Adafruit_SSD1306 oled(OLED_WIDTH, OLED_HEIGHT, &Wire, OLED_RESET);

#include <ESP32-RGB64x32MatrixPanel-I2S-DMA.h>
RGB64x32MatrixPanel_I2S_DMA matrix;


//define pins for LED matrix
//pin definiton for HUZZAH32
#define R1_PIN  12
#define G1_PIN  27
#define B1_PIN  33
#define R2_PIN  15
#define G2_PIN  32
#define B2_PIN  14

#define A_PIN   21
#define B_PIN   17
#define C_PIN   16
#define D_PIN   19
#define E_PIN   -1
          
#define LAT_PIN 18
#define OE_PIN  4

#define CLK_PIN 5

//define the input pins
//all input pins come through MCP23017, pin definitions at https://github.com/adafruit/Adafruit-MCP23017-Arduino-Library
#define noseSensor  15

//finger switches
#define leftPinkie  0
#define leftRing    1
#define leftMiddle  2
#define leftIndex   3
#define leftThumb   4

#define rightThumb  12
#define rightIndex  11
#define rightMiddle 10
#define rightRing   9
#define rightPinkie 8

#define batIn 35

//global variables
unsigned long startTime = 0;  //current tick
int blinkTime = 0;  //how long until the next blink
bool blinkEnable = true;
int currentExpression = 0;  //which expression we are currrently in. 0=neutral, 1=happy, 2=love, 3=angry, 4=sad, 5=suprise
int batVolt = 0;  //current battery voltage

//current face colour
int faceR = 0;
int faceG = 0;
int faceB = 7;

//idle animation
bool idling = false;  //whether we should be idling or not
float idleHPos = 32;  //current horizontal position
float idleVPos = 16;  //current vertical position
float idleHSpeed = 1; //horizontal speed
float idleVSpeed = 1; //vertical speed
#define idleHBorder round(idleBitmapW/2)  //accounts for the size of the image
#define idleVBorder round(idleBitmapH/2)   //accounts for the size of the image
int idleCorner = 2; //corner detection
float idleR = 0;  //red value of the idle image
float idleG = 0;  //green value of the idle image
float idleB = 7;  //blue value of the idle image

void setup() {
  Serial.begin(115200);
  io.begin();

  io.pinMode(noseSensor, INPUT); //nose sensor - pulled low and goes high when booped

  //pin setup for finger switches. Using pullup, so goes LOW when button is pressed
  io.pinMode(leftPinkie, INPUT);
  io.pullUp(leftPinkie, HIGH);
  io.pinMode(leftRing, INPUT);
  io.pullUp(leftRing, HIGH);
  io.pinMode(leftMiddle, INPUT);
  io.pullUp(leftMiddle, HIGH);
  io.pinMode(leftIndex, INPUT);
  io.pullUp(leftIndex, HIGH);
  io.pinMode(leftThumb, INPUT);
  io.pullUp(leftThumb, HIGH);
  io.pinMode(rightThumb, INPUT);
  io.pullUp(rightThumb, HIGH);
  io.pinMode(rightIndex, INPUT);
  io.pullUp(rightIndex, HIGH);
  io.pinMode(rightMiddle, INPUT);
  io.pullUp(rightMiddle, HIGH);
  io.pinMode(rightRing, INPUT);
  io.pullUp(rightRing, HIGH);
  io.pinMode(rightPinkie, INPUT);
  io.pullUp(rightPinkie, HIGH);

  //OLED screen
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  if(!oled.begin(SSD1306_SWITCHCAPVCC, 0x3C)) { // Address 0x3C for 128x32
    Serial.println(F("SSD1306 allocation failed"));
    for(;;); // Don't proceed, loop forever
  }

  // Show initial display buffer contents on the screen --
  // the library initializes this with an Adafruit splash screen.
  oled.display();
  delay(2000); // Pause for 2 seconds

  // Clear the buffer
  oled.clearDisplay();

  //starts the matrix and writes all black to it
  matrix.begin(R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN);
  
  matrix.fillScreen(matrix.Color333(0, 0, 0));  
  matrix.drawBitmap(0,0, noseNeutral, 128, 32, matrix.Color333(faceR, faceG, faceB));
  matrix.drawBitmap(0,0, mouthNeutral, 128, 32, matrix.Color333(faceR, faceG, faceB));
  matrix.drawBitmap(0, 0, eyeNeutral, 128, 32, matrix.Color333(faceR, faceG, faceB));

  startTime = millis();
  blinkTime = random(1,15)*1000;
}

//idle animation
void idle(){
  float h = idleHPos + idleHSpeed;  //temporary variable to store next horizontal position
  float v = idleVPos + idleVSpeed;  //temporary variable to store next vertical position

  if(h <= matrix.width()-idleHBorder && h >= idleHBorder){
    //if the next position is within the screen, keep it
    idleHPos = h;
  } else if(h > matrix.width()-idleHBorder){
    //however, if it hits the right side of the screen then bounce back
    idleHSpeed = -(random(100, 150)/100.0); //randomly generates either negative 1 or 2 (this makes the image go backwards across the screen)
  } else if(h < idleHBorder){
    //and if it hits the left side of the screen; bounce
    idleHSpeed = (random(50, 150)/100.0);  //randomly generates either 1 or 2
  }


  //same as before, but for up and down
  if(v <= matrix.height()-idleVBorder && v >= idleVBorder){
    idleVPos = v;
  } else if(v > matrix.height()-idleVBorder){
    //if it hits the bottom
    idleVSpeed = -(random(100, 150)/100.0);
  } else if(v < idleVBorder){
    //if it hits the top
    idleVSpeed = (random(100, 150)/100.0);
  }

  //the following is used to detect when the image hits the corner, DVD style. Once it does, it randomly generates a new colour (between 0 and 1) and multiplies it by the brightness
  if(idleHPos <= idleCorner+idleHBorder && idleHPos >= idleHBorder && idleVPos <= idleCorner+idleVBorder && idleVPos >= idleVBorder){
    //top left
    idleR = round(random(0, 99)/100.0);
    idleG = round(random(0, 99)/100.0);
    idleB = round(random(0, 99)/100.0);
  } else if(idleHPos >= (matrix.width()-idleHBorder)-idleCorner && idleHPos <= (matrix.width()-idleHBorder) && idleVPos <= idleCorner+idleVBorder && idleVPos >= idleVBorder){
    //top right
    idleR = round(random(0, 99)/100.0);
    idleG = round(random(0, 99)/100.0);
    idleB = round(random(0, 99)/100.0);
  } else if(idleHPos <= idleCorner+idleHBorder && idleHPos >= idleHBorder && idleVPos >= (matrix.height()-idleVBorder)-idleCorner && idleVPos <= (matrix.height()-idleVBorder)){
    //bottom left
    idleR = round(random(0, 99)/100.0);
    idleG = round(random(0, 99)/100.0);
    idleB = round(random(0, 99)/100.0);
  } else if(idleHPos >= (matrix.width()-idleHBorder)-idleCorner && idleHPos <= (matrix.width()-idleHBorder) && idleVPos >= (matrix.height()-idleVBorder)-idleCorner && idleVPos <= (matrix.height()-idleVBorder)){
    //bottom right
    idleR = round(random(0, 99)/100.0);
    idleG = round(random(0, 99)/100.0);
    idleB = round(random(0, 99)/100.0);
  }
  
  matrix.fillScreen(matrix.Color333(0, 0, 0));  //sets the entire screen to black before drawing the next frame
  //matrix.drawCircle(idleHPos, idleVPos, 3, matrix.Color333(idleR, idleG, idleB));
  matrix.drawBitmap((idleHPos-(15/2)), (idleVPos-(4/2)), idleBitmap, 15, 4, matrix.Color333(idleR, idleG, idleB));  //then draws the image in its new position
  delay(50);  //and wait a few milliseconds so the screen doesn't go too fast
}

void boop(){
  matrix.fillScreen(matrix.Color333(0, 0, 0));  //blank the screen
  //then draw the suprised bitmaps
  matrix.drawBitmap(0, 0, eyeSuprise, 128, 32, matrix.Color333(faceR, faceG, faceB));
  matrix.drawBitmap(0, 0, noseSuprise, 128, 32, matrix.Color333(faceR, faceG, faceB));
  matrix.drawBitmap(0, 0, mouthSuprise, 128, 32, matrix.Color333(faceR, faceG, faceB));
  //wait a few seconds
  delay(2000);
  //then switch back to the neutral expression
  matrix.fillScreen(matrix.Color333(0, 0, 0));  
  matrix.drawBitmap(0, 0, noseNeutral, 128, 32, matrix.Color333(faceR, faceG, faceB));
  matrix.drawBitmap(0, 0, mouthNeutral, 128, 32, matrix.Color333(faceR, faceG, faceB));
  matrix.drawBitmap(0, 0, eyeNeutral, 128, 32, matrix.Color333(faceR, faceG, faceB));
}

void blink(){
  matrix.fillRect(0,0,26,2,matrix.Color333(0,0,0));
  delay(20);
  matrix.fillRect(0,0,26,4,matrix.Color333(0,0,0));
  delay(20);
  matrix.fillRect(0,0,26,6,matrix.Color333(0,0,0));
  delay(20);
  matrix.fillRect(0,0,26,8,matrix.Color333(0,0,0));
  delay(20);
  matrix.fillRect(0,0,26,10,matrix.Color333(0,0,0));
  delay(50);
  matrix.drawBitmap(0, 0, eyeNeutral4, 128, 32, matrix.Color333(faceR, faceG, faceB));
  delay(20);
  matrix.drawBitmap(0, 0, eyeNeutral3, 128, 32, matrix.Color333(faceR, faceG, faceB));
  delay(20);
  matrix.drawBitmap(0, 0, eyeNeutral2, 128, 32, matrix.Color333(faceR, faceG, faceB));
  delay(20);
  matrix.drawBitmap(0, 0, eyeNeutral1, 128, 32, matrix.Color333(faceR, faceG, faceB));
  delay(20);
  matrix.drawBitmap(0, 0, eyeNeutral, 128, 32, matrix.Color333(faceR, faceG, faceB));
  delay(20);
}

void oledText(int x, int y, String inText){
  oled.clearDisplay(); //first make sure the screen is blank

  oled.setTextSize(1);             // Normal 1:1 pixel scale
  oled.setTextColor(SSD1306_WHITE);        // Draw white text
  oled.setCursor(x,y);
  oled.println(inText);

  oled.display();
}

void loop() {
  //read current battery voltage
  //batVolt = ((analogRead(35)/4095)*2);
  
  if(currentExpression == 0 && blinkEnable && millis() >= startTime+blinkTime){
    blink();
    startTime = millis();
    blinkTime = random(1,15)*1000;
  }

  if(io.digitalRead(noseSensor) == HIGH){
    oledText(0,0,"Booped");
    idling = false;
    boop();
    startTime = millis();
    blinkTime = random(1,15)*1000;
  }
  
  if(io.digitalRead(leftPinkie) == LOW){
    oledText(0,0,"LEFTPINKIE");
  } else if(io.digitalRead(leftRing) == LOW){
    //suprise
    oledText(0,0,"Suprise");
    idling = false;
    currentExpression = 5;
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.drawBitmap(0,0, noseSuprise, 128, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, mouthSuprise, 128, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, eyeSuprise, 128, 32, matrix.Color333(faceR, faceG, faceB));
    delay(500);
  } else if(io.digitalRead(leftMiddle) == LOW){
    //sad
    oledText(0,0,"Sad");
    idling = false;
    currentExpression = 4;
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.drawBitmap(0,0, noseSad, 128, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, mouthSad, 128, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, eyeSad, 128, 32, matrix.Color333(faceR, faceG, faceB));
    delay(500);
  } else if(io.digitalRead(leftIndex) == LOW){
    //angry
    oledText(0,0,"Angry");
    idling = false;
    currentExpression = 3;
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.drawBitmap(0,0, noseSad, 128, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, mouthAngry, 128, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, eyeAngry, 128, 32, matrix.Color333(faceR, faceG, faceB));
    delay(500);
  } else if(io.digitalRead(leftThumb) == LOW){
    
  } else if(io.digitalRead(rightThumb) == LOW){
    //blink enable
    blinkEnable = !blinkEnable;
    delay(500);
  } else if(io.digitalRead(rightIndex) == LOW){
    //neutral
    oledText(0,0,"Neutral");
    idling = false;
    currentExpression = 0;
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.drawBitmap(0,0, noseNeutral, 128, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, mouthNeutral, 128, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, eyeNeutral, 128, 32, matrix.Color333(faceR, faceG, faceB));
    delay(500);
  } else if(io.digitalRead(rightMiddle) == LOW){
    //happy
    oledText(0,0,"Happy");
    idling = false;
    currentExpression = 1;
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.drawBitmap(0,0, noseNeutral, 128, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, mouthHappy, 128, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, eyeHappy, 128, 32, matrix.Color333(faceR, faceG, faceB));
    delay(500);
  } else if(io.digitalRead(rightRing) == LOW){
    //love
    oledText(0,0,"Love");
    idling = false;
    currentExpression = 2;
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.drawBitmap(0,0, noseNeutral, 128, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, mouthHappy, 128, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, eyeHeart, 128, 32, matrix.Color333(faceR, faceG, faceB));
    delay(500);
  } else if(io.digitalRead(rightPinkie) == LOW){
    //idle
    oledText(0,0,"Idle");
    idling = true;
    delay(500);
  }

  if(idling == true){idle();}
}
