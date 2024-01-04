#include <Wire.h>
#include <Adafruit_RGBLCDShield.h>
#include <utility/Adafruit_MCP23017.h>
#include <EEPROM.h>

Adafruit_RGBLCDShield lcd = Adafruit_RGBLCDShield();

#define BLUE 0x4
#define VIOLET 0x5
#define WHITE 0x7
#define GREEN 0x2
#define RED 0x1
/* The following 4 constants allow main and settings menu pages
 * to circle all the way around.
 */
#define NUM_OPTIONS 3
#define BUTTONS 3
#define TIMES 7
#define NUM_SETTINGS 7

// This array of bytes creates a heart character
byte heart[] = {
  B00000,
  B01010,
  B11111,
  B11111,
  B01110,
  B00100,
  B00000,
  B00000
};

// This array of bytes creates an arrow pointing right
byte arrowRight[] = {
  B00000,
  B00100,
  B00110,
  B00111,
  B00110,
  B00100,
  B00000,
  B00000
};

// This array of bytes creates an arrow pointing left
byte arrowLeft[] = {
  B00000,
  B00100,
  B01100,
  B11100,
  B01100,
  B00100,
  B00000,
  B00000
};

// This array of bytes creates an arrow pointing up
byte arrowUp[] = {
  B00100,
  B01110,
  B11111,
  B00000,
  B00000,
  B00000,
  B00000,
  B00000
};

// This array of bytes creates an arrow pointing down
byte arrowDown[] = {
  B00000,
  B00000,
  B00000,
  B00000,
  B00000,
  B11111,
  B01110,
  B00100
};

// A random string from this array will be displayed when a user gets the sequence correct
String winStrings[] = {
  "Nice!",
  "Well Done!",
  "Great!",
  "Amazing!",
  "Good Job!",
  "Excellent!",
  "Perfect!"  
};

// A random string from this array will be displayed when a user makes a mistake
String failStrings[] = {
  "Fail!",
  "Oops!",
  "Try Again!",
  "Not Quite!",
  "Wrong!"
};

// This array contains all possible options for the number of buttons that are specified to use
String letterCombinations[] = {
  "UD",
  "UL",
  "UR",
  "DL",
  "DR",
  "LR",
  "UDL",
  "UDR",
  "ULR",
  "DLR",
  "UDLR"
};

// Main Menu options
String options[] = {
  "Story Mode",
  " Practice ",
  " Settings "
};

// Settings menu options
String settingsOpt[] = {
  " Difficulty ",
  "Buttons Used",
  "   Speed    ",
  "   Timer    ",
  "Reset Story ",
  "Set Defaults",
  "Back to Menu"
};

// Displays for set_difficulty() and set_speed()
String difficultyLevel[] = {
  "* ",
  "** ",
  "*** ",
  "**** ",
  "***** ",
  "****** ",
  "******* ",
  "******** ",
  "********* ",
  "********** ",
  "*********** ",
  "************"
};

// Displays for set_timer()
String timerOptions[] {
  "3",
  "4",
  "5",
  "6",
  "7",
  "8",
  "9"
};

//Displays for set_buttonsUsed()
String letterNum[] {
  "2",
  "3",
  "4"
};

// The following variables contain default starting values for all difficulty settings and Story Mode progression
byte lvlNum = 1; // can't go above 69
byte difficultySet = 0; // can't go above 11
byte letterSet = 0; // can't go above 2
byte speedSet = 0; // can't go above 11
byte timerSet = 6; // minimum is 0, maximum is at 6
byte optionNum = 0; // can't go above 2
byte settingNum = 0; // can't go above 6
int blankTime = 250; // max 266, min 68
int showTime = 1000; // max 1086, min 140

void startMenu() {
  /* This function sets up the diplay when Main Menu page opens
   *  It is called in void loop() everytime
   */
  optionNum = 0;
  lcd.clear();
  lcd.setBacklight(BLUE);
  lcd.setCursor(0,0);
  lcd.print(F("Choose an Option"));
  lcd.setCursor(3,1);
  lcd.print(options[0]);
  lcd.setCursor(0,1);
  lcd.print(F("<"));
  lcd.setCursor(15,1);
  lcd.print(F(">"));
  // Returns to Main Menu page in loop()
}

void startSettings() {
 /* This function sets up the diplay when Settings page opens
  *  It is called at the start of settings()
  */
  settingNum = 0;
  lcd.clear();
  lcd.setBacklight(BLUE);
  lcd.setCursor(0,0);
  lcd.print(F("Choose an Option"));
  lcd.setCursor(2,1);
  lcd.print(settingsOpt[0]);
  lcd.setCursor(0,1);
  lcd.print(F("<"));
  lcd.setCursor(15,1);
  lcd.print(F(">"));
  // Returns to main settings page in settings()
}

boolean challenge(byte num_letters,
                  byte word_length,
                  int show_time,
                  int delay_time,
                  byte timer) {
  /* Is called every time when a new sequence is to be created e.g each level in story_mode() and each round in practise_mode()
   * Uses 5 arguments to set the overall difficulty of a challenge
   * Returns True if user gets the sequence correct
   * Returns False if user made a mistake or ran out of time
   */
  String empty = "";
  String check = "";
  byte button = -1;
  String useLetters = "";
  byte r = 0;
  // Algorithm picks a random string from letterCombinations[] based on num_letters and uses those buttons for the next sequence
  switch(num_letters) {
    case 2:
        r = random(0,6);
        break;
    case 3:
        r = random(6,10);
        break;
    case 4:
        r = 10;
        break;
  }
  useLetters = letterCombinations[r];
 
  lcd.setCursor(5, 1);
  lcd.print(F("Ready?"));
  delay(show_time);
  lcd.setCursor(5, 1);
  lcd.print("      ");
  delay(delay_time);
  lcd.setCursor(6, 1);
  lcd.print(F("Go!"));
  delay(show_time);
  lcd.setCursor(6, 1);
  lcd.print("   ");
  delay(delay_time);
  // From here on a random "button" at a time will be generated and shown from the string of available buttons that was created beforehand
  for (byte x = 1; x <= word_length; x++) {
    byte n = random(0, useLetters.length());
    check += useLetters[n];
    lcd.setCursor(7, 1);
    switch(check[x -1]) {
      case 'R':
        lcd.write(1);
        break;
      case 'L':
        lcd.write(2);
        break;
      case 'U':
        lcd.write(3);
        break;
      case 'D':
        lcd.write(4);
        break;
    }
    delay(show_time);
    lcd.setCursor(7, 1);
    lcd.print(" ");
    delay(delay_time);
  }
 
  int start_time = millis();
  int end_time = start_time + (timer + 3) * 1000;
 
  /* The following loop starts as soon as the full sequence of characters has been shown to the user
   *  Starts the timer countdown, takes note of the buttons that a user pressed and compares the input to the existing sequence
   */
  while(empty.length() <= word_length - 1) {
    uint8_t buttons = lcd.readButtons();
    int current_time = end_time - millis();
    byte current_sec = current_time / 1000;
    byte current_milli = (current_time / 100) % 10;
    lcd.setCursor(12, 0);
    lcd.print(current_sec);
    lcd.setCursor(13, 0);
    lcd.print(F("."));
    lcd.setCursor(14, 0);
    lcd.print(current_milli);
    lcd.setCursor(empty.length(), 1);
    if (buttons & BUTTON_RIGHT) {
      empty += "R";
      lcd.write(1);
      while (lcd.readButtons() != 0) {}
    }
    if (buttons & BUTTON_LEFT) {
      empty += "L";
      lcd.write(2);
      while (lcd.readButtons() != 0) {}
    }
    if (buttons & BUTTON_UP) {
      empty += "U";
      lcd.write(3);
      while (lcd.readButtons() != 0) {}
    }
    if (buttons & BUTTON_DOWN) {
      empty += "D";
      lcd.write(4);
      while (lcd.readButtons() != 0) {}
    }
    // Returns False, turns the display red and displays a suitable message when a mistake is made in the input
    if (buttons & ~BUTTON_SELECT) {
      button = button + 1;
        if (empty[button] != check[button]) {
          lcd.clear();
          lcd.setBacklight(RED);
          lcd.setCursor(1,1);
          lcd.print(failStrings[random(0, 5)]);
          delay(2000L);
          lcd.clear();
          lcd.setBacklight(BLUE);
          return false;
        }
    }
    // Returns False, turns the display red and displays a suitable message when timer reaches 0
    if (current_time <= 0) {
      lcd.clear();
          lcd.setBacklight(RED);
          lcd.setCursor(1,1);
          lcd.print(F("Out of time"));
          delay(2000L);
          lcd.clear();
          lcd.setBacklight(BLUE);
          return false;
    }
  }
  // Returns True, turns the diplay green and displays suitable message when the input is correct
    lcd.clear();
    lcd.setBacklight(GREEN);
    lcd.setCursor(1,1);
    lcd.print(winStrings[random(0, 7)]);
    delay(1000L);
    lcd.clear();
    lcd.setBacklight(BLUE);
    return true;
    // returns to story_mode() or practise_mode()
}

void story_mode() {
  /* This function sets up the display and difficulty levels for the Story Mode
   * Changes difficulty based on level progression
   * Calls challenge() at the start of every level and sends the values for arguments to it
   * Updates EEPROM every time a new level is achieved and saves its number as a checkpoint
   * Starts with 3 lives and repeats until all of them are lost
   */
  byte lifeNum = 3;
  byte Speed = 0;
  byte buttonNum = 0;
  byte Length = 0;
  byte timeAllowed = 6;
  while ((lvlNum < 70) && (lifeNum > 0)) {
    lcd.setCursor(5, 0);
    lcd.print(F("Lvl "));
    lcd.setCursor(9, 0);
    lcd.print(lvlNum);
    for (byte x = 0; x < lifeNum; x++) {
      lcd.setCursor(x, 0);
      lcd.write(0);
    }
    /* Calculate N, M, D and T based on the level number for increasing difficulty e.g
     * lvlNum = 15, N = 7, M = 2, D = 398 mill & 122 mill, T = 5 sec
     * lvlNum = 16, N = 4, M = 3, D = 828 mill & 212 mill, T = 9 sec
     */
    byte a = (lvlNum - 1) % 3;
    switch(a) {
      case 0:
        Speed = 3;
        timeAllowed = 6;
        break;
      case 1:
        Speed = 5;
        timeAllowed = 4;
        break;
      case 2:
        Speed = 8;
        timeAllowed = 2;
        break;
    }
    showTime = -(86 * Speed) + 1086;
    blankTime = -(18 * Speed) + 266;
    if (lvlNum <= 15) {
      buttonNum = 2;
      Length = ((lvlNum - 1) / 3) + 4;
    }
    else if (lvlNum <= 42) {
      buttonNum = 3;
      Length = ((lvlNum - 16) / 3) + 4;
    }
    else {
      buttonNum = 4;
      Length = ((lvlNum - 43) / 3) + 7;
    }
    // Call challenge() and act on boolean result
    if (challenge(buttonNum, Length, showTime, blankTime, timeAllowed)) {
      lvlNum = lvlNum + 1;
      EEPROM.update(0, lvlNum);
    }
    else {
      lifeNum = lifeNum - 1;
    }
  }
  // Displays this when user reaches 0 lives
  lcd.setCursor(6, 0);
  lcd.print(F("Game"));
  lcd.setCursor(6, 1);
  lcd.print(F("Over"));
  delay(1000L);
  // returns back to Main Menu page in loop()

}

void practise_mode() {
  /* Sets up practise mode
   * Consists of 5 rounds in total
   * Calculates the percentage of correct answers and displays it at the end
   * Difficulty levels are based on Settings
   */
  showTime = -(86 * speedSet) + 1086;
  blankTime = -(18 * speedSet) + 266;
  byte count = 0;
  for (byte x = 0; x < 5; x++) {
    if (challenge(letterSet + 2, difficultySet + 4, showTime, blankTime, timerSet)) {
      count = count + 1;
    }
  }
  lcd.clear();
  byte percentage = (count * 100) / 5;
  lcd.setCursor(6, 0);
  lcd.print(percentage);
  lcd.setCursor(9, 0);
  lcd.print(F("%"));
  lcd.setCursor(4, 1);
  lcd.print(F("Accuracy"));
  delay(1000L);
  // returns back to Main Menu page in loop()
}

void set_buttonsUsed() {
  // Sets the number of buttons a user will be required to use and saves to EEPROM
  lcd.setCursor(2, 0);
  lcd.print(F("Buttons Used"));
  lcd.setCursor(0,1);
  lcd.print(F("-"));
  lcd.setCursor(15,1);
  lcd.print(F("+"));
  lcd.setCursor(7, 1);
  lcd.print(letterNum[letterSet]);
  while (true) {
    uint8_t buttons = lcd.readButtons();
    if (buttons & BUTTON_RIGHT) {
      lcd.setCursor(7, 1);
      letterSet = (letterSet + 1)%BUTTONS;
      lcd.print(letterNum[letterSet]);  
      while (lcd.readButtons() != 0) {}
    }
    if (buttons & BUTTON_LEFT) {
      lcd.setCursor(7, 1);
      if (letterSet == 0) {
        letterSet = BUTTONS - 1;
      } else {
        letterSet = letterSet - 1;
        }
      lcd.print(letterNum[letterSet]);
      while (lcd.readButtons() != 0) {}
    }
    if (buttons & BUTTON_SELECT) {
      while (lcd.readButtons() != 0) {}
      EEPROM.update(2, letterSet);
      startSettings();
      return;
    }
  }
  // Returns to main Settings page in settings()
}

void set_speed() {
  // Sets the value of global variable speedSet
  // This determines the length of time for which each character is shown as well as the "blank" time between each character is shown
  // Saves to EEPROM
  lcd.setCursor(5, 0);
  lcd.print(F("Speed"));
  lcd.setCursor(0, 1);
  lcd.print(F("-"));
  lcd.setCursor(15, 1);
  lcd.print(F("+"));
  lcd.setCursor(2, 1);
  lcd.print(difficultyLevel[speedSet]);
  while (true) {
    uint8_t buttons = lcd.readButtons();
    if (buttons & BUTTON_RIGHT) {
      lcd.setCursor(2, 1);
      if (speedSet < 11) {
        speedSet = speedSet + 1;
      }
      lcd.print(difficultyLevel[speedSet]);  
      while (lcd.readButtons() != 0) {}
    }
    if (buttons & BUTTON_LEFT) {
      lcd.setCursor(2, 1);
      if (speedSet > 0) {
        speedSet = speedSet - 1;
      }
      lcd.print(difficultyLevel[speedSet]);
      while (lcd.readButtons() != 0) {}
    }
    if (buttons & BUTTON_SELECT) {
      while (lcd.readButtons() != 0) {}
      EEPROM.update(3, speedSet);
      startSettings();
      return;
    }
  }
  // Returns to main Settings page in settings()
}

void set_timer() {
  // Sets the time allowed to finish the sequence and saves to EEPROM
  lcd.setCursor(5, 0);
  lcd.print(F("Timer"));
  lcd.setCursor(0, 1);
  lcd.print(F("-"));
  lcd.setCursor(15, 1);
  lcd.print(F("+"));
  lcd.setCursor(7, 1);
  lcd.print(timerOptions[timerSet]);
  while (true) {
    uint8_t buttons = lcd.readButtons();
    if (buttons & BUTTON_RIGHT) {
      lcd.setCursor(7, 1);
      timerSet = (timerSet + 1) % TIMES;
      lcd.print(timerOptions[timerSet]);  
      while (lcd.readButtons() != 0) {}
    }
    if (buttons & BUTTON_LEFT) {
      lcd.setCursor(7, 1);
      if (timerSet == 0) {
        timerSet = TIMES - 1;
      } else {
        timerSet = timerSet - 1;
        }
      lcd.print(timerOptions[timerSet]);
      while (lcd.readButtons() != 0) {}
    }
    if (buttons & BUTTON_SELECT) {
      while (lcd.readButtons() != 0) {}
      EEPROM.update(4, timerSet);
      startSettings();
      return;
    }
  }
  // Returns to main Settings page in settings()
}

void set_difficulty() {
  // Sets the length of a sequence and saves to EEPROM
  lcd.setCursor(2, 0);
  lcd.print(F("Difficulty"));
  lcd.setCursor(0, 1);
  lcd.print(F("-"));
  lcd.setCursor(15, 1);
  lcd.print(F("+"));
  lcd.setCursor(2, 1);
  lcd.print(difficultyLevel[difficultySet]);
  while (true) {
    uint8_t buttons = lcd.readButtons();
    if (buttons & BUTTON_RIGHT) {
      lcd.setCursor(2, 1);
      if (difficultySet < 11) {
        difficultySet = difficultySet + 1;
      }
      lcd.print(difficultyLevel[difficultySet]);  
      while (lcd.readButtons() != 0) {}
     
    }
    if (buttons & BUTTON_LEFT) {
      lcd.setCursor(2, 1);
      if (difficultySet > 0) {      
        difficultySet = difficultySet - 1;
      }
      lcd.print(difficultyLevel[difficultySet]);
      while (lcd.readButtons() != 0) {}
    }
    if (buttons & BUTTON_SELECT) {
      while (lcd.readButtons() != 0) {}
      EEPROM.update(1, difficultySet);
      startSettings();
      return;
    }
  }
  // Returns to main Settings page in settings()
}

void settings() {
  /* A function for main settings page
   * Allows to scroll all the way around the entire menu options
   * Calls other functions based on the option picked
   */
  startSettings();
  while (true) {
    uint8_t buttons = lcd.readButtons();
    if (buttons & BUTTON_RIGHT) {
      lcd.setCursor(2,1);
      settingNum = (settingNum + 1)%NUM_SETTINGS;
      lcd.print(settingsOpt[settingNum]);  
      while (lcd.readButtons() != 0) {}
    }
    if (buttons & BUTTON_LEFT) {
      lcd.setCursor(2,1);
      if (settingNum == 0) {
        settingNum = NUM_SETTINGS - 1;
      } else {
        settingNum = settingNum - 1;
        }
      lcd.print(settingsOpt[settingNum]);  
      while (lcd.readButtons() != 0) {}
    }
    if (buttons & BUTTON_SELECT) {
      lcd.clear();
      lcd.setCursor(0,0);
      while (lcd.readButtons() != 0) {}
      switch(settingNum) {
        case 0:
          set_difficulty();
          break;
        case 1:
          set_buttonsUsed();
          break;
        case 2:
          set_speed();
          break;
        case 3:
          set_timer();
          break;
        case 4:
          reset_story();
          break;
        case 5:
          reset_settings();
          break;
        case 6:
          return;
          // Returns to Main Menu page in settings()
      }
    }
  }
}

void reset_story() {
  // Sets Story Mode level to 1 and updates EEPROM
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(F("Resetting story"));
  lcd.setCursor(2, 1);
  lcd.print(F("mode progress"));
  delay(1000L);
  EEPROM.update(0, 1);
  lvlNum = 1;
  startSettings();
  return;
  // Returns to main Settings page in settings()
}

void reset_settings() {
  // Sets all settings for difficulty back to default values and updates EEPROM
  lcd.clear();
  lcd.setCursor(1, 0);
  lcd.print(F("Defaults reset"));
  delay(1000L);
  EEPROM.update(1, 0);
  EEPROM.update(2, 0);
  EEPROM.update(3, 0);
  EEPROM.update(4, 6);
  difficultySet = 0;
  letterSet = 0;
  speedSet = 0;
  timerSet = 6;
  startSettings();
  return;
  // Returns to main Settings page in settings()
}

void setup() {
  // Creates custom characters and makes sure that EEPROM values are in the allowed range
  lcd.begin(16, 2);
  lcd.createChar(0, heart);
  lcd.createChar(1, arrowRight);
  lcd.createChar(2, arrowLeft);
  lcd.createChar(3, arrowUp);
  lcd.createChar(4, arrowDown);
  lcd.print(F("Lets begin!"));
  lcd.setBacklight(WHITE);
  delay(2000L);
  // Check for Story mode's level number
  if (EEPROM.read(0) > 69) {
    lvlNum = 1;
    EEPROM.write(0, 1);
  }
  else {
    lvlNum = EEPROM.read(0);
  }
  // Checks for length N
  if (EEPROM.read(1) > 11) {
    difficultySet = 0;
    EEPROM.write(1, 0);
  }
  else {
    difficultySet = EEPROM.read(1);
  }
  // Checks for M
  if (EEPROM.read(2) > 2) {
    letterSet = 0;
    EEPROM.write(2, 0);
  }
  else {
    letterSet = EEPROM.read(2);
  }
  // Checks for D
  if (EEPROM.read(3) > 11) {
    speedSet = 0;
    EEPROM.write(3, 0);
  }
  else {
    speedSet = EEPROM.read(3);
  }
  // Checks for T
  if (EEPROM.read(4) > 6) {
    timerSet = 6;
    EEPROM.write(4, 6);
  }
  else {
    timerSet = EEPROM.read(4);
  }
  startMenu();
}

void loop() {
  // This loop is the Main Menu page
  uint8_t buttons = lcd.readButtons();
  if (buttons & BUTTON_RIGHT) {
    lcd.setCursor(3,1);
    optionNum = (optionNum + 1)%NUM_OPTIONS;
    lcd.print(options[optionNum]);  
    while (lcd.readButtons() != 0) {}
  }
  if (buttons & BUTTON_LEFT) {
    lcd.setCursor(3,1);
    if (optionNum == 0) {
      optionNum = NUM_OPTIONS - 1;
    } else {
      optionNum = optionNum - 1;
      }
    lcd.print(options[optionNum]);  
    while (lcd.readButtons() != 0) {}
  }
  if (buttons & BUTTON_SELECT) {
    lcd.clear();
    lcd.setCursor(0,0);
    while (lcd.readButtons() != 0) {}
    switch(optionNum) {
      case 0:
        story_mode();
        break;
      case 1:
        practise_mode();
        break;
      case 2:
        settings();
        break;
    }
    startMenu();
  }
}
