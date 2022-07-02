#include "TM1637.h"
#include "Keyboard.h"

#define CLK 9//pins definitions for TM1637 and can be changed to other ports
#define DIO 10
TM1637 tm1637(CLK,DIO);

#define ENCODER_CLK 2
#define ENCODER_DT  3
#define BUTTON 7

int lastClk = HIGH;
int CurThrottle = 0;
int mode = 0;

#define MODE_CNT 3
char opt[MODE_CNT][4] = {
  'Z','Q',',','.',
  'A','D','A','D',
  'A','D',';','\'',
};

void disp(){
  int cur = CurThrottle;
  if (cur < 0){
    tm1637.display(0,11);
    cur = -cur;
  }else{
    tm1637.display(0,15);
  }
  tm1637.display(1,cur);
  tm1637.display(3,mode);
}

void mode_change(){
  mode = (mode+1) % MODE_CNT;
  //Serial.print("MODE ");
  //Serial.println(mode);
  disp();
  delay(100);
}

void setup() {
  Serial.begin(115200);
  pinMode(ENCODER_CLK, INPUT);
  pinMode(ENCODER_DT, INPUT);

  tm1637.init();
  tm1637.set(BRIGHT_TYPICAL);//BRIGHT_TYPICAL = 2,BRIGHT_DARKEST = 0,BRIGHTEST = 7;
  disp();

  pinMode(BUTTON,INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON),mode_change,FALLING);
}



#define MAX_LEVELS 9

void loop() {
  int newClk = digitalRead(ENCODER_CLK);
  if (newClk != lastClk) {
    // CLK别针上有变化
    lastClk = newClk;
    int dtValue = digitalRead(ENCODER_DT);
    if (newClk == LOW && dtValue == HIGH) {
      //Serial.println("Rotated clockwise ⏩");
      if (CurThrottle < MAX_LEVELS){
        
        if (CurThrottle >= 0){
          Keyboard.press(opt[mode][0]);
          Keyboard.releaseAll();
        }else{
          Keyboard.press(opt[mode][2]);
          Keyboard.releaseAll();
        }
        CurThrottle++;
        disp();
      }
      delay(100);
    }
    if (newClk == LOW && dtValue == LOW) {
      //Serial.println("Rotated counterclockwise ⏪");
      if (CurThrottle > -1 * MAX_LEVELS){
        if (CurThrottle > 0){
          Keyboard.press(opt[mode][1]);
          Keyboard.releaseAll();
        }else{
          Keyboard.press(opt[mode][3]);
          Keyboard.releaseAll();
        }
        CurThrottle--;
        disp();
      }
      delay(100);
    }
  }
}
