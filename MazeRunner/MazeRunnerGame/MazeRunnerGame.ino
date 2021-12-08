#include "LedControl.h" //  need the library
#include <LiquidCrystal.h>
#include <EEPROM.h>
//info stuff
const char gameName[] = "Maze Runner";
const char authorName[] = "Ciorica Vlad";
const char githubLink[] = "github.com/vladciorica/MazeRunner";

// pins
const int dinPin = 12;
const int clockPin = 11;
const int loadPin = 10;


const int RS = 7;
const int enable = 9;
const int d4 = 4;
const int d5 = 3;
const int d6 = 2;
const int d7 = 8;
const int brightnessPin = 5;
const int contrastPin = 6;

const int xPin = A0;
const int yPin = A1;
const int swPin = 0;

// data structures
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);


struct cursorPosition {
  int lcdRow, lcdCol;
  char Name[20];
};

// menu stuff
const int menuPositionsSize = 4;
cursorPosition menuCursorPos[menuPositionsSize];
int currentCursorMenuPos = 0;
int lastcurrentCursorMenuPos = 0;


long long lastScroll = 0;
int scrollInterval = 500;

byte arrow[8] = {
  B11000,
  B01100,
  B00110,
  B00011,
  B00110,
  B01100,
  B11000,
};

byte down[8] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B10001,
  B01010,
  B00100,
};

byte up[8] = {
  B00100,
  B01010,
  B10001,
  B00000,
  B00000,
  B00000,
  B00000,
};

// menu settings stuff
const int settingsSize = 6;
cursorPosition settingsCursorPos[settingsSize];
int currentSettingsPos = 0, lastSettingsPos = 0;
char currentUser[5] = "AAA\0";
int contrastValue = 3, brightnessValue = 3, currentLevel = 1, matrixBrightnessValue = 1;
int userCol = 6, userRow = 0;
int constrastValues[] = {0, 50, 100, 150, 200, 250};
int brightnessValues[] = {0, 50, 100, 150, 200, 250};

// menu game stuff
const char startGameMessage1[] = "Press to";
const char startGameMessage2[] = "start level ";
bool gameStarted = false;
bool gameEnded = false;
int endXPos = 7, endYPos = 5;
int maximumLevel = 1;
int gameScore = 0;
int startScore = 0;
long long int lastLedBlink = 0;
const int blinkLedInterval = 200;

byte xPos = 0;
byte yPos = 0;

byte xLastPos = 0;
byte yLastPos = 0;

// menu highscore stuff

const int max_value = 10000;
int highScore[3] = {0, 0, 0};
char highScoreNames[3][4] = {"UNK", "UNK", "UNK"};
int currentScorePos = 0;
int lastScorePos = 0;
const int scoresSize = 4;
cursorPosition scoreCursorPos[scoresSize];
bool newHighScore = false;;
bool scoreUpdated = false;


// joystick stuff
bool switchState = LOW;
bool lastState  = HIGH;

const int minThreshold = 200;
const int maxThreshold = 600;

bool joyMoved = false;
bool joyMoved2 = false;

//matrix stuff
const byte matrixSize = 8;

unsigned int long long lastMoved = 0;
const int moveInterval = 150;

bool matrixChanged = true;

bool matrix[matrixSize][matrixSize] = {
  {1, 0, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 1, 1, 1, 1, 1, 1},
  {1, 0, 1, 0, 0, 0, 0, 1},
  {1, 0, 1, 0, 1, 1, 0, 1},
  {1, 0, 1, 0, 1, 0, 0, 1},
  {1, 0, 0, 0, 1, 0, 1, 1},
  {1, 1, 1, 1, 1, 0, 1, 1}
};

bool levelStartMatrix[matrixSize][matrixSize] = {
  {1, 0, 1, 1, 1, 1, 1, 1},
  {1, 0, 0, 0, 0, 0, 0, 1},
  {1, 0, 1, 1, 1, 1, 1, 1},
  {1, 0, 1, 0, 0, 0, 0, 1},
  {1, 0, 1, 0, 1, 1, 0, 1},
  {1, 0, 1, 0, 1, 0, 0, 1},
  {1, 0, 0, 0, 1, 0, 1, 1},
  {1, 1, 1, 1, 1, 0, 1, 1}
};

//general logic stuff
bool inMenu = true;
bool inGame = false;
long long int blinkInterval = 300, lastBlink = 0;
bool lockedIn = true;
bool blinkState = false;

int EEPROMNameAddress = 6;

void setup() {
  pinMode(brightnessPin, OUTPUT);
  pinMode(contrastPin, OUTPUT);
  pinMode(swPin, INPUT_PULLUP);
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);
  
  Serial.begin(9600);
  matrixSetup();
  cursorsSetup();
  lcdSetup();
  loadDataEEPROM();
}

void matrixSetup()
{
  lc.shutdown(0, false); // turn off power saving, enables display
  lc.clearDisplay(0);// clear screen
}

void cursorsSetup()
{
  menuCursorPos[0].lcdRow = 0;
  menuCursorPos[0].lcdCol = 0;
  strcpy(menuCursorPos[0].Name, "Start");
  menuCursorPos[1].lcdRow = 0;
  menuCursorPos[1].lcdCol = 7;
  strcpy(menuCursorPos[1].Name, "Settings");
  menuCursorPos[2].lcdRow = 1;
  menuCursorPos[2].lcdCol = 7;
  strcpy(menuCursorPos[2].Name, "Info");
  menuCursorPos[3].lcdRow = 1;
  menuCursorPos[3].lcdCol = 0;
  strcpy(menuCursorPos[3].Name, "Score");
  settingsCursorPos[0].lcdRow = 0;
  settingsCursorPos[0].lcdCol = 0;
  strcpy(settingsCursorPos[0].Name, "Name: ");
  settingsCursorPos[1].lcdRow = 1;
  settingsCursorPos[1].lcdCol = 0;
  strcpy(settingsCursorPos[1].Name, "Start level: ");
  settingsCursorPos[2].lcdRow = 0;
  settingsCursorPos[2].lcdCol = 0;
  strcpy(settingsCursorPos[2].Name, "Contrast: ");
  settingsCursorPos[3].lcdRow = 1;
  settingsCursorPos[3].lcdCol = 0;
  strcpy(settingsCursorPos[3].Name, "Brightness: ");
  settingsCursorPos[4].lcdRow = 0;
  settingsCursorPos[4].lcdCol = 0;
  strcpy(settingsCursorPos[4].Name, "Matrix Bness:");
  settingsCursorPos[5].lcdRow = 1;
  settingsCursorPos[5].lcdCol = 0;
  strcpy(settingsCursorPos[5].Name, "Back");
  scoreCursorPos[0].lcdRow = 0;
  scoreCursorPos[0].lcdCol = 0;
  scoreCursorPos[1].lcdRow = 1;
  scoreCursorPos[1].lcdCol = 0;
  scoreCursorPos[2].lcdRow = 0;
  scoreCursorPos[2].lcdCol = 0;
  scoreCursorPos[3].lcdRow = 1;
  scoreCursorPos[3].lcdCol = 0;
}

void writeIntIntoEEPROM(int address, int number)
{
  EEPROM.write(address, number >> 8);
  EEPROM.write(address + 1, number & 0xFF);
}

void writeStringToEEPROM(int addrOffset, char* strToWrite)
{
  for (int i = 0; i < scoresSize - 1; i++)
  {
    EEPROM.write(addrOffset + i, strToWrite[i]);
  }
}

char* readStringFromEEPROM(int addrOffset)
{
  char* data = new char[4];
  for (int i = 0; i < scoresSize - 1; i++)
  {
    data[i] = EEPROM.read(addrOffset + i);
  }
  data[3] = '\0';
  return data;
}

int readIntFromEEPROM(int address)
{
  return (EEPROM.read(address) << 8) + EEPROM.read(address + 1);
}

bool verifName(char *str)
{
  for (int i = 0 ; i < strlen(str); i++)
    if (str[i] < 'A' || str[i] > 'Z')
      return false;
  return true;
}
void loadDataEEPROM()
{
  for (int i = 0; i < EEPROMNameAddress; i += 2)
  {
    if (readIntFromEEPROM(i) > 0)
      highScore[i / 2] = readIntFromEEPROM(i);
    else
      highScore[i / 2] = 0;
  }
  for (int i = EEPROMNameAddress; i < EEPROMNameAddress + 3; i++)
  {
    if (verifName(readStringFromEEPROM(i)) == true)
      strcpy(highScoreNames[i - EEPROMNameAddress], readStringFromEEPROM(i));
    else
      strcpy(highScoreNames[i - EEPROMNameAddress],"UNK");
  }
}

void lcdSetup()
{
  lcd.begin(16, 2);
  lcd.createChar(0, arrow);
  lcd.createChar(1, down);
  lcd.createChar(2, up);
  lcd.setCursor(3, 0);
  lcd.print("Welcome to");
  lcd.setCursor(3, 1);
  lcd.print(gameName);
  delay(2000);
  updateSettings();
  lcd.clear();
}

void printMenu() {
  for (int i = 0 ; i < menuPositionsSize; i++)
  {
    lcd.setCursor(menuCursorPos[i].lcdCol + 1, menuCursorPos[i].lcdRow);
    lcd.print(menuCursorPos[i].Name);
  }
  lcd.setCursor(menuCursorPos[currentCursorMenuPos].lcdCol, menuCursorPos[currentCursorMenuPos].lcdRow);
  lcd.write(byte(0));
}

void moveMenuCursor() {
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);

  int lastPos = currentCursorMenuPos;

  if (xValue < minThreshold) {
    if (currentCursorMenuPos == 3)
      currentCursorMenuPos = 0;
    if (currentCursorMenuPos == 2)
      currentCursorMenuPos = 1;
  }

  if (xValue > maxThreshold) {
    if (currentCursorMenuPos == 0)
      currentCursorMenuPos = 3;
    if (currentCursorMenuPos == 1)
      currentCursorMenuPos = 2;
  }

  if (lastPos == currentCursorMenuPos)
  {
    if (yValue > maxThreshold) {
      if (currentCursorMenuPos == 1)
        currentCursorMenuPos = 0;
      if (currentCursorMenuPos == 2)
        currentCursorMenuPos = 3;
    }

    if (yValue < minThreshold) {
      if (currentCursorMenuPos == 0)
        currentCursorMenuPos = 1;
      if (currentCursorMenuPos == 3)
        currentCursorMenuPos = 2;
    }
  }

  if (lastPos != currentCursorMenuPos)
  {
    lcd.clear();
  }

}

bool buttonPressed()
{
  lastState = switchState;
  switchState = digitalRead(swPin);
  if (switchState == LOW && switchState != lastState)
  {
    return true;
  }
  return false;
}


void displayMenu() {
  if (buttonPressed() == true && currentCursorMenuPos != -1)
  {
    lcd.clear();
    lastcurrentCursorMenuPos = currentCursorMenuPos;
    currentCursorMenuPos = -1;
    if (lastcurrentCursorMenuPos == 0)
    {
      lcd.clear();
      gameEnded = false;
      gameStarted = false;
      inGame = false;
      gameScore = 0;
      xPos = 0;
      yPos = 0;
      scoreUpdated = false;
      newHighScore = false;
      // waitAnimationDisplay
    }
    if (lastcurrentCursorMenuPos == 1)
    {
      lockedIn = true;
      joyMoved = false;
      joyMoved2 = false;
      currentSettingsPos = 0;
      lastSettingsPos = 0;
    }
    if (lastcurrentCursorMenuPos == 3)
    {
      joyMoved = false;
      joyMoved2 = false;
      currentScorePos = 0;
      lastScorePos = 0;
    }
  }
  if (currentCursorMenuPos != -1)
  {
    printMenu();
    moveMenuCursor();
  }
  else if (currentCursorMenuPos == -1)
  {
    if (lastcurrentCursorMenuPos == 0)
    {
      inGame = true;
      displayGame();
    }
    if (lastcurrentCursorMenuPos == 1)
    {
      displaySettings();
      if (buttonPressed() == true)
      {
        if (lockedIn == true)
        {
          if (currentSettingsPos == settingsSize - 1)
          {
            lcd.clear();
            lockedIn = true;
            currentCursorMenuPos = lastcurrentCursorMenuPos;
            return;
          }
          else
          {
            lockedIn = false;
            lastBlink = millis();
          }
        }
        else if (lockedIn == false)
        {
          lockedIn = true;
          joyMoved = false;
          joyMoved2 = false;
          userRow = 0;
          userCol = 6;
          updateSettings();
        }
      }
      if (lockedIn == false)
      {
        modifySettings();
      }
    }
    if (lastcurrentCursorMenuPos == 2)
    {
      displayInfo();
      if (buttonPressed() == true)
      {
        lcd.clear();
        currentCursorMenuPos = lastcurrentCursorMenuPos;
      }
    }
    if (lastcurrentCursorMenuPos == 3)
    {
      displayScore();
      if (buttonPressed() == true)
      {
        if (currentScorePos == scoresSize - 1)
        {
          lcd.clear();
          currentCursorMenuPos = lastcurrentCursorMenuPos;
          return;
        }
      }
    }
  }
}

void moveScoreCursor()
{
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);

  int lastPos = currentScorePos;

  if (xValue < minThreshold && joyMoved == false) {
    if (currentScorePos > 0)
    {
      currentScorePos--;
      joyMoved = true;
    }
  }

  if (xValue > maxThreshold && joyMoved == false) {
    if (currentScorePos < scoresSize - 1)
    {
      currentScorePos++;
      joyMoved = true;
    }
  }

  if (xValue > minThreshold && xValue < maxThreshold)
    joyMoved = false;

  if (lastPos != currentScorePos)
  {
    lcd.clear();
  }
  if (lastPos % 2 == 1 && currentScorePos == lastPos + 1) // in jos
  {
    lcd.clear();
    lastScorePos = currentScorePos;
  }
  if (lastPos % 2 == 0 && currentScorePos == lastPos - 1) // in sus
  {
    lcd.clear();
    lastScorePos = currentScorePos - 1;
  }
}

void printScore()
{

  lcd.setCursor(scoreCursorPos[currentScorePos].lcdCol, scoreCursorPos[currentScorePos].lcdRow);
  lcd.write(byte(0));
  for (int i = lastScorePos ; i < min(lastScorePos + 2, scoresSize); i++)
  {
    lcd.setCursor(scoreCursorPos[i].lcdCol + 1, scoreCursorPos[i].lcdRow);
    if (i < 3)
    {
      lcd.print(i + 1);
      lcd.print(".");
      lcd.print(highScoreNames[i]);
      lcd.print(": ");
      lcd.print(highScore[i]);
    }
    else if (i == 3)
      lcd.print("Back");
  }
  if (lastScorePos < scoresSize - 2)
  {
    lcd.setCursor(15, 1);
    lcd.write(byte(1));
  }
  if (lastScorePos > 1)
  {
    lcd.setCursor(15, 0);
    lcd.write(byte(2));
  }
}
void displayScore()
{
  printScore();
  moveScoreCursor();
}

void updateScore()
{
  for (int i = 0 ; i < scoresSize - 1; i++)
  {
    if (gameScore < highScore[i] or highScore[i] == 0)
    {
      for (int j = scoresSize - 2; j > i; j--)
      {
        highScore[j] = highScore[j - 1];
        strcpy(highScoreNames[j], highScoreNames[j - 1]);
      }
      highScore[i] = gameScore;
      strcpy(highScoreNames[i], currentUser);
      if(i == 0)
        newHighScore = true;
      break;
    }
  }
  for (int i = 0 ; i < scoresSize - 1; i++)
  {
    writeIntIntoEEPROM(i * 2, highScore[i]);
  }
  for (int i = 0 ; i < scoresSize - 1; i++)
  {
    writeStringToEEPROM(i + EEPROMNameAddress, highScoreNames[i]);
  }
}

void generateMatrix() //generate matrix for the new level
{
  for (int i = 0 ; i < matrixSize; i++)
    for (int j = 0; j < matrixSize; j++)
      matrix[i][j] = levelStartMatrix[i][j];
  updateDisplay();
}

void printGame()
{
  lcd.setCursor(0, 0);
  lcd.print("Level: ");
  lcd.print(currentLevel);
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(gameScore);
}
void printBeforeGame()
{
  lcd.setCursor(2, 0);
  lcd.print(startGameMessage1);
  lcd.setCursor(2, 1);
  lcd.print(startGameMessage2);
  lcd.print(currentLevel);
  //Serial.println(gameScore);
}

void printEndGame()
{
  if (newHighScore == false)
  {
    lcd.setCursor(0, 0);
    lcd.print("You scored:");
    lcd.print(gameScore);
    lcd.setCursor(0, 1);
    lcd.print("Press to go back");
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("You scored:");
    lcd.print(gameScore);
    lcd.print(" New Best Score");
    if (millis() - lastScroll > scrollInterval)
    {
      lcd.scrollDisplayLeft();
      lastScroll = millis();
    }
    lcd.setCursor(0, 1);
    lcd.print("Press to go back");
  }
}
void displayGame()
{
  if (gameEnded == true)
  {
    if ( scoreUpdated == false)
    {
      updateScore();
      scoreUpdated = true;
    }
    printEndGame();
    if (buttonPressed() == true)
    {
      lcd.clear();
      currentCursorMenuPos = lastcurrentCursorMenuPos;
      gameEnded = false;
      gameStarted = false;
      inGame = false;
      gameScore = 0;
      xPos = 0;
      yPos = 0;
      clearDisplay();
    }
    return;
  }
  if (gameStarted == false)
  {
    printBeforeGame();
    if (buttonPressed() == true)
    {
      lcd.clear();
      gameStarted = true;
      gameEnded = false;
      gameScore = 0;
      startScore = millis() / 10;
      generateMatrix();
      lastLedBlink = millis();
    }
  }
  else
  {
    printGame();
  }
}

void updateSettings()
{
  //analogWrite(contrastPin,constrastValues[constrastValue]);
  //analogWrite(brightnessPin,brightnessValues[brightnessValue]);
  lc.setIntensity(0, matrixBrightnessValue * 3);
}

void modifySettings()
{
  int auxValue;
  char auxName[4];
  int Size = 5;
  if (currentSettingsPos == 0)
  {
    strcpy(auxName, currentUser);
    int xValue = analogRead(xPin);
    int yValue = analogRead(yPin);

    int lastCol = userCol;

    if (yValue < minThreshold && joyMoved2 == false) {
      userCol++;
      joyMoved2 = true;
    }

    if (yValue > maxThreshold && joyMoved2 == false) {
      userCol--;
      joyMoved2 = true;
    }

    if (userCol < 6)
      userCol = 8;
    if (userCol > 8)
      userCol = 6;

    if (yValue > minThreshold && yValue < maxThreshold)
      joyMoved2 = false;

    if (userCol == lastCol)
    {
      if (xValue < minThreshold && joyMoved == false) {
        auxName[userCol - 6]++;
        joyMoved = true;
      }

      if (xValue > maxThreshold && joyMoved == false) {
        auxName[userCol - 6]--;
        joyMoved = true;
      }

      if (auxName[userCol - 6] > 'Z')
        auxName[userCol - 6] = 'A';
      if (auxName[userCol - 6] < 'A')
        auxName[userCol - 6] = 'Z';

      if (xValue > minThreshold && xValue < maxThreshold)
        joyMoved = false;

      if (strcmp(auxName, currentUser))
      {
        strcpy(currentUser, auxName);
        joyMoved2 = false;
      }
    }

  }
  else {
    if (currentSettingsPos == 1)
      auxValue = currentLevel;
    if (currentSettingsPos == 2)
      auxValue = contrastValue;
    if (currentSettingsPos == 3)
      auxValue = brightnessValue;
    if (currentSettingsPos == 4)
      auxValue = matrixBrightnessValue;

    int xValue = analogRead(xPin);
    int yValue = analogRead(yPin);

    int lastValue = auxValue;
    if (xValue < minThreshold && joyMoved == false) {
      auxValue++;
      joyMoved = true;
    }

    if (xValue > maxThreshold && joyMoved == false) {
      auxValue--;
      joyMoved = true;
    }

    if (auxValue > Size)
      auxValue = 1;
    if (auxValue < 0)
      auxValue = Size;

    if (xValue > minThreshold && xValue < maxThreshold)
      joyMoved = false;

    if (auxValue != lastValue)
    {
      if (currentSettingsPos == 1)
        currentLevel = auxValue;
      if (currentSettingsPos == 2)
        contrastValue = auxValue;
      if (currentSettingsPos == 3)
        brightnessValue = auxValue;
      if (currentSettingsPos == 4)
        matrixBrightnessValue = auxValue;
      //updateSettings();
    }
  }
}

void printSettings()
{
  if (lockedIn == false)
  {
    if (millis() - lastBlink > blinkInterval)
    {
      lastBlink = millis();
      blinkState = !blinkState;
      lcd.setCursor(settingsCursorPos[currentSettingsPos].lcdCol, settingsCursorPos[currentSettingsPos].lcdRow);
      if (blinkState == true)
        lcd.write(byte(0));
      else
        lcd.clear();
    }
  }
  else
  {
    lcd.setCursor(settingsCursorPos[currentSettingsPos].lcdCol, settingsCursorPos[currentSettingsPos].lcdRow);
    lcd.write(byte(0));
  }
  for (int i = lastSettingsPos ; i < min(lastSettingsPos + 2, settingsSize); i++)
  {
    lcd.setCursor(settingsCursorPos[i].lcdCol + 1, settingsCursorPos[i].lcdRow);
    lcd.print(settingsCursorPos[i].Name);
    if (i == 0)
      lcd.print(currentUser);
    else if (i == 1)
      lcd.print(currentLevel);
    else if (i == 2)
      lcd.print(contrastValue);
    else if (i == 3)
      lcd.print(brightnessValue);
    else if (i == 4)
      lcd.print(matrixBrightnessValue);
  }
  if (lastSettingsPos < settingsSize - 2)
  {
    lcd.setCursor(15, 1);
    lcd.write(byte(1));
  }
  if (lastSettingsPos > 1)
  {
    lcd.setCursor(15, 0);
    lcd.write(byte(2));
  }
}

void moveSettingsCursor()
{
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);

  int lastPos = currentSettingsPos;
  if (xValue < minThreshold && joyMoved == false) {
    if (currentSettingsPos > 0)
    {
      currentSettingsPos--;
      joyMoved = true;
    }
  }

  if (xValue > maxThreshold && joyMoved == false) {
    if (currentSettingsPos < settingsSize - 1)
    {
      currentSettingsPos++;
      joyMoved = true;
    }
  }

  if (xValue > minThreshold && xValue < maxThreshold)
    joyMoved = false;

  if (lastPos != currentSettingsPos)
  {
    lcd.clear();
  }
  if (lastPos % 2 == 1 && currentSettingsPos == lastPos + 1) // in jos
  {
    lcd.clear();
    lastSettingsPos = currentSettingsPos;
  }
  if (lastPos % 2 == 0 && currentSettingsPos == lastPos - 1) // in sus
  {
    lcd.clear();
    lastSettingsPos = currentSettingsPos - 1;
  }
}

void displaySettings()
{
  printSettings();
  if (lockedIn == true)
  {
    moveSettingsCursor();
  }
}

void displayInfo()
{
  lcd.setCursor(4, 0);
  lcd.print(gameName);
  lcd.print(" by ");
  lcd.print(authorName);
  lcd.setCursor(4, 1);
  lcd.print(githubLink);
  if (millis() - lastScroll > scrollInterval)
  {
    lcd.scrollDisplayLeft();
    lastScroll = millis();
  }
}

void loop() {

  // display logic
  // create menu for the game

  if (inMenu == true) {
    displayMenu();
  }
  // game logic
  if (inGame == true)
  {
    if (millis() % 10 == 0 && gameStarted == true && gameEnded == false)
    {
      gameScore = millis() / 10 - startScore;
    }
    if (xPos == endXPos && yPos == endYPos && gameEnded == false)
    {
      if (currentLevel < maximumLevel)
      {
        currentLevel++;
        gameStarted = false;
      }
      else
      {
        lcd.clear();
        gameStarted = false;
        gameEnded = true;
      }
    }
    if (gameStarted == true && gameEnded == false)
    {
      if (millis() - lastMoved > moveInterval)
      {
        updatePositions();
        lastMoved = millis();
      }
      blinkCurrentPos();
      if (matrixChanged == true)
      {
        // matrix display logic
        updateDisplay();
        matrixChanged = false;
      }
    }
  }
}

void blinkCurrentPos()
{
  Serial.println("vlad");
  if (millis() - lastLedBlink > blinkLedInterval)
  {
    Serial.print("timpul: ");
    Serial.println((double)lastLedBlink);
    matrix[xPos][yPos] = !matrix[xPos][yPos];
    lastLedBlink = millis();
    lc.setLed(0, xPos, yPos, matrix[xPos][yPos]);
  }
}
void updateDisplay() {
  for (int row = 0; row < matrixSize; row++)
  {
    for (int col = 0; col < matrixSize; col++)
    {
      lc.setLed(0, row, col, matrix[row][col]);
    }
  }
}

void clearDisplay()
{
  for (int row = 0; row < matrixSize; row++)
  {
    for (int col = 0; col < matrixSize; col++)
    {
      lc.setLed(0, row, col, 0);
    }
  }
}
void updatePositions() {
  int xValue = analogRead(xPin);
  int yValue = analogRead(yPin);

  xLastPos = xPos;
  yLastPos = yPos;

  if (xValue < minThreshold) {
    if (xPos > 0) {
      xPos--;
    }
    else {
      xPos = matrixSize - 1;
    }
  }

  if (xValue > maxThreshold) {
    if (xPos < matrixSize - 1) {
      xPos++;
    }
    else {
      xPos = 0;
    }
  }

  if (xPos == xLastPos)
  {
    if (yValue > maxThreshold) {
      if (yPos > 0) {
        yPos--;
      }
      else {
        yPos = matrixSize - 1;
      }
    }

    if (yValue < minThreshold) {
      if (yPos < matrixSize - 1) {
        yPos++;
      }
      else {
        yPos = 0;
      }
    }
  }
  if ((xLastPos != xPos || yLastPos != yPos) && matrix[xPos][yPos] == 0) {
    matrixChanged = true;
    if (xPos != endXPos || yPos != endYPos)
      matrix[xPos][yPos] = 1;
    matrix[xLastPos][yLastPos] = 0;
  }
  else
  {
    xPos = xLastPos;
    yPos = yLastPos;
  }
}
