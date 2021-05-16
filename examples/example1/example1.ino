#include <LCDMenu.hpp>

void foo();

const LCDMenu& menu = LCDMenu::get();

///////////////////////////////////////////////////////////////////////////////
// Main menu
///////////////////////////////////////////////////////////////////////////////
const char mainMenuTempeh[]  = "1 - Tempeh";
const char mainMenuNatto[]  = "2 - Natto";
const char mainMenuKoji[]  = "3 - Koji";

// Define the main menu
const LCDMenuEntry mainMenu[] = {
  {mainMenuTempeh, false, '1', [](){//prints on lcd and Serial
                                    //and shows menu on lcd again after
                                    Serial.println("Option1");
                                    menu.print("Option2");
                                    foo();
                                  } },
  {mainMenuNatto, false, '2', [](){//prints on lcd and serial
                                  //and get the message on lcd screen
                                  //UNTIL PRESSING A BUTTON WHICH
                                  //CHANGES INTERNAL CURSOR VALUE
                                  Serial.println("Option2");
                                  menu.print("Option2");
                                  foo();
                                  } },
  {mainMenuKoji, false, '3', [](){Serial.println("Still not available.");
                                  menu.print("Still not available.");
                                  } },

};
constexpr uint8_t mainMenuSize = GET_MENU_SIZE(mainMenu);

void foo(){
  Serial.println("Here we are");
  }

void setup() {
  Serial.begin(9600);
  while (!Serial){}; //<---Serial init on LCDMenu constructor

  //LED on 5th. Debugging purpose
  pinMode(5, INPUT);
  digitalWrite(5, HIGH); //PULLUP

  menu.lcdBegin();
  menu.load(mainMenu, mainMenuSize);
  // Display current menu (mainMenu)
  menu.show();
}

void loop() {

   menu.run(500);
   delay(500);
}
