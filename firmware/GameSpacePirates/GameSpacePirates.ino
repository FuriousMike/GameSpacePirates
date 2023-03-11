#define _LCD_TYPE 1  // для работы с I2C дисплеями
#include <EncButton2.h>
EncButton2<EB_BTN> encfire(INPUT, 2);   
EncButton2<EB_BTN> encmove(INPUT, 4);   
#include <LiquidCrystal_I2C.h>
LiquidCrystal_I2C lcd(0x27, 16, 2);
//---------------------------------------КОНСТАНТЫ---------------------------
const byte snowball = 0;
const byte spaceship = 1;
const byte spacepirate = 2;
const byte hithuge = 3;
const byte hit = 4;
const byte easymode = 5;
const byte normalmode = 6;
const byte hardmode = 7;
//---------------------------------------ПЕРЕМЕННЫЕ---------------------------
boolean isDiffScreenDrawed = 1;
byte GamePart;
byte MaxPirates;
int TimeFactor;
int PeriodOfAmmo;
int Shots;
int ammo = 1;
int q = 1;
int x;
int a;
int b;
int kills;
int GameLevel = 1;
int MinPeriodPirate;
int MaxPeriodPirate;
int SpeedOfGame;
int long t_print = 0;
int long t_pirates = 0;
int long t_last = 0;
long t_exit = 0;
byte myBalls[2][17];
byte hugeHits[2][17];
byte spacePirates[2][17];
byte hits[2][17];
//--------------------------------------ИКОНКИ ИГРЫ---------------------------
byte Snowball[] = {
  B00000,
  B00000,
  B01110,
  B11111,
  B11111,
  B11111,
  B01110,
  B00000
};
byte Spaceship[] = {
  0x1F,
  0x08,
  0x1E,
  0x0F,
  0x0F,
  0x1E,
  0x08,
  0x1F
};
byte Spacepirate[] = {
  0x1F,
  0x02,
  0x0F,
  0x1E,
  0x1E,
  0x0F,
  0x02,
  0x1F
};
byte Hithuge[] = {
  B01001,
  B10011,
  B11010,
  B01101,
  B10100,
  B01011,
  B11001,
  B10010
};
byte Hit[] = {
  B00000,
  B00100,
  B11010,
  B01111,
  B01010,
  B10101,
  B01001,
  B00000
};

byte EasyMode[] = {
  B01010,
  B00000,
  B10001,
  B01110,
  B00000,
  B00000,
  B00000,
  B00000
};

byte NormalMode[] = {
  B01010,
  B00000,
  B00000,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000
};

byte HardMode[] = {
  B01010,
  B00000,
  B00000,
  B01110,
  B10001,
  B00000,
  B00000,
  B00000
};

void setup() {
  Serial.begin(9600);  
  ClearAll();
  lcd.init();           // инициализация
  lcd.backlight();      // включить подсветку

 // Create new characters:
  lcd.createChar(0, Snowball);
  lcd.createChar(1, Spaceship);
  lcd.createChar(2, Spacepirate);
  lcd.createChar(3, Hithuge);
  lcd.createChar(4, Hit);
  lcd.createChar(5, EasyMode);
  lcd.createChar(6, NormalMode);
  lcd.createChar(7, HardMode);
  t_exit = millis();
}
void loop() {
  switch(GamePart) {
//---------------------------------------РЕЖИМ ВЫБОРА СЛОЖНОСТИ---------------------------
case 0:
    encfire.tick();
    encmove.tick();
    if (isDiffScreenDrawed) {
      GameLevel = 1;
      b = 0;
      DrawDiff();
      lcd.setCursor(3,1);
      lcd.cursor();
      isDiffScreenDrawed = 0;
    }
    if (encfire.press() == 1) {
     GeneralCleaning();
     SetDiff();
     GamePart = 1;
    }
    if (encmove.press()) {
      b = (b + 1) % 4;
      switch(b) {
        case 0:
          lcd.setCursor(3,1);
          GameLevel = 1;
        break;
        case 1:
          lcd.setCursor(6,1);
          GameLevel = 2;
        break;
        case 2:
          lcd.setCursor(9,1);
          GameLevel = 3;
        break;
        case 3:
          lcd.setCursor(12,1);
          GameLevel = 4;
        break;
      }
    }
  break;
//---------------------------------------ИГРОВАЯ ЧАСТЬ---------------------------
  case 1:  
    lcd.noCursor();
    Draw(spaceship,0,x%2);
    encfire.tick();
    encmove.tick();
   if (encmove.press()){  //кнопка для перемещений корабля
    x++;
    ClearCeil(0,(x-1)%2);
    Draw(spaceship,0, x%2);
  }

  if (encfire.press() && ammo>0) {  //кнопка для снарядов
    myBalls[x%2][1] = 1;
    ammo--;
    Shots++;
  }
  
  if (millis() - t_print >= SpeedOfGame) {
      for (byte j = 0; j< 2;j++) {         
        for (byte i = 0; i < 16; i++) {
          if (hugeHits[j][i] == 1) {
            hugeHits[j][i] = 0;
            ClearCeil(i,j);
          }
        }
      }

    if ((long)(q * PeriodOfAmmo) - (long)(millis() - t_exit) < 0) {     // увеличение боезапаса игрока
      ammo++;
      q++;
    }
    
    for (byte j = 0; j< 2;j++) {         
      for (byte i = 0; i < 16; i++) {
        hugeHits[j][i] = hits[j][i];
        hits[j][i] = 0;
      }
    }

    for (byte j = 0; j< 2;j++) {           // смещение снарядов
      for (byte i = 15; i > 0; i--) {
        if (myBalls[j][i] == 1) {
          ClearCeil(i,j);
        }
        myBalls[j][i] = myBalls[j][i-1];
      }
    }

    for (byte j = 0; j< 2;j++) {         // смещение пиратов
      for (byte i = 0; i < 17; i++) {
        if (spacePirates[j][i] == 1) {
          ClearCeil(i,j);
        }
        spacePirates[j][i] = spacePirates[j][i+1];
      }
    }
      t_print = millis();
  }
       // генерация пирата
    if (millis() - t_pirates >= MinPeriodPirate + (MaxPeriodPirate - MinPeriodPirate)*(1-DifficultOfGame()) && ActivePiratres() < MaxPirates) {
       spacePirates[random(0,2)][16] = 1;
       t_pirates = millis();
    }

    for (int k=-1; k<1; k++) {
        for (int j = 0; j< 2; j++) {
          for (int i = 1; i < 16; i++) { 
              if (myBalls[j][i] == 1 && spacePirates[j][i+k] == 1) {
                myBalls[j][i] = 0;
                ClearCeil(i,j);
                ClearCeil(i+k,j);
                spacePirates[j][i+k] = 0;
                hits[j][i] = 1;
                kills++;
              }
            }   // проверка на попадания
          }
        }

    for (int j = 0; j< 2;j++) {          
      for (int i = 0; i <16; i++) {
         if (hits[j][i] == 1) {
          Draw(hit,i,j);
          continue;
        }
        if (hugeHits[j][i] == 1) {
          Draw(hithuge,i,j);
          continue;
        }
        if (myBalls[j][i] == 1) {
          Draw(snowball,i,j);
          continue;
        }
        if (spacePirates[j][i] == 1) {
          Draw(spacepirate,i,j);
          continue;
        }
       
      }
    }
    for (int j = 0; j< 2;j++) {        // Проверка пробития
      for (int i = 15; i > 0; i--) {
            if (j == x%2 && i == 1 && spacePirates[j][i] == 1) {
              t_last = millis();
              t_last = (t_last - t_exit) * 0.001;
              ClearAll();
              GameOver(t_last);
              GamePart = 2;
            }
        }
    }
  break;
//--------------------------------ВЫВОД ПОСЛЕИГРОВОЙ СТАТИСТИКИ---------------------------
  case 2:
    encfire.tick();
    encmove.tick();
    if (encfire.press() == 1) {
     GeneralCleaning();
     isDiffScreenDrawed = 1;
     GamePart = 0;
    }
    if (encmove.press()) {
      a = (a + 1) % 4;
      switch(a) {
        case 0:
          GameOver(t_last);
        break;
        case 1:
          Statistics("Kills:",kills);
        break;
        case 2:
          Statistics("Moves:",x);
        break;
        case 3:
          Statistics("Shots:",Shots);
        break;
      }
    }
  break;
  }
}
//----------------------------------------ФУНКЦИИ-------------------------------
void ClearAll() {    // очистка всех активных состояний 
  for (int j = 0; j< 2;j++){
      for (int i = 0; i < 17; i++) {
        myBalls[j][i] = 0;
        hits[j][i] = 0;
        spacePirates[j][i] = 0;
        hugeHits[j][i] = 0;
      }
    }
}

void GameOver(int long st) {                // Выводит количество набранных очков
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print(F("You lose!"));
  lcd.setCursor(4,1);
  lcd.print("Score:"+ String(st));
}

void Statistics (String str, int value) {   // Выводит статистику на экран
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print(F("Statistics"));
  lcd.setCursor(4,1);
  lcd.print(str + String(value));
}

void ClearCeil(byte col, byte row) {   // Очищает выбранную клетку
  lcd.setCursor(col, row);
  lcd.print(" ");
}

void Draw(byte typeOfCustomChar, byte col, byte row) {   // Рисует выбранный символ
  lcd.setCursor(col, row);
  lcd.write(byte(typeOfCustomChar));
}

int ActivePiratres() {    // Вычисляет число активных пиратов
  int numberOfPirates = 0;
  for (byte j = 0; j< 2;j++) {         
    for (byte i = 0; i < 17; i++) {
      if (spacePirates[j][i] == 1) {
        numberOfPirates++;
      }
    }
  }
  return numberOfPirates;
}

double DifficultOfGame() {    // Функция планомерного роста сложности (наклонная синусоида с выходом на плато)
  double x = (double)(millis() - t_exit) / TimeFactor;
  double result = (double)0.9 + (1/(-x -1.0525)) + sin((x+0.0525)*10)*0.1;
  return result;
}

void GeneralCleaning() {
  t_exit = millis();
  q = 1;
  lcd.clear();
  ammo = 1;
  x = 0;
  Shots = 0;
  ClearAll();
  GamePart = 0;
  a = 0;
  kills = 0;
}

void DrawDiff() {
  lcd.clear();
  lcd.setCursor(3,0);
  lcd.print(F("Difficulty"));
  Draw(5,3,1);
  Draw(6,6,1);
  Draw(7,9,1);
  lcd.setCursor(12,1);
  lcd.print(char(239));
}

void SetDiff() {
  switch(GameLevel) {
    case 1:
      MaxPirates = 2;
      SpeedOfGame = 100;
      MinPeriodPirate = 1500;
      MaxPeriodPirate = 2500;
      TimeFactor = 10000;
      PeriodOfAmmo = 1000;
      break;
    case 2:
      MaxPirates = 4;
      SpeedOfGame = 100;
      MinPeriodPirate = 1000;
      MaxPeriodPirate = 1800;
      TimeFactor = 9000;
      PeriodOfAmmo = 1000;
      break;
    case 3:
      MaxPirates = 7;
      SpeedOfGame = 100;
      MinPeriodPirate = 500;
      MaxPeriodPirate = 1300;
      TimeFactor = 7000;
      PeriodOfAmmo = 1000;
      break;
    case 4:
      MaxPirates = 12;
      SpeedOfGame = 70;
      MinPeriodPirate = 100;
      MaxPeriodPirate = 1500;
      TimeFactor = 5000;
      PeriodOfAmmo = 800;
      break;
  }
}