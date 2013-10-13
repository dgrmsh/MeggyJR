#include <MeggyJrSimple.h>

struct sn {
  byte x,y;
  sn * prev;
  sn * next;
};

enum directions {UP, RIGHT, DOWN, LEFT};
byte dir;
byte prevdir;
byte gameover;
byte newx,newy;
byte newfruitx,newfruity,fruitonboard;
byte snlen;

unsigned long Current;
unsigned long PrevMove;
unsigned long PrevFruit;

int MoveInt;
int FruitInt;

sn *head;
sn *p;
sn *nextp;

void MakeMove(byte inx, byte iny) {
  prevdir=dir;
  if(inx<0 || inx>7 || iny<0 || iny>7 || ReadPx(inx,iny)==Green) {
    gameover = 1;
  } else if(ReadPx(inx,iny)==Orange) {
    ++snlen;
    --fruitonboard;
    SetAuxLEDs(snlen);
  } else {
    p=head->next;
    while(p->next!=NULL) {
      p=p->next;
    }
    DrawPx(p->x,p->y,Dark);
    p=p->prev;
    free(p->next);
    p->next=NULL;
  }
  if(!gameover) {
    nextp =( sn * ) malloc(sizeof(sn));
    nextp->x = head->x; nextp->y = head->y;
    nextp->next = head->next;
    head->x=newx;
    head->y=newy;
    head->next = nextp;
    nextp->prev = head;
    nextp->next->prev = nextp;
    DrawPx(head->x,head->y,Green);
  }
}

void GameOverAnimation() {
  byte i;
  for(byte col=1;col<=4;++col) {
    for(i=col-1;i<=7-col;++i) {
      DrawPx(i,8-col,col);
      DisplaySlate();
      delay(50);
    }
    for(i=8-col;i>=col;--i) {
      DrawPx(8-col,i,col);
      DisplaySlate();    
      delay(50);      
    }
    for(i=8-col;i>=col;--i) {
      DrawPx(i,col-1,col);
      DisplaySlate();    
      delay(50);      
    }
    for(i=col-1;i<=7-col;++i) {
      DrawPx(col-1,i,col);
      DisplaySlate();    
      delay(50);      
    }    
  }
  delay(1000);
}

void GameSetup() {
  gameover=0;
  
  Current = millis();
  PrevMove = Current;
  PrevFruit = Current;
  MoveInt = 1000; FruitInt = 3000;
  fruitonboard=0;
  
  head = (sn*) malloc(sizeof(sn));
  head->x=4; head->y=4; head->prev=NULL;
  
  dir=UP;
  prevdir=UP;
  snlen=2;
  SetAuxLEDs(snlen);  
  
  p = (sn*) malloc(sizeof(sn));
  head->next = p;
  p->x = head->x; p->y = head->y-1;
  p->prev = head; p->next = NULL;
  
  DrawPx(head->x,head->y,Green);
  DrawPx(p->x,p->y,Green);
  
  newx=head->x; newy=head->y;
}

void setup() {
  MeggyJrSimpleSetup();
  GameSetup();
}

void loop() {
  CheckButtonsPress();
  if(Button_Up && prevdir!=UP && prevdir!=DOWN) {
    dir=UP;
  }
  if(Button_Right && prevdir!=RIGHT && prevdir!=LEFT) {
    dir=RIGHT;
  }
  if(Button_Down && prevdir!=UP && prevdir!=DOWN) {
    dir=DOWN;
  }
  if(Button_Left && prevdir!=RIGHT && prevdir!=LEFT) {  
    dir=LEFT;
  }
  
  Current = millis();
  if(Current - PrevMove >= MoveInt - 5*snlen) {
    PrevMove = Current;
    switch(dir) {
      case UP:
            newx = head->x;
            newy = head->y+1;
            break;
      case RIGHT: 
            newx = head->x+1;
            newy = head->y;      
            break;
      case DOWN: 
            newx = head->x;
            newy = head->y-1;
            break;
      case LEFT:
            newx = head->x-1;
            newy = head->y;
            break;
    }
    MakeMove(newx,newy);
    if(gameover) {
      delay(1000);
      GameOverAnimation();
      ClearSlate();
      DisplaySlate();      
      GameSetup();      
    }
  }
  DisplaySlate();  
  if(!gameover && fruitonboard<5 && Current - PrevFruit >= FruitInt || fruitonboard==0) {
    PrevFruit = Current;
    newfruitx=random(0,8); newfruity=random(0,8);
    while(ReadPx(newfruitx,newfruity)!=Dark) {
      newfruitx=random(0,8);
      newfruity=random(0,8);
    }
    ++fruitonboard;
    DrawPx(newfruitx,newfruity,Orange);
    DisplaySlate();    
  }
  delay(30);
}
