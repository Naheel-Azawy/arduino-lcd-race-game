#include <LiquidCrystal.h>

#define PIN_BUTTON 2
#define POT_SPEED A0
#define POT_STEER A1

#define USE_POT

const int rs = 12, en = 11, d4 = 10, d5 = 9, d6 = 8, d7 = 7;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

#define MAX_DIST 16

#define LOC_UP   0
#define LOC_DOWN 1

#define CAR_UP_1      1
#define CAR_DOWN_1    2
#define CAR_UP_2      3
#define CAR_DOWN_2    4
#define STREET_UP_1   5
#define STREET_DOWN_1 6
#define STREET_UP_2   7
#define STREET_DOWN_2 8

byte car_up_1[8] = {
  0b10100,
  0b00000,
  0b00000,
  0b10010,
  0b01111,
  0b10010,
  0b00000,
  0b00000
};

byte car_down_1[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b10010,
  0b01111,
  0b10010,
  0b00000,
  0b10100
};

byte car_up_2[8] = {
  0b01010,
  0b00000,
  0b00000,
  0b10010,
  0b01111,
  0b10010,
  0b00000,
  0b00000
};

byte car_down_2[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b10010,
  0b01111,
  0b10010,
  0b00000,
  0b01010
};

byte street_up_1[8] = {
  0b10100,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

byte street_down_1[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b10100
};

byte street_up_2[8] = {
  0b01010,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000
};

byte street_down_2[8] = {
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b00000,
  0b01010
};

int distance;
int location;
int other_location;

int count;
int score;

char mat[2][MAX_DIST + 1];

void btn_click() {
  location = !location;
}

void reset() {
  count = 0;
  score = 1;
  distance = MAX_DIST - 1;
  location = LOC_UP;
  other_location = random(2);
}

char get_car_char(bool mode1, int location) {
  if (mode1) {
    return location == LOC_UP ? CAR_UP_1 : CAR_DOWN_1;
  } else {
    return location == LOC_UP ? CAR_UP_2 : CAR_DOWN_2;
  }
}

void setup() {

#ifndef USE_POT
  pinMode(PIN_BUTTON, INPUT);
  digitalWrite(PIN_BUTTON, HIGH);
  // Digital pin 2 maps to interrupt 0
  attachInterrupt(0/*PIN_BUTTON*/, btn_click, FALLING);
#else
  pinMode(POT_SPEED, INPUT);
  pinMode(POT_STEER, INPUT);
#endif

  lcd.begin(16, 2);
  lcd.createChar(CAR_UP_1, car_up_1);
  lcd.createChar(CAR_DOWN_1, car_down_1);
  lcd.createChar(CAR_UP_2, car_up_2);
  lcd.createChar(CAR_DOWN_2, car_down_2);
  lcd.createChar(STREET_UP_1, street_up_1);
  lcd.createChar(STREET_DOWN_1, street_down_1);
  lcd.createChar(STREET_UP_2, street_up_2);
  lcd.createChar(STREET_DOWN_2, street_down_2);

  reset();
  mat[0][MAX_DIST] = mat[1][MAX_DIST] = '\0';
}

void loop() {
  bool mode1 = count++ % 2 == 0;

  for (int i = 0; i < MAX_DIST; ++i) {
    if (mode1) {
      mat[0][i] = STREET_UP_1;
      mat[1][i] = STREET_DOWN_1;
    } else {
      mat[0][i] = STREET_UP_2;
      mat[1][i] = STREET_DOWN_2;
    }
  }

#ifdef USE_POT
  location = analogRead(POT_STEER) < 400 ? LOC_UP : LOC_DOWN;
#endif

  if (distance < 0) {
    if (location == other_location) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print("GAME OVER!");
      lcd.setCursor(0, 1);
      lcd.print("SCORE: ");
      lcd.print(score);
      reset();
      delay(3000);
      return;
    } else {
      other_location = random(2);
      distance = MAX_DIST - 1;
    }
    ++score;
  }

  mat[location][0] = get_car_char(mode1, location);
  mat[other_location][distance] = get_car_char(mode1, other_location);
  --distance;

  lcd.setCursor(0, 0);
  lcd.print(mat[0]);
  lcd.setCursor(0, 1);
  lcd.print(mat[1]);

  double sleep;

#ifndef USE_POT
  sleep = (10 + 500 / (double) score);
#else
  sleep = (10 + 500 / (analogRead(POT_SPEED) / 100.0));
#endif

  delay(sleep > 500 ? 500 : sleep);

}
