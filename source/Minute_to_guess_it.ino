//Author: Liberty Kapungu 

#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include <Keypad.h>
#include <EEPROM.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

#define BUTTON_PIN 2

#define LED_1 6
#define LED_2 7
#define LED_3 8
#define LED_4 9

#define BUZZER_PIN 3

#define SEED_PIN A0

//Keypad Config
const int ROW_NUM = 4;     //four rows
const int COLUMN_NUM = 4;  //four columns

char keys[ROW_NUM][COLUMN_NUM] = {
  { '0', '1', '2', '3' },
  { '4', '5', '6', '7' },
  { '8', '9', 'A', 'B' },
  { 'C', 'D', 'E', 'F' }
};

byte pin_rows[ROW_NUM] = { A0, A1, A2, A3 };     //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {11, 10, 12, 13};  //connect to the column pinouts of the keypad

Keypad keypad = Keypad(makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM);

//Game Parameters
const int CORRECT_ANS = 2;
const int WRONG_ANS = -1;
const int GAME_DURATION = 60;  //Seconds

//Game Variables
unsigned int highScore = 0;
volatile unsigned int score = 0;
volatile unsigned int correctAns = 0;
volatile unsigned int totalAns = 0;
volatile int gameTimer = GAME_DURATION;
volatile bool gameRun = false;
bool gameIsOver = false;

//Button config
volatile uint8_t buttonState = HIGH;
volatile uint8_t buttonPressed = HIGH;
volatile unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 140;  //HUMAN FINGER REACTION TIME

int numberIndex = 0;

byte fourBitNumbers[] = {

  0b0000,  // 0
  0b0001,  // 1
  0b0010,  // 2
  0b0011,  // 3
  0b0100,  // 4
  0b0101,  // 5
  0b0110,  // 6
  0b0111,  // 7
  0b1000,  // 8
  0b1001,  // 9
  0b1010,  // 10
  0b1011,  // 11
  0b1100,  // 12
  0b1101,  // 13
  0b1110,  // 14
  0b1111   // 15
};

//Function Prototypes
void initialiseTimer1();
void initialiseButtons();
void setLEDs(int led1, int led2, int led3, int led4);
void initialiseLEDs();
void initialiseBuzzer();
void randomizeNumberArray();
void initialiseLCD();
void buttonPress();
void remainingTimeIndicator();
void indicateInputBuzz(bool isCorrect);
void gameStart();
void displayScore();
void updateGameState();
void checkUserAnswer();
int hexToInt(char hexChar);
void gameOver();


void setup() {

  initialiseTimer1();

  initialiseLCD();
  initialiseButtons();
  initialiseLEDs();
  randomizeNumberArray();
  initialiseBuzzer();

  // ### Uncomment to reset the value of the highscore in EEPROM  ### //
  EEPROM.write(0, 0x0); //Resets the EEPROM upon startup. 

  highScore = EEPROM.read(0);
}


void loop() {

  if (gameRun) {

    updateGameState();
  }

  else if (gameIsOver) {

    gameIsOver = false;
    gameOver();

  } else {

    return;
  }
}


void initialiseTimer1() {

  cli();  //Dissable Interrupts

  TCCR1A = 0;
  TCCR1B = 0;
  TCNT1 = 0;

  OCR1A = 15624;

  TCCR1B |= (1 << WGM12);

  //prescalar of 1024
  TCCR1B |= (1 << CS12) | (1 << CS10);

  TIMSK1 |= (1 << OCIE1A);

  sei();  //Allow interrupts
}


void initialiseButtons() {

  pinMode(BUTTON_PIN, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(BUTTON_PIN), buttonPress, FALLING);
}


void setLEDs(int led1, int led2, int led3, int led4) {

  digitalWrite(LED_1, led1);
  digitalWrite(LED_2, led2);
  digitalWrite(LED_3, led3);
  digitalWrite(LED_4, led4);
}


void initialiseLEDs() {

  pinMode(LED_1, OUTPUT);
  pinMode(LED_2, OUTPUT);
  pinMode(LED_3, OUTPUT);
  pinMode(LED_4, OUTPUT);

  setLEDs(1, 0, 0, 0);
}


void initialiseBuzzer() {

  pinMode(BUZZER_PIN, OUTPUT);

  tone(BUZZER_PIN, 2000, 150);
}


//Take array, randomize it.
void randomizeNumberArray() {

  randomSeed(analogRead(SEED_PIN));

  byte temp;

  for (int i = 0; i < 16; i++) {

    int randomIndex = random(i + 1);

    temp = fourBitNumbers[i];
    fourBitNumbers[i] = fourBitNumbers[randomIndex];
    fourBitNumbers[randomIndex] = temp;
  }
}


void initialiseLCD() {

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0, 0);
  //lcd.print("0b0 Number Guess");
  lcd.print("Minute to Guess");

  lcd.setCursor(0, 1);
  lcd.print("Press the Button");
}


void buttonPress() {

  //Debounce the reset Button
  if ((millis() - lastDebounceTime) < debounceDelay) {
    return;
  }

  lastDebounceTime = millis();

  uint8_t currentState = digitalRead(BUTTON_PIN);

  buttonState = currentState;

  if (buttonState == LOW) {
    gameStart();
  }
}


void remainingTimeIndicator() {

  if (gameTimer >= 45 && gameTimer <= 60) {
    setLEDs(1, 1, 1, 1);
  }

  if (gameTimer < 45 && gameTimer > 30) {
    setLEDs(0, 1, 1, 1);
  }

  if (gameTimer <= 30 && gameTimer > 15) {
    setLEDs(0, 0, 1, 1);
  }

  if (gameTimer < 15 && gameTimer >= 1) {
    setLEDs(0, 0, 0, 1);
  }

  if (gameTimer < 1) {
    setLEDs(0, 0, 0, 0);
    tone(BUZZER_PIN, 2000, 1000);
  }
}


void indicateInputBuzz(bool isCorrect) {

  if (isCorrect) {

    tone(BUZZER_PIN, 2900, 150);

  } else {

    tone(BUZZER_PIN, 130, 350);
  }
}


void gameStart() {

  randomizeNumberArray();

  score = 0;
  correctAns = 0;
  totalAns = 0;

  gameIsOver = false;
  gameRun = true;
  gameTimer = 60;
  setLEDs(1, 1, 1, 1);
}


void displayScore() {

  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("HighScore: ");
  lcd.print(highScore);

  lcd.setCursor(0, 1);
  lcd.print("SCR:");
  lcd.print(score);

  float accuracy = (float(correctAns) / float(totalAns)) * 100.0;

  lcd.setCursor(8, 1);
  lcd.print("ACY:");
  lcd.print((int)accuracy);
  lcd.print("%");
}


void updateGameState() {

  checkUserAnswer();

  if (gameTimer <= 0) {

    gameRun = false;
    gameIsOver = true;
  }
}


void checkUserAnswer() {

  static byte previousNumber = 255;  // Initialize with a value that is not valid

  byte currentNumber = fourBitNumbers[numberIndex];

  if (currentNumber != previousNumber) {

    lcd.clear();
    lcd.setCursor(4, 0);
    lcd.print("0b");

    // Print leading zeros
    for (int i = 3; i >= 0; i--) {

      if (bitRead(currentNumber, i) == 0) {

        lcd.print("0");

        if (i <= 1 && currentNumber == 0) {

          break;
        }

      } else {

        break;
      }
    }

    lcd.print(currentNumber, BIN);
    previousNumber = currentNumber;
  }

  char key = keypad.getKey();

  if (key) {

    int userInput = hexToInt(key);  // Convert hex character to integer

    if (userInput == fourBitNumbers[numberIndex]) {
      // Correct answer
      // Increment score
      score += CORRECT_ANS;
      correctAns++;
      indicateInputBuzz(true);
      numberIndex++;
      totalAns++;

    } else {
      // Incorrect answer
      // Decrement score
      if (score > 0) {

        score += WRONG_ANS;
      }

      indicateInputBuzz(false);
      numberIndex++;
      totalAns++;
    }

    if (numberIndex >= 15) {
      // All numbers have been guessed, randomize again
      randomizeNumberArray();
      numberIndex = 0;
    }
  }
}


//Convert hexadecimal characters to integers
int hexToInt(char hexChar) {

  if (hexChar >= '0' && hexChar <= '9') {
    return hexChar - '0';
  }

  else if (hexChar >= 'A' && hexChar <= 'F') {
    return hexChar - 'A' + 10;

  } else {
    return -1;  // Invalid input
  }
}


void gameOver() {

  if (score > highScore) {

    highScore = score;  // Update the high score if the current score is higher

    // Write the high score to EEPROM
    EEPROM.write(0, highScore);
    EEPROM.update(0, highScore);  // Save the data to EEPROM
  }

  displayScore();
}


ISR(TIMER1_COMPA_vect) {

  if (gameRun) {

    if (gameTimer > 0) {

      gameTimer--;

    } else if(gameTimer == 0) {

      gameRun = false;
      gameIsOver = true;
    }

    remainingTimeIndicator();
  }
}
