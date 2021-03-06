#include <LCDMenu.hpp>

void foo(uint8_t number);

LCDMenu& menu = LCDMenu::get();

///////////////////////////////////////////////////////////////////////////////
// Main menu
///////////////////////////////////////////////////////////////////////////////
char mainMenuTempeh[]  = "1 - Tempeh";
char mainMenuNatto[]  = "2 - Natto";
char mainMenuKoji[]  = "3 - Koji";
char mainMenuNumber[]  = "4 - Number Menu";

// Define the main menu
LCDMenuEntry mainMenu[] = {
  {mainMenuTempeh, false, '1', [](){//prints on lcd and Serial
                                    //and shows menu on lcd again after
                                    //UNTIL PRESSING A BUTTON WHICH
                                    //CHANGES INTERNAL CURSOR VALUE
                                    Serial.println("Option1");
                                    menu.print("Option2");
                                    foo(0);
                                  } },
  {mainMenuNatto, false, '2', [](){//prints on lcd and serial
                                  //and get the message on lcd screen for n msec.
                                  Serial.println("Option2");
                                  menu.print("Option2",(uint8_t) 500);
                                  foo(1);
                                  } },
  {mainMenuKoji, false, '3', [](){Serial.println("Still not available.");
                                  menu.print("Still not available.");
                                  } },
  {mainMenuNumber, false, '3', [](){//menu.print("Starring NumberMenu", (uint8_t) 1000);
                                    foo((uint8_t)menu.getNumber<uint8_t>("something", (uint8_t) 5));

                                  } },
};
constexpr uint8_t mainMenuSize = GET_MENU_SIZE(mainMenu);

void foo(uint8_t number){
  Serial.print("Here we are: ");
  Serial.println(number);
  //int
  menu.print(number,(uint8_t) 500);
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
}
