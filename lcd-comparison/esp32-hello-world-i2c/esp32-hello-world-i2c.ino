//www.elegoo.com
//2016.12.9

/*
 LiquidCrystal Hello World example modified to use LiquidCrystal_I2C library.

 LiquidCrystal_I2C library downloaded from:
 https://github.com/johnrickman/LiquidCrystal_I2C/archive/refs/tags/1.1.3.zip
 */

// include the library code:
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  // Print a message to the LCD.
  lcd.print("Hello, World!!!");
}

void loop() {
  // set the cursor to column 0, line 1
  // (note: line 1 is the second row, since counting begins with 0):
  lcd.setCursor(0, 1);
  // print the number of seconds since reset:
  lcd.print(millis() / 1000);
}

