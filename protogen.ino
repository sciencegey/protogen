//imports custom bitmaps
#include "idle.h"
#include "eyes.h"
#include "mouth.h"
#include "nose.h"

//import libraries
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
#define noseSensor  7

//finger switches
#define leftPinkie  53
#define leftRing    52
#define leftMiddle  51
#define leftIndex   50
#define leftThumb   49

#define rightThumb  48
#define rightIndex  47
#define rightMiddle 46
#define rightRing   45
#define rightPinkie 44


//global variables
unsigned long startTime = 0;  //current tick
int blinkTime = 0;  //how long until the next blink
bool blinkEnable = true;
int currentExpression = 0;  //which expression we are currrently in. 0=neutral, 1=happy, 2=love, 3=angry, 4=sad, 5=suprise

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
  /*
  pinMode(noseSensor, INPUT_PULLDOWN); //nose sensor - pulled low and goes high when booped

  //pin setup for finger switches. Using pulldown, so goes HIGH when button is pressed
  pinMode(leftPinkie, INPUT_PULLDOWN);
  pinMode(leftRing, INPUT_PULLDOWN);
  pinMode(leftMiddle, INPUT_PULLDOWN);
  pinMode(leftIndex, INPUT_PULLDOWN);
  pinMode(leftThumb, INPUT_PULLDOWN);
  pinMode(rightThumb, INPUT_PULLDOWN);
  pinMode(rightIndex, INPUT_PULLDOWN);
  pinMode(rightMiddle, INPUT_PULLDOWN);
  pinMode(rightRing, INPUT_PULLDOWN);
  pinMode(rightPinkie, INPUT_PULLDOWN);
  */
  //starts the matrix and writes all black to it
  matrix.begin(R1_PIN, G1_PIN, B1_PIN, R2_PIN, G2_PIN, B2_PIN, A_PIN, B_PIN, C_PIN, D_PIN, E_PIN, LAT_PIN, OE_PIN, CLK_PIN);
  
  matrix.fillScreen(matrix.Color333(0, 0, 0));  
  matrix.drawBitmap(0,0, noseNeutral, 64, 32, matrix.Color333(faceR, faceG, faceB));
  matrix.drawBitmap(0,0, mouthNeutral, 64, 32, matrix.Color333(faceR, faceG, faceB));
  matrix.drawBitmap(0, 0, eyeNeutral, 64, 32, matrix.Color333(faceR, faceG, faceB));

  startTime = millis();
  blinkTime = random(1,15)*1000;
}

void loop() {

  if(currentExpression == 0 && blinkEnable && millis() >= startTime+blinkTime){
    blink();
    startTime = millis();
    blinkTime = random(1,15)*1000;
  }

  /*if(digitalRead(noseSensor) == HIGH){
    idling = false;
    boop();
    startTime = millis();
    blinkTime = random(1,15)*1000;
  }*/
  if(digitalRead(leftPinkie) == LOW){
    
  } else if(digitalRead(leftRing) == LOW){
    //suprise
    idling = false;
    currentExpression = 5;
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.drawBitmap(0,0, noseSuprise, 64, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, mouthSuprise, 64, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, eyeSuprise, 64, 32, matrix.Color333(faceR, faceG, faceB));
    delay(500);
  } else if(digitalRead(leftMiddle) == LOW){
    //sad
    idling = false;
    currentExpression = 4;
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.drawBitmap(0,0, noseSad, 64, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, mouthSad, 64, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, eyeSad, 64, 32, matrix.Color333(faceR, faceG, faceB));
    delay(500);
  } else if(digitalRead(leftIndex) == LOW){
    //angry
    idling = false;
    currentExpression = 3;
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.drawBitmap(0,0, noseSad, 64, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, mouthAngry, 64, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, eyeAngry, 64, 32, matrix.Color333(faceR, faceG, faceB));
    delay(500);
  } else if(digitalRead(leftThumb) == LOW){
    
  } else if(digitalRead(rightThumb) == LOW){
    //blink enable
    blinkEnable = !blinkEnable;
    delay(500);
  } else if(digitalRead(rightIndex) == LOW){
    //neutral
    idling = false;
    currentExpression = 0;
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.drawBitmap(0,0, noseNeutral, 64, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, mouthNeutral, 64, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, eyeNeutral, 64, 32, matrix.Color333(faceR, faceG, faceB));
    delay(500);
  } else if(digitalRead(rightMiddle) == LOW){
    //happy
    idling = false;
    currentExpression = 1;
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.drawBitmap(0,0, noseNeutral, 64, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, mouthHappy, 64, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, eyeHappy, 64, 32, matrix.Color333(faceR, faceG, faceB));
    delay(500);
  } else if(digitalRead(rightRing) == LOW){
    //love
    idling = false;
    currentExpression = 2;
    matrix.fillScreen(matrix.Color333(0, 0, 0));
    matrix.drawBitmap(0,0, noseNeutral, 64, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, mouthHappy, 64, 32, matrix.Color333(faceR, faceG, faceB));
    matrix.drawBitmap(0,0, eyeHeart, 64, 32, matrix.Color333(faceR, faceG, faceB));
    delay(500);
  } else if(digitalRead(rightPinkie) == LOW){
    //idle
    idling = true;
    delay(500);
  }

  if(idling == true){idle();}
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
  matrix.drawBitmap(0, 0, eyeSuprise, 64, 32, matrix.Color333(faceR, faceG, faceB));
  matrix.drawBitmap(0, 0, noseSuprise, 64, 32, matrix.Color333(faceR, faceG, faceB));
  matrix.drawBitmap(0, 0, mouthSuprise, 64, 32, matrix.Color333(faceR, faceG, faceB));
  //wait a few seconds
  delay(2000);
  //then switch back to the neutral expression
  matrix.fillScreen(matrix.Color333(0, 0, 0));  
  matrix.drawBitmap(0, 0, noseNeutral, 64, 32, matrix.Color333(faceR, faceG, faceB));
  matrix.drawBitmap(0, 0, mouthNeutral, 64, 32, matrix.Color333(faceR, faceG, faceB));
  matrix.drawBitmap(0, 0, eyeNeutral, 64, 32, matrix.Color333(faceR, faceG, faceB));
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
  matrix.drawBitmap(0, 0, eyeNeutral4, 64, 32, matrix.Color333(faceR, faceG, faceB));
  delay(20);
  matrix.drawBitmap(0, 0, eyeNeutral3, 64, 32, matrix.Color333(faceR, faceG, faceB));
  delay(20);
  matrix.drawBitmap(0, 0, eyeNeutral2, 64, 32, matrix.Color333(faceR, faceG, faceB));
  delay(20);
  matrix.drawBitmap(0, 0, eyeNeutral1, 64, 32, matrix.Color333(faceR, faceG, faceB));
  delay(20);
  matrix.drawBitmap(0, 0, eyeNeutral, 64, 32, matrix.Color333(faceR, faceG, faceB));
  delay(20);
}
