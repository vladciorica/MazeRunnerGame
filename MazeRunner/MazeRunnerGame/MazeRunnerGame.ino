#include "LedControl.h" //  need the library
#include <LiquidCrystal.h>
#include <EEPROM.h>
//info stuff
const char gameName[] = "Maze Runner";
const char authorName[] = "Ciorica Vlad";
const char githubLink[] = "github.com/vladciorica/MazeRunner";

// pins
const int  dinPin = 12;
const int  clockPin = 11;
const int  loadPin = 10;


const int  RS = 7;
const int  enable = 9;
const int  d4 = 4;
const int  d5 = 3;
const int  d6 = 2;
const int  d7 = 8;
const int  brightnessPin = 5;
const int  contrastPin = 6;

const int  xPin = A0;
const int  yPin = A1;
const int  swPin = 0;

// data structures
LedControl lc = LedControl(dinPin, clockPin, loadPin, 1); //DIN, CLK, LOAD, No. DRIVER
LiquidCrystal lcd(RS, enable, d4, d5, d6, d7);


struct cursorPosition {
  int lcdRow, lcdCol;
  char* Name;
};


// menu stuff
const int  menuPositionsSize = 4;
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
const int  settingsSize = 7;
cursorPosition settingsCursorPos[settingsSize];
int currentSettingsPos = 0, lastSettingsPos = 0;
char currentUser[5] = "AAA\0";
int contrastValue = 3, brightnessValue = 3, currentLevel = 1, matrixBrightnessValue = 1;
int userCol = 6, userRow = 0;
const int contrastValues[] = {50, 75, 100, 120, 135, 150};
const int brightnessValues[] = {0, 50, 100, 150, 200, 250};

// menu game stuff
const char startGameMessage1[] = "Press to";
const char startGameMessage2[] = "start level ";
bool gameStarted = false;
bool gameEnded = false;
const int  maximumLevel = 5;
int gameScore = 0;
int startScore = 0;
int totalScore = 0;
long long int lastLedBlink = 0;
const int  blinkLedInterval = 200;

byte xPos = 0;
byte yPos = 0;

byte xLastPos = 0;
byte yLastPos = 0;

struct cursorPosition1 {
  int lcdRow, lcdCol;
};
// menu highscore stuff

const int  max_value = 10000;
int highScore[3] = {0, 0, 0};
char highScoreNames[3][4] = {"UNK", "UNK", "UNK"};
int currentScorePos = 0;
int lastScorePos = 0;
const int  scoresSize = 4;
cursorPosition1 scoreCursorPos[scoresSize];
bool newHighScore = false;;
bool scoreUpdated = false;


// joystick stuff
bool switchState = LOW;
bool lastState  = HIGH;

const int  minThreshold = 200;
const int  maxThreshold = 600;

bool joyMoved = false;
bool joyMoved2 = false;

//matrix stuff
const byte matrixSize = 8;

unsigned int long long lastMoved = 0;
const int  moveInterval = 150;

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

const int maximumMaps = 2;
const short numberOfMaps[] = {1, 1, 1, 2, 2};
const short startXPos[maximumLevel][maximumMaps] = {{0}, {0}, {0}, {0, 0}, {0, 0}};
const short startYPos[maximumLevel][maximumMaps] = {{0}, {0}, {0}, {0, 0}, {0, 5}};
const short endXPos[maximumLevel][maximumMaps] = {{7}, {7}, {7}, {7, 0}, {7, 7}};
const short endYPos[maximumLevel][maximumMaps] = {{5}, {6}, {5}, {7, 7}, {5, 0}};
int currentMap = 0;

//general logic stuff
bool inMenu = true;
bool inGame = false;
int blinkInterval = 300;
long long lastBlink = 0;

long long int lastBoost = 0;
long long int lastBoostBlink = 0;
int  blinkBoostInterval = 100;
const int boostTime[] = {1500, 1500, 1250, 1000, 1000};
const int boostInterval = 2000;
const int boostScore[] = {100, 200, 300, 400, 550};
short int xBoost = -1, yBoost = -1;

long long int lastWall = 0;
const int wallIntervals[] = {2000, 1750, 1500, 1250, 1000};
const int wallTime[] = {1500, 1500, 1250, 1000, 750};
short int xWall = -1, yWall = -1;


bool lockedIn = true;
bool blinkState = false;

int EEPROMNameAddress = 6;
int EEPROMSettingsAddress = 16;

void setup() {
  pinMode(brightnessPin, OUTPUT);
  pinMode(contrastPin, OUTPUT);
  pinMode(swPin, INPUT_PULLUP);
  pinMode(xPin, INPUT);
  pinMode(yPin, INPUT);

  Serial.begin(9600);
  loadDataEEPROM();
  updateSettings();
  lcdSetup();
  matrixSetup();
  cursorsSetup();
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
  lcd.clear();
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
  menuCursorPos[0].Name = new char[strlen("Start") + 1];
  strcpy(menuCursorPos[0].Name, "Start");
  menuCursorPos[1].lcdRow = 0;
  menuCursorPos[1].lcdCol = 7;
  menuCursorPos[1].Name = new char[strlen("Settings") + 1];
  strcpy(menuCursorPos[1].Name, "Settings");
  menuCursorPos[2].lcdRow = 1;
  menuCursorPos[2].lcdCol = 7;
  menuCursorPos[2].Name = new char[strlen("Info") + 1];
  strcpy(menuCursorPos[2].Name, "Info");
  menuCursorPos[3].lcdRow = 1;
  menuCursorPos[3].lcdCol = 0;
  menuCursorPos[3].Name = new char[strlen("Score") + 1];
  strcpy(menuCursorPos[3].Name, "Score");
  settingsCursorPos[0].lcdRow = 0;
  settingsCursorPos[0].lcdCol = 0;
  settingsCursorPos[0].Name = new char[strlen("Name: ") + 1];
  strcpy(settingsCursorPos[0].Name, "Name: ");
  settingsCursorPos[1].lcdRow = 1;
  settingsCursorPos[1].lcdCol = 0;
  settingsCursorPos[1].Name = new char[strlen("Start level: ") + 1];
  strcpy(settingsCursorPos[1].Name, "Start level: ");
  settingsCursorPos[2].lcdRow = 0;
  settingsCursorPos[2].lcdCol = 0;
  settingsCursorPos[2].Name = new char[strlen("Contrast: ") + 1];
  strcpy(settingsCursorPos[2].Name, "Contrast: ");
  settingsCursorPos[3].lcdRow = 1;
  settingsCursorPos[3].lcdCol = 0;
  settingsCursorPos[3].Name = new char[strlen("Brightness: ") + 1];
  strcpy(settingsCursorPos[3].Name, "Brightness: ");
  settingsCursorPos[4].lcdRow = 0;
  settingsCursorPos[4].lcdCol = 0;
  settingsCursorPos[4].Name = new char[strlen("Matrix Bness: ") + 1];
  strcpy(settingsCursorPos[4].Name, "Matrix Bness:");
  settingsCursorPos[5].lcdRow = 1;
  settingsCursorPos[5].lcdCol = 0;
  settingsCursorPos[5].Name = new char[strlen("Clear score: ") + 1];
  strcpy(settingsCursorPos[5].Name, "Clear Score");
  settingsCursorPos[6].lcdRow = 0;
  settingsCursorPos[6].lcdCol = 0;
  settingsCursorPos[6].Name = new char[strlen("Back: ") + 1];
  strcpy(settingsCursorPos[6].Name, "Back");
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
  EEPROM.update(address, number >> 8);
  EEPROM.update(address + 1, number & 0xFF);
}

void writeStringToEEPROM(int addrOffset, char* strToWrite)
{
  for (int i = 0; i < scoresSize - 1; i++)
  {
    EEPROM.update(addrOffset + i, strToWrite[i]);
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
  /*if(strlen(str) != 3)
    return false;*/
  for (int i = 0 ; i < strlen(str); i++)
    if (str[i] < 'A' || str[i] > 'Z')
      return false;
  return true;
}

void loadDataEEPROM()
{
  int EEPROMvalue;
  for (int i = 0; i < EEPROMNameAddress; i += 2)
  {
    if (readIntFromEEPROM(i) > 0)
      highScore[i / 2] = readIntFromEEPROM(i);
    else
      highScore[i / 2] = 0;
  }
  for (int i = EEPROMNameAddress; i < EEPROMNameAddress + 7; i = i + 3)
  {
    if (verifName(readStringFromEEPROM(i)) == true)
      strcpy(highScoreNames[(i - EEPROMNameAddress) / 3], readStringFromEEPROM(i));
    else
      strcpy(highScoreNames[(i - EEPROMNameAddress) / 3], "UNK");
  }
  if (verifName(readStringFromEEPROM(EEPROMSettingsAddress)) == true)
    strcpy(currentUser, readStringFromEEPROM(EEPROMSettingsAddress));
  else
    strcpy(currentUser, "AAA");
  for (int i = 19; i < 25; i = i + 2)
  {
    EEPROMvalue = readIntFromEEPROM(i);
    if (i == 19)
    {
      if (EEPROMvalue < 0 || EEPROMvalue > 5)
        contrastValue = 3;
      else
        contrastValue = EEPROMvalue;
    }
    else if (i == 21)
    {
      if (EEPROMvalue < 0 || EEPROMvalue > 5)
        brightnessValue = 3;
      else
        brightnessValue = EEPROMvalue;
    }
    else if (i == 23)
    {
      if (EEPROMvalue < 0 || EEPROMvalue > 5)
        matrixBrightnessValue = 3;
      else
        matrixBrightnessValue = EEPROMvalue;
    }
  }
}

// function that will print the menu data
void printMenu() {
  for (int i = 0 ; i < menuPositionsSize; i++)
  {
    lcd.setCursor(menuCursorPos[i].lcdCol + 1, menuCursorPos[i].lcdRow);
    lcd.print(menuCursorPos[i].Name);
  }
  lcd.setCursor(menuCursorPos[currentCursorMenuPos].lcdCol, menuCursorPos[currentCursorMenuPos].lcdRow);
  lcd.write(byte(0));
}

// function that will help us to navigate through the menu
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

bool buttonPressed() // function that returns true if the joystick button is pressed or false otherwise
{
  lastState = switchState;
  switchState = digitalRead(swPin);
  if (switchState == LOW && switchState != lastState)
  {
    return true;
  }
  return false;
}

/// general Menu logic
/// here we will generate all the data which will be shown on the lcd
void displayMenu() {
  /// if we are on the main menu and the button is pressed then we have to enter in the current selection
  if (buttonPressed() == true && currentCursorMenuPos != -1)
  {
    lcd.clear();
    lastcurrentCursorMenuPos = currentCursorMenuPos;
    currentCursorMenuPos = -1;
    /// for every selectuib from the menu we have to to some intializations
    if (lastcurrentCursorMenuPos == 0)
    {
      lcd.clear();
      gameEnded = false;
      gameStarted = false;
      inGame = false;
      gameScore = 0;
      totalScore = (currentLevel - 1) * 2000;
      xPos = startXPos[currentLevel - 1][0];
      yPos = startYPos[currentLevel - 1][0];
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
  /// we print the main menu if we did not select any field
  if (currentCursorMenuPos != -1)
  {
    printMenu();
    moveMenuCursor();
  }
  else if (currentCursorMenuPos == -1)
  {
    /// otherwise, we see what field we chose and develop the logic from that field
    if (lastcurrentCursorMenuPos == 0)
    {
      inGame = true;
      displayGame();
    }
    if (lastcurrentCursorMenuPos == 1)
    {
      displaySettings();
      // lockedIn will tell us if we chose a field from settings or not
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
          else if (currentSettingsPos == settingsSize - 2)
          {
            clearHighScore();
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
          clearDisplay();
        }
      }
      if (lockedIn == false)
      {
        modifySettings();
      }
    }
    if (lastcurrentCursorMenuPos == 2)
    {
      // we press the button to exit info
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

// function which will clear our highscores
void clearHighScore()
{
  for (int i = 0; i < EEPROMNameAddress + 9; i++)
    EEPROM.update(i, 0);
  for (int i = 0; i < scoresSize - 1; i++)
  {
    highScore[i] = 0;
    strcpy(highScoreNames[i], "UNK");
  }
}

// function that will help us to navigate in the score section
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

// function that will print the score section on the lcd

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

// here we update the scores
void updateScore()
{
  for (int i = 0 ; i < 3; i++)
    Serial.println(highScoreNames[i]);
  Serial.println(currentUser);
  for (int i = 0 ; i < scoresSize - 1; i++)
  {
    // if final score is lower that the score from highScore or it is not updated at all, we move all the scores down, and put on the current position the final score
    if (totalScore < highScore[i] or highScore[i] == 0)
    {
      for (int j = scoresSize - 2; j > i; j--)
      {
        highScore[j] = highScore[j - 1];
        strcpy(highScoreNames[j], highScoreNames[j - 1]);
      }
      highScore[i] = totalScore;
      strcpy(highScoreNames[i], currentUser);
      if (i == 0)
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
    Serial.println(highScoreNames[i]);
    writeStringToEEPROM(i * 3 + EEPROMNameAddress, highScoreNames[i]);
  }
}

inline void copyMatrix(bool levelStartMatrixAux[8][8])
{
  for (int i = 0 ; i < matrixSize; i++)
    for (int j = 0 ; j < matrixSize; j++)
      matrix[i][j] = levelStartMatrixAux[i][j];
}

void generateMatrix() //generate matrix for the new level
{
  xPos = startXPos[currentLevel - 1][currentMap];
  yPos = startYPos[currentLevel - 1][currentMap];
  if (currentLevel == 1)
  {
    bool levelStartMatrixAux[matrixSize][matrixSize] = {
      {1, 0, 1, 1, 1, 1, 1, 1},
      {1, 0, 0, 0, 0, 0, 0, 1},
      {1, 0, 1, 1, 1, 1, 1, 1},
      {1, 0, 1, 0, 0, 0, 0, 1},
      {1, 0, 1, 0, 1, 1, 0, 1},
      {1, 0, 1, 0, 1, 0, 0, 1},
      {1, 0, 0, 0, 1, 0, 1, 1},
      {1, 1, 1, 1, 1, 0, 1, 1}
    };
    copyMatrix(levelStartMatrixAux);
  }
  else if (currentLevel == 2)
  {
    bool levelStartMatrixAux[matrixSize][matrixSize] = {
      {1, 1, 1, 1, 1, 1, 1, 1},
      {0, 1, 0, 0, 0, 0, 0, 0},
      {0, 1, 0, 1, 1, 1, 1, 0},
      {0, 0, 0, 0, 1, 0, 0, 0},
      {0, 1, 1, 1, 1, 0, 1, 0},
      {0, 0, 0, 0, 1, 0, 1, 0},
      {0, 1, 1, 1, 0, 0, 1, 0},
      {0, 0, 0, 0, 0, 1, 0, 0}
    };
    copyMatrix(levelStartMatrixAux);
  }
  else if (currentLevel == 3)
  {
    bool levelStartMatrixAux[matrixSize][matrixSize] = {
      {0, 1, 0, 0, 0, 0, 0, 0},
      {0, 1, 0, 1, 1, 1, 1, 0},
      {0, 1, 0, 0, 0, 0, 1, 0},
      {0, 1, 0, 1, 1, 0, 1, 0},
      {0, 1, 0, 1, 1, 1, 1, 0},
      {0, 0, 0, 0, 1, 0, 0, 0},
      {0, 1, 1, 0, 1, 0, 1, 0},
      {0, 0, 0, 0, 1, 0, 1, 1}
    };
    copyMatrix(levelStartMatrixAux);
  }
  else if (currentLevel == 4)
  {
    if (currentMap == 0)
    {
      bool levelStartMatrixAux[matrixSize][matrixSize] = {
        {1, 0, 1, 1, 1, 0, 0, 0},
        {1, 0, 0, 0, 0, 0, 1, 0},
        {1, 0, 1, 1, 1, 1, 1, 0},
        {1, 0, 0, 0, 0, 0, 1, 0},
        {1, 0, 1, 0, 1, 0, 1, 0},
        {1, 0, 1, 0, 1, 0, 1, 0},
        {1, 0, 0, 0, 1, 0, 1, 1},
        {1, 1, 1, 1, 1, 0, 0, 0}
      };
      copyMatrix(levelStartMatrixAux);
    }
    else if (currentMap == 1)
    {
      bool levelStartMatrixAux[matrixSize][matrixSize] = {
        {1, 1, 1, 1, 1, 1, 1, 0},
        {0, 1, 0, 1, 0, 0, 0, 0},
        {0, 1, 0, 1, 0, 1, 1, 1},
        {0, 1, 0, 1, 0, 1, 0, 1},
        {0, 1, 0, 1, 0, 1, 0, 1},
        {0, 1, 0, 1, 0, 1, 0, 1},
        {0, 1, 0, 1, 0, 1, 0, 1},
        {0, 0, 0, 0, 0, 0, 0, 1}
      };
      copyMatrix(levelStartMatrixAux);
    }
  }
  else if (currentLevel == 5)
  {
    if (currentMap == 0)
    {
      bool levelStartMatrixAux[matrixSize][matrixSize] = {
        {1, 0, 1, 1, 1, 1, 1, 1},
        {1, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 1},
        {1, 0, 1, 0, 0, 0, 0, 1},
        {1, 0, 1, 0, 1, 1, 0, 1},
        {1, 0, 1, 0, 1, 0, 0, 1},
        {1, 0, 0, 0, 1, 0, 1, 1},
        {1, 1, 1, 1, 1, 0, 1, 1}
      };
      copyMatrix(levelStartMatrixAux);
    }
    else if (currentMap == 1)
    {
      bool levelStartMatrixAux[matrixSize][matrixSize] = {
        {1, 0, 0, 0, 0, 1, 0, 0},
        {1, 0, 1, 0, 1, 0, 1, 0},
        {1, 0, 1, 0, 1, 0, 1, 0},
        {1, 0, 1, 0, 0, 0, 1, 0},
        {1, 0, 1, 0, 1, 1, 1, 0},
        {1, 1, 1, 1, 1, 1, 1, 0},
        {1, 0, 0, 0, 0, 0, 0, 0},
        {0, 0, 1, 1, 1, 1, 1, 1}
      };
      copyMatrix(levelStartMatrixAux);
    }
  }
  /*for (int i = 0 ; i < matrixSize; i++)
      for (int j = 0; j < matrixSize; j++)
        matrix[i][j] = levelStartMatrix[currentLevel - 1][currentMap][i][j];
  */

  updateDisplay();
}

// here we print the infos in the game
void printGame()
{
  int currentScore = totalScore + gameScore;
  lcd.setCursor(0, 0);
  lcd.print("Level: ");
  lcd.print(currentLevel);
  lcd.setCursor(0, 1);
  lcd.print("Score: ");
  lcd.print(currentScore);
}

// here we print the infos at the start of the game/level
void printBeforeGame()
{
  lcd.setCursor(2, 0);
  lcd.print(startGameMessage1);
  lcd.setCursor(2, 1);
  lcd.print(startGameMessage2);
  lcd.print(currentLevel);
  //Serial.println(gameScore);
}

// here we print the infos at the end of the game
void printEndGame()
{
  if (newHighScore == false)
  {
    lcd.setCursor(0, 0);
    lcd.print("You scored:");
    lcd.print(totalScore);
    lcd.setCursor(0, 1);
    lcd.print("Press to go back");
  }
  else
  {
    lcd.setCursor(0, 0);
    lcd.print("You scored:");
    lcd.print(totalScore);
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

// game menu logic
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
      totalScore = 0;
      xPos = startXPos[currentLevel - 1][currentMap];
      yPos = startYPos[currentLevel - 1][currentMap];
      currentMap = 0;
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
      xBoost = -1;
      yBoost = -1;
      generateMatrix();
      lastLedBlink = millis();
      lastBoost = millis();
      lastWall = millis();
      currentMap = 0;
    }
  }
  else
  {
    printGame();
  }
}

// here we update the settings
void updateSettings()
{
  analogWrite(contrastPin, contrastValues[contrastValue]);
  analogWrite(brightnessPin, brightnessValues[brightnessValue]);
  lc.setIntensity(0, matrixBrightnessValue * 3);
  writeIntIntoEEPROM(19, contrastValue);
  writeIntIntoEEPROM(21, brightnessValue);
  writeIntIntoEEPROM(23, matrixBrightnessValue);
}

// here we change the settings values
void modifySettings()
{
  int auxValue;
  char auxName[4];
  int Size = 5;
  // if we change the name
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

    // we know we have the name from 6th to 8th position in the first row
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
        Serial.println(auxName);
        strcpy(currentUser, auxName);
        writeStringToEEPROM(EEPROMSettingsAddress, auxName);
        joyMoved2 = false;
      }
    }
  }
  // otherwise we have to change and integer value
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

    // for the starting level we have other boundaries
    if (currentSettingsPos == 1)
    {
      if (auxValue > Size)
        auxValue = 1;
      if (auxValue < 1)
        auxValue = Size;
    }
    else
    {
      if (auxValue > Size)
        auxValue = 0;
      if (auxValue < 0)
        auxValue = Size;
    }
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
      {
        matrixBrightnessValue = auxValue;
        lightDisplay();
      }
      // if something has changed we update the settings
      updateSettings();
    }
  }
}

void printSettings()
{
  // here we have the blink on the current field from settings
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
  // here we print the sections from settings
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

// here we move through the settings fields
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
  // just if we are lockedIn we can change the value of the settings
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

  //display logic
  // create menu for the game
  if (inMenu == true) {
    displayMenu();
  }
  // game logic
  if (inGame == true)
  {
    // if we are in game, then we update the score
    if (millis() % 10 == 0 && gameStarted == true && gameEnded == false)
    {
      gameScore = millis() / 10 - startScore;
    }
    // if we arrived at the exit and we are in the game then we need to go to next level/map or finish game
    if (xPos == endXPos[currentLevel - 1][currentMap] && yPos == endYPos[currentLevel - 1][currentMap]  && gameStarted == true && gameEnded == false)
    {
      // if we have more maps at the current level we go to the next map
      if (currentMap < numberOfMaps[currentLevel - 1] - 1)
      {
        currentMap++;
        xBoost = -1;
        yBoost = -1;
        xWall = -1;
        yWall = -1;
        lastBoost = millis();
        lastWall = millis();
        generateMatrix();
      }
      else // otherwise if we have levels to complete we go to the next level
      {
        if (currentLevel < maximumLevel)
        {
          lcd.clear();
          currentLevel++;
          gameStarted = false;
          totalScore += gameScore;
          currentMap = 0;
        }
        else // otherwise we finish the game
        {
          lcd.clear();
          gameStarted = false;
          gameEnded = true;
          currentLevel = 1;
          totalScore += gameScore;
          currentMap = 0;
        }
      }
    }
    // if we are in the game
    if (gameStarted == true && gameEnded == false)
    {
      // check if we moved on the matrix
      if (millis() - lastMoved > moveInterval)
      {
        updatePositions();
        lastMoved = millis();
      }
      generateScoreBoost(); // generate a score boost at a random position
      generateWalls(); // generate a wall at a random position
      blinkCurrentBoost(); 
      blinkCurrentPos();
      // if we moved on the matrix then we have to update the led matrix
      if (matrixChanged == true)
      {
        // matrix display logic
        updateDisplay();
        matrixChanged = false;
      }
    }
  }
}

void generateWalls()
{
  // generate a wall at a certain time interval
  if (millis() - lastWall > wallIntervals[currentLevel - 1])
  {
    if (xWall == -1 || yWall == -1)
    {
      xWall = random(0, matrixSize);
      yWall = random(0, matrixSize);
      while (matrix[xWall][yWall] == 1 || (xWall == xPos && yWall == yPos) || (xWall == xBoost && yWall == yBoost))
      {
        xWall = random(0, matrixSize);
        yWall = random(0, matrixSize);
      }
    }
    lastWall = millis();
    matrix[xWall][yWall] = 1;
    lc.setLed(0, xWall, yWall, matrix[xWall][yWall]);
  }
  // the wall stays on the map a certain moment of time
  if (millis() - lastWall > wallTime[currentLevel - 1])
  {
    matrix[xWall][yWall] = 0;
    lc.setLed(0, xWall, yWall, matrix[xWall][yWall]);
    xWall = -1;
    yWall = -1;
  }
}

void generateScoreBoost()
{
  // generate a score boost at a certain time interval
  if (millis() - lastBoost > boostInterval)
  {
    if (xBoost == -1 || yBoost == -1)
    {
      xBoost = random(0, matrixSize);
      yBoost = random(0, matrixSize);
      while (matrix[xBoost][yBoost] == 1 || (xBoost == xPos && yBoost == yPos))
      {
        xBoost = random(0, matrixSize);
        yBoost = random(0, matrixSize);
      }
    }
    lastBoost = millis();
    lastBoostBlink = millis();
  }
}

void blinkCurrentPos()
{
  // we make our current position to blink to know where we are
  if (millis() - lastLedBlink > blinkLedInterval)
  {
    matrix[xPos][yPos] = !matrix[xPos][yPos];
    lastLedBlink = millis();
    lc.setLed(0, xPos, yPos, matrix[xPos][yPos]);
  }
}

void blinkCurrentBoost()
{
  // the score boost stays on the map a fixed moment of time
  if (millis() - lastBoost < boostTime[currentLevel - 1])
  {
    if (millis() - lastBoostBlink > blinkBoostInterval && xBoost != -1 && yBoost != -1)
    {
      matrix[xBoost][yBoost] = !matrix[xBoost][yBoost];
      lastBoostBlink = millis();
      lc.setLed(0, xBoost, yBoost, matrix[xBoost][yBoost]);
    }
  }
  else
  {
    matrix[xBoost][yBoost] = 0;
    lc.setLed(0, xBoost, yBoost, matrix[xBoost][yBoost]);
    xBoost = -1;
    yBoost = -1;
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

void lightDisplay()
{
  for (int row = 0; row < matrixSize; row++)
  {
    for (int col = 0; col < matrixSize; col++)
    {
      lc.setLed(0, row, col, 1);
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
  }

  if (xValue > maxThreshold) {
    if (xPos < matrixSize - 1) {
      xPos++;
    }
  }

  if (xPos == xLastPos)
  {
    if (yValue > maxThreshold) {
      if (yPos > 0) {
        yPos--;
      }
    }

    if (yValue < minThreshold) {
      if (yPos < matrixSize - 1) {
        yPos++;
      }
    }
  }
  // if we are on a score boost
  if ((xLastPos != xPos || yLastPos != yPos) && (xPos == xBoost && yPos == yBoost))
  {
    matrix[xBoost][yBoost] = 0;
    xBoost = -1;
    yBoost = -1;
    totalScore -= boostScore[currentLevel - 1];
    matrixChanged = true;
    matrix[xLastPos][yLastPos] = 0;
  }
  // if we can go to this position
  else if ((xLastPos != xPos || yLastPos != yPos) && matrix[xPos][yPos] == 0) {
    matrixChanged = true;
    if (xPos != endXPos[currentLevel - 1][currentMap] || yPos != endYPos[currentLevel - 1][currentMap])
      matrix[xPos][yPos] = 1;
    matrix[xLastPos][yLastPos] = 0;
  }
  else
  {
    xPos = xLastPos;
    yPos = yLastPos;
  }
}
