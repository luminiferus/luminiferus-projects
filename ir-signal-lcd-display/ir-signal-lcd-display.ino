// LiquidCrystal_I2C library downloaded from:
// https://github.com/johnrickman/LiquidCrystal_I2C/archive/refs/tags/1.1.3.zip

#include <LiquidCrystal_I2C.h>
#include <IRremote.hpp>

const int IR_RECEIVE_PIN = 11;
LiquidCrystal_I2C lcd(0x27, 16, 2);

void setup() {
  Serial.begin(9600);
  delay(500);
  Serial.println("Display received IR signal data to an LCD1602 I2C screen");
  Serial.println();

  IrReceiver.begin(IR_RECEIVE_PIN, ENABLE_LED_FEEDBACK);

  lcd.init();
  lcd.backlight();
  lcd.setCursor(0,0);
  lcd.print("Infrared Signal");
  lcd.setCursor(0,1);
  lcd.print("Data Displayer");
}

void loop() {
  if (IrReceiver.decode()) {
    IrReceiver.printIRResultShort(&Serial);
    Serial.println();

    String protocolString = String(IrReceiver.getProtocolString());
    String addressString = "0x" + String(IrReceiver.decodedIRData.address, HEX);
    String commandString ="0x" + String(IrReceiver.decodedIRData.command, HEX); 
    String rawDataString= "0x" + String(IrReceiver.decodedIRData.decodedRawData, HEX);

    if (!(IrReceiver.decodedIRData.flags & IRDATA_FLAGS_IS_REPEAT)) {
      displayLabelAndValue("Protocol", protocolString);
      displayLabelAndValue("Address", addressString);
      displayLabelAndValue("Command", commandString);
      displayLabelAndValue("Raw Data", rawDataString);
    }

    delay(250);
    IrReceiver.resume();
  }
}

void displayLabelAndValue(String label, String value) {
   lcd.clear();
   lcd.setCursor(0,0);
   lcd.print(label);
   lcd.setCursor(0,1);
   lcd.print(value);
   delay(2500);
}
