#ifndef LCD_H
#define LCD_H
#include <LiquidCrystal.h>
#include <string>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(2, 3, 4, 5, 6, 9);
void printLCD(const String& top_row_msg, const String& bottom_row_msg);

void printLCD(const String& top_row_msg, const String& bottom_row_msg)
{

  // Synth Section (eg. Filter, OSC etc.)
  lcd.setCursor(0, 0);
  lcd.print(top_row_msg);

  // Value + Unit
  //const char* text=variable+" %-7d "+unit;
  lcd.setCursor(0, 1);
  lcd.print(bottom_row_msg);
}
#endif /* LCD_H */
