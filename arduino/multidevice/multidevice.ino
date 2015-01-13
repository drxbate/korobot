//define bluetooth

/*
0~8:speed,9:stop,0~3:backword,4~8:forward
                i:forward
j:trun left     k:back        l:trun right
*/
#include <SoftwareSerial.h>
SoftwareSerial bluetooth(9,8); //RX,TX
const int fbBT=1;

const int _step=32;
//Motor A 

const int DAT = 7;
const int LCK = 6;
const int CLK = 5;
int speedPins[] = {
  13,12,11,10};
int speed[]={
  0,0,0,0}; 
int dirByte=0xff;

const int minSpeed=-4;
const int maxSpeed=4;
int estSpeed=0;//estimate speed(1~8)
unsigned long time=0;
void setup () 
{

  for(int i=0;i<4;i++){
    pinMode(speedPins[i],OUTPUT);
  }
  pinMode(DAT,OUTPUT);
  pinMode(LCK,OUTPUT);
  pinMode(CLK,OUTPUT);
  //pinMode(POW,OUTPUT);
  //digitalWrite(POW,power);  

  Serial.begin(9600);
  /*
  for(int i=0;i<4;i++){
   turnDir(i,0);
   }*/

  bluetooth.begin(9600);

  time=micros();
} 

void loop () 
{

  int flag=0;
  int recvB = 0;

  if(analogRead(fbBT)<700 && true){
     if(Serial.available()>0){
       recvB=Serial.read();
       flag=1;
     }
  }
  else{
     if(bluetooth.available()>0){
    recvB=bluetooth.read();
    //                                                                                                                                                                                                                                                                                                                                                                    Serial.write(recvB);
    flag=1;
  }
 }




  if(flag==1){
    if(recvB>='0' && recvB<='9'){
      estSpeed=recvB=='9'?0xff:recvB-'4';
        for(int i=0;i<4;i++){
          speedUp(i,estSpeed);
      }      
    }
    else if(recvB=='i'){
      estSpeed=estSpeed==0xff?1:estSpeed;
      for(int i=0;i<4;i++){
        speedUp(i,estSpeed);
      }      
    }
    else if(recvB=='l'){
      speedUp(0,estSpeed+1);
      speedUp(1,estSpeed+1);
      speedUp(2,estSpeed-1);      
      speedUp(3,estSpeed-1);      
    }
    else if(recvB=='j'){            
      speedUp(0,estSpeed-1);
      speedUp(1,estSpeed-1);
      speedUp(2,estSpeed+1);      
      speedUp(3,estSpeed+1);      
    }
    else if(recvB=='k'){ 
      for(int i=0;i<4;i++){
        speedUp(i,-1);
      }
    }   
    

  }


  //set speed
  //analogWrite (speedPins[0], 255-speed[0]); 
  //analogWrite (speedPins[1], 255-speed[1]); 
  drive();
}

void speedUp(int i,int level){
  if(level==0xff){
    turnDir(i,0);
    return;
  }
  speed[i] = level>0?144-level*32:144+level*32;
  turnDir(i,level>=0?1:-1);
}

void turnDir(int index,int dir){
  int offset = index<<1;
  int dirBitA=1<<offset;
  int dirBitB=dirBitA<<1;

  // set direction
  if (1 == dir) 
  {
    dirByte=dirByte & (~dirBitA) | dirBitB;
    digitalWrite(LCK,LOW);
    shiftOut(DAT, CLK, MSBFIRST, dirByte);
    digitalWrite(LCK,HIGH);
    Serial.println(dirByte);
    /* 
     digitalWrite (dir1PinA , LOW); 
     digitalWrite (dir2PinA, HIGH); 
     */
  } 
  else if(dir==-1)
  {

    dirByte=(dirByte | dirBitA) & (~dirBitB);
    digitalWrite(LCK,LOW);
    shiftOut(DAT, CLK, MSBFIRST, dirByte);
    digitalWrite(LCK,HIGH);
    Serial.println(dirByte);
    /* 
     digitalWrite (dir1PinA , HIGH); 
     digitalWrite (dir2PinA, LOW); 
     */
  } 
  else if(dir==0){
    dirByte=dirByte | dirBitA | dirBitB;
    Serial.println(dirByte);
    /*
  digitalWrite (dir1PinA, HIGH); 
     digitalWrite (dir2PinA, HIGH); 
     */
    digitalWrite(LCK,LOW);
    shiftOut(DAT, CLK, MSBFIRST, dirByte);
    digitalWrite(LCK,HIGH);
  }

}


int s=0;
void drive(){
  //for(int s=0;s<255;s++){

    if(micros()-time<30){
      return;
    }
    else{
      time=micros();
      s=(s+1)&0xff;
    }
    for(int idx=0;idx<4;idx++){
      digitalWrite( speedPins[idx],s < 255-speed[idx]?HIGH:LOW);
    }
    //delayMicroseconds(30);
  //}
}

