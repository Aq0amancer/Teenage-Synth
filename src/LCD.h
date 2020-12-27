#ifndef LCD_H
#define LCD_H
#include <LiquidCrystal.h>
#include <string>

// initialize the library with the numbers of the interface pins
LiquidCrystal lcd(7, 8, 9, 10, 11, 12);
void printLCD(const char *top_row_msg, const char *bottom_row_msg);

void printLCD(const char *top_row_msg, const char *bottom_row_msg)
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
