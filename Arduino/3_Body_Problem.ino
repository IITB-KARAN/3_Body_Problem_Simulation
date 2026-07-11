#include <Adafruit_GFX.h>
#include <Adafruit_ST7789.h>
#include <SPI.h>
#include <math.h>

#define TFT_CS   10
#define TFT_DC    8
#define TFT_RST   9

Adafruit_ST7789 tft = Adafruit_ST7789(TFT_CS, TFT_DC, TFT_RST);

#define WIDTH   320
#define HEIGHT  240

#define CX        160
#define CY        120
#define G_CONST   1.0
#define EPS       0.001
#define DT        0.005
#define BOB_R     2
#define BUTTON_PIN 2
#define MASS_BUTTON 3
#define RESET_BUTTON 4

float bx[3], by[3];
float bvx[3], bvy[3];
float bax[3], bay[3];
float bmass[3];
int   bpx[3], bpy[3];

uint16_t bbc[3];


float bscale;
int   boxX, boyY;

void computeAccel() {
  for (int i = 0; i < 3; i++) {
    bax[i] = 0;
    bay[i] = 0;

    for (int j = 0; j < 3; j++) {
      if (i == j) continue;

      float dx = bx[j] - bx[i];
      float dy = by[j] - by[i];
      float r2 = dx*dx + dy*dy;

      float r3 = pow(r2 + EPS*EPS, 1.5);
      

      bax[i] += G_CONST * bmass[j] * dx / r3;
      bay[i] += G_CONST * bmass[j] * dy / r3;
    }
  }
}


void leapfrog() {
  for (int i = 0; i < 3; i++) {
    bvx[i] += bax[i] * DT * 0.5;
    bvy[i] += bay[i] * DT * 0.5;
  }

  for (int i = 0; i < 3; i++) {
    bx[i] += bvx[i] * DT;
    by[i] += bvy[i] * DT;
  }

  computeAccel();

  for (int i = 0; i < 3; i++) {
    bvx[i] += bax[i] * DT * 0.5;
    bvy[i] += bay[i] * DT * 0.5;
  }
}



void drawSystem() {
  for (int i = 0; i < 3; i++) {

    // trail
    if (bpx[i] > 0 && bpx[i] < WIDTH && bpy[i] > 0 && bpy[i] < HEIGHT) {
      tft.drawPixel(bpx[i], bpy[i], bbc[i]);
    }
    

    int sx = (int)(boxX + bx[i] * bscale);
    int sy = (int)(boyY + by[i] * bscale);

    if (sx > 0 && sx < WIDTH && sy > 0 && sy < HEIGHT) {
      tft.fillCircle(sx, sy, BOB_R, bbc[i]);
    }

    bpx[i] = sx;
    bpy[i] = sy;
  }

  
  float cmx = 0, cmy = 0, totalMass = 0;

  for (int i = 0; i < 3; i++) {
    cmx += bmass[i] * bx[i];
    cmy += bmass[i] * by[i];
    totalMass += bmass[i];
  }

  cmx /= totalMass;
  cmy /= totalMass;

  int cx = (int)(boxX + cmx * bscale);
  int cy = (int)(boyY + cmy * bscale);

  if (cx > 0 && cx < WIDTH && cy > 0 && cy < HEIGHT) {
    tft.fillCircle(cx, cy, 2, ST77XX_WHITE);
  }

}


void threeBody(
  float x1, float y1,
  float x2, float y2,
  float x3, float y3,
  float vx1, float vy1,
  float vx2, float vy2,
  float vx3, float vy3,
  float m1, float m2, float m3,
  float scale,
  int offX, int offY,
  uint16_t c1, uint16_t c2, uint16_t c3
) {
  tft.fillScreen(ST77XX_BLACK);

  bx[0]=x1; by[0]=y1;
  bx[1]=x2; by[1]=y2;
  bx[2]=x3; by[2]=y3;

  bvx[0]=vx1; bvy[0]=vy1;
  bvx[1]=vx2; bvy[1]=vy2;
  bvx[2]=vx3; bvy[2]=vy3;

  bmass[0]=m1; bmass[1]=m2; bmass[2]=m3;

  bscale=scale;
  boxX=offX;
  boyY=offY;

  bbc[0]=c1;
  bbc[1]=c2;
  bbc[2]=c3;

  computeAccel();

  for (int i=0; i<3; i++) {
    bpx[i] = (int)(offX + bx[i]*scale);
    bpy[i] = (int)(offY + by[i]*scale);
    tft.fillCircle(bpx[i], bpy[i], BOB_R, bbc[i]);
  }
}

void showFigureEight() {
  threeBody(-1,0, 1,0, 0,0,
            0.3471,0.5327,
            0.3471,0.5327,
           -0.6942,-1.0654,
            1,1,1, 70, CX,CY,
            ST77XX_RED, ST77XX_GREEN, ST77XX_BLUE);
}
void BruckeA11() {
  threeBody(0.0132604844,0,
            1.4157286016,0,
            -1.4289890859,0,
            0,1.054151921,
            0,-0.2101466639,
            0,-0.8440052572,
            1,1,1, 70, CX,CY,
            ST77XX_RED, ST77XX_GREEN, ST77XX_BLUE);
}
void BruckeA14() {
  threeBody(-0.2637815221,0, 
            1.9698126146,0,
            -1.7060310924,0,
            0,0.9371630895,
            0,-0.1099503287,
            0,-0.8272127608,
            1,1,1, 50, CX,CY,
            ST77XX_RED, ST77XX_GREEN, ST77XX_BLUE);
}         
void yarn() {
  threeBody(-1,0, 
             1,0,
             0,0,
             0.464445,0.39606,
             0.464445,0.39606,
             -0.92889,-0.79212,
             1,1,1, 70, CX,CY,
             ST77XX_RED, ST77XX_GREEN, ST77XX_BLUE);
}    
void Bruckea3() {
  threeBody(0.3149337497,0, 
             0.812382071,0,
             -1.1273158206,0,
             0,1.4601869417,
             0,-0.5628292375,
             0,-0.8973577042,
             1,1,1, 70, CX,CY,
             ST77XX_RED, ST77XX_GREEN, ST77XX_BLUE);
}        
void loop_ended_triangle() {
  threeBody( 0.6661637520772179,-0.081921852656887,
             -0.025192663684493022,0.45444857588251897,
             -0.10301329374224,-0.765806200083609,
             0.84120297540307,0.029746212757039,
             0.142642469612081,-0.492315648524683,
             -0.98384544501151,0.462569435774018,
             1,1,1, 120, CX,CY,
             ST77XX_RED, ST77XX_GREEN, ST77XX_BLUE);
}


void showOrbit(int n) {
  if      (n==0) showFigureEight();
  else if (n==1) Bruckea3();
  else if (n==2) loop_ended_triangle();
  else if (n==3) yarn();
  else if (n==4) BruckeA11();
  else if (n==5) BruckeA14();

}

bool lastButtonState = HIGH;
bool currentButtonState;
int currentOrbit = 0;
bool lastMassButton = HIGH;
bool currentMassButton;
bool lastResetState = HIGH;
bool currentResetState;

float massValue = 1.0;

void setup() {
  tft.init(240, 320);   
  tft.setRotation(1);   
  tft.fillScreen(ST77XX_BLACK);
  pinMode(BUTTON_PIN, INPUT_PULLUP);
  pinMode(MASS_BUTTON, INPUT_PULLUP);
  pinMode(RESET_BUTTON, INPUT_PULLUP);
  showOrbit(currentOrbit);
}

void loop() {
  
  currentButtonState = digitalRead(BUTTON_PIN);


  if (lastButtonState == HIGH && currentButtonState == LOW) {

    currentOrbit++;

    if (currentOrbit > 5 ) currentOrbit = 0;

    showOrbit(currentOrbit);

    delay(200);   
  }

      
currentMassButton = digitalRead(MASS_BUTTON);

  if (lastMassButton == HIGH && currentMassButton == LOW) {

    massValue += 0.01;   // increase mass

    if (massValue > 1.1) massValue = 1; 

      bmass[0] = massValue;
    

    delay(200);
  }
 
  currentResetState = digitalRead(RESET_BUTTON);

  if (lastResetState == HIGH && currentResetState == LOW) {

    showOrbit(currentOrbit);   // reload same orbit

    delay(200); 
  }

  lastResetState = currentResetState;


  lastMassButton = currentMassButton;

  lastButtonState = currentButtonState;
    leapfrog();
    drawSystem();
  
  delay(5);
  int n=currentOrbit;
  tft.setCursor(5, 5);
  tft.setTextColor(ST77XX_WHITE);
  tft.setTextSize(2);
  if      (n==0) tft.print("8-orbit");
  else if (n==1) tft.print("BruckeA3");
  else if (n==2) tft.print("loop_ended_triangle");
  else if (n==3) tft.print("Yarn");
  else if (n==4) tft.print("BruckeA11");
  else if (n==5) tft.print("BruckeA14");
 
 
}