#include <LCDMenu.hpp>

#define NUMERTOSTART 7
#define PERCENT 0.15

uint8_t number = 0;

LCD_I2C lcd(0x27);

void setup() {
  lcd.begin();
  lcd.backlight();

  {
    LCDMenu& menu = LCDMenu::get(lcd);

    // Install mainMenu as the current menu to run
    menu.getNumber("Choose a number to get percent: ", (uint8_t) NUMERTOSTART, [](int v){
                          number = v * PERCENT;
                          });
  }//~LCDMenu is called when menu is out of scope

  //FROM NOW ON, CODE CAN MAKE USE OF GLOBAL VARIABLES SET BY GETNUMBER MENUS
}

void loop() {

}
