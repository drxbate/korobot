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
int speed[]={
  0,0,0,0}; 
int dirByte=0xff;

const int minSpeed=-4;
const int maxSpeed=4;
int estSpeed=0;//estimate speed(1~8)
unsigned long tDrive[]={0,0,0,0},tBlueTooth=0;
int flag=0,recvB=0;
void setup () 
{

  for(int i=0;i<4;i++){
    pinMode(speedPins[i],OUTPUT);
  }
  pinMode(DAT,OUTPUT);
  pinMode(LCK,OUTPUT);
  pinMode(CLK,OUTPUT);
  
  Serial.begin(9600);

  bluetooth.begin(9600);

  //tDrive=tBlueTooth=micros();
} 

void loop () 
{
  readBlueTooth();
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
      speedUp(0,estSpeed+2);
      speedUp(1,estSpeed+2);
      speedUp(2,estSpeed-2);      
      speedUp(3,estSpeed-2);      
    }
    else if(recvB=='j'){            
      speedUp(0,estSpeed-2);
      speedUp(1,estSpeed-2);
      speedUp(2,estSpeed+2);      
      speedUp(3,estSpeed+2);      
    }
    else if(recvB=='k'){ 
      for(int i=0;i<4;i++){
        speedUp(i,-1);
      }
    }   
    flag=0;
  }
  drive();
}

void readBlueTooth(){
  int tmpT = micros();
  if(flag==1 || tmpT-tBlueTooth<7680){
    return;
  }
  else{
    tBlueTooth = tmpT;
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
  speed[i] = level>0?144-level*32:144+level*32;
  turnDir(i,level>=0?1:-1);
}

void turnDir(int index,int dir){
  int offset = index<<1;
  int dirBitA=1<<offset;
  int dirBitB=dirBitA<<1;

  if (1 == dir) 
  {
    dirByte=dirByte & (~dirBitA) | dirBitB;
    digitalWrite(LCK,LOW);
    shiftOut(DAT, CLK, MSBFIRST, dirByte);
    digitalWrite(LCK,HIGH);
  } 
  else if(dir==-1)
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


int s[]={0,0,0,0};
void drive(){
  for(int idx=0;idx<4;idx++){
    int tmpT = micros();
    if(tmpT-tDrive[idx]<30){
      continue;
    }
    else{
      tDrive[idx]=tmpT;
      digitalWrite( speedPins[idx],s[idx] < 255-speed[idx]?HIGH:LOW);
      s[idx]=(s[idx]+1)&0xff;
    }


  }
}



