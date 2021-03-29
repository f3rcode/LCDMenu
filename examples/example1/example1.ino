#include <LCDMenu.hpp>

void foo();

const LCDMenu& menu = LCDMenu::get();

///////////////////////////////////////////////////////////////////////////////
// Main menu
///////////////////////////////////////////////////////////////////////////////
const char mainMenuTempeh[]  = "1 - Tempeh";
const char mainMenuNatto[]  = "2 - Natto";
const char mainMenuKoji[]  = "3 - Koji";


// Forward declarations for the config-menu referenced before it is defined.
extern const LCDMenuEntry configMenu[];
extern const uint8_t configMenuSize;

// Define the main menu
const LCDMenuEntry mainMenu[] = {
  {mainMenuTempeh, false, '1', [](){Serial.println("Go!");

                                    foo();
                                  } },
  {mainMenuNatto, false, '2', [](){
                                  Serial.println("Go!");

                                  foo();
                                  } },
  {mainMenuKoji, false, '3', [](){Serial.println("Still not available.");
                                  delay(500);
                                  menu.show();} },

};
constexpr uint8_t mainMenuSize = GET_MENU_SIZE(mainMenu);

void foo(){
  Serial.println("Here we are");
  }

void setup() {

  while (!Serial){};
  menu.load(mainMenu, mainMenuSize);
  // Display current menu (mainMenu)
  menu.show();
}

void loop() {

   menu.run(500);
   delay(1500);
}
