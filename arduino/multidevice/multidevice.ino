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
int speedPins[] = {13,12,11,10};
int speed[]={0,0,0,0}; 
int dirByte=0xff;

const int minSpeed=-4;
const int maxSpeed=4;
int estSpeed=,estDirect=1;//estimate speed(1~8)
unsigned long tDrive=0,tBlueTooth=0;
int flag=0,recvB=0;
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

  tDrive=tBlueTooth=micros();
} 

void loop () 
{
  readBlueTooth();
  if(flag==1){
    if(recvB>='0' && recvB<='9'){
      estSpeed=  recvB=='0'?0xff:recvB-'0';
      for(int i=0;i<4;i++){
        speedUp(i,estSpeed);
      }      
    }
    else if(recvB=='i'){
      estDirect = 1;
      for(int i=0;i<4;i++){
        speedUp(i,estSpeed);
        turnDir(i,1);
      }      
    }
    else if(recvB=='l'){
      speedUp(0,estSpeed-8);
      speedUp(1,estSpeed-8);
      speedUp(2,estSpeed+8);      
      speedUp(3,estSpeed+8);      
    }
    else if(recvB=='j'){
      speedUp(0,estSpeed+8);
      speedUp(1,estSpeed+8);
      speedUp(2,estSpeed-8);      
      speedUp(3,estSpeed-8);      
    }
    else if(recvB=='k'){ 
      
      estDirect = -1;
      for(int i=0;i<4;i++){
        turnDir(i,-1);
      }
    }   
    else if(recvB=='u'){
      turnDir(0,1);
      turnDir(1,1);
      turnDir(2,-1);
      turnDir(3,-1);
    }
    else if(recvB=='o'){
      turnDir(0,-1);
      turnDir(1,-1);
      turnDir(2,1);
      turnDir(3,1);
    }
    flag=0;
  }
  drive();
}

void readBlueTooth(){
  if(flag==1 || micros()-tBlueTooth<1000){
    return;
  }
  else{
    tBlueTooth = micros();
  }

  if(analogRead(fbBT)<700 && true){
    if(Serial.available()>0){
      recvB=Serial.read();
      flag=1;
    }
  }
  else{
    if(bluetooth.available()>0){
      recvB=bluetooth.read();                                                                                                                                                                                                                                                                                                                                                                  
      Serial.write(recvB);
      flag=1;
    }
  }
}


void speedUp(int i,int level){
  if(level==0xff){
    turnDir(i,0);
    return;
  }
  speed[i] = 255 -(144-(level<=0?0:level>9?9:level)*16);

}

void turnDir(int index,int dir){
  int offset = index<<1;
  int dirBitA=1<<offset;
  int dirBitB=dirBitA<<1;

  if (dir==-1) 
  {
    dirByte=dirByte & (~dirBitA) | dirBitB;
    digitalWrite(LCK,LOW);
    shiftOut(DAT, CLK, MSBFIRST, dirByte);
    digitalWrite(LCK,HIGH);
  } 
  else if(dir==1)
  {

    dirByte=(dirByte | dirBitA) & (~dirBitB);
    digitalWrite(LCK,LOW);
    shiftOut(DAT, CLK, MSBFIRST, dirByte);
    digitalWrite(LCK,HIGH);
  } 
  else if(dir==0){
    dirByte=dirByte | dirBitA | dirBitB;
    digitalWrite(LCK,LOW);
    shiftOut(DAT, CLK, MSBFIRST, dirByte);
    digitalWrite(LCK,HIGH);
  }

}


int s=0;
void drive(){
  if(micros()-tDrive<30){
    return;
  }
  else{
    tDrive=micros();
    s=(s+1)&0xff;
  }
  for(int idx=0;idx<4;idx++){
    digitalWrite( speedPins[idx],s < speed[idx]?HIGH:LOW);
  }
}



