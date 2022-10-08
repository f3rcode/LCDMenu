#ifndef LCDMENU_h
#define LCDMENU_h

///////////////////////////////////////////////////////////////////////////////
// LCDMenu (by f3rcode) -
// Menu Library for Arduino LCD based on SerialMenu (Dan Truong's library)
// [Lots of comments remain the same from SerialMenu library]
///////////////////////////////////////////////////////////////////////////////
// Notes (SerialMenu library's):
/////////
// Be carefuly with memory constrained boards, running out of SRAM will lead
// to flaky code behavior. Keep menu text minimal as it could eat up memory.
// To alleviate memory pressure menus can be stored in Flash program memory
// using the PROGMEM keyword, instead of SRAM data memory. However, even Flash
// memory is limited.
//
// Menu callback functions can be declared as separate functions or as lambda
// functions directly in the data structure with the menu data. Lambda notation
// is best for simple tasks like setting a global variable or calling another
// menu, as it keeps the code for a menu entry concise and within the menu
// entry definition. See the example.
// A lambda function syntax is written "[](){}" where the code goes inside {}.
// The other elements "[]()" are not used here.
//

#include <avr/pgmspace.h>
#include <HardwareSerial.h>
#include <LCD_I2C.h>

//////////////f3rcode//////////////
// Lcd display has 2 rows
//////////////////////////////////
#define LCD_MAX_ROWS 2
//buttons
#define ENTER_BUTTON 8 //PB0 = D8
#define UP_BUTTON 9 //PB1 = D9
#define DOWN_BUTTON 10 //PB2 = D10
//int
#define PCINTx(a) PCINT ## a //a MUST be in a range [0,7] corresponding to PB0..7 i.e. D8..13

///////////////////////////////////////////////////////////////////////////////
// Macro to get the number of menu entries in a menu array.
///////////////////////////////////////////////////////////////////////////////
#define GET_MENU_SIZE(menu) sizeof(menu)/sizeof(LCDMenuEntry)

///////////////////////////////////////////////////////////////////////////////
// Define a menu entry as:
// - a menu message to display
// - a boolean to specify if the message is in SRAM or PROGMEM Flash memory
// - a menu key to select
// - a callback function to perform the menu action
///////////////////////////////////////////////////////////////////////////////

struct LCDMenuEntry
{
  char * message;
  void (*actionCallback)();
};

///////////////////////////////////////////////////////////////////////////////
// The menu is a singleton class in which you load an array of menu entries.
///////////////////////////////////////////////////////////////////////////////
class LCDMenu
{
  private:

    // This class implements a singleton design pattern with one static instance
    static LCDMenu * singleton;

    // Points to the array of menu entries for the current menu
    LCDMenuEntry * menu;

    // number of entries in the current menu
    uint8_t size;

    char *getNumberMenuLabel;
    boolean inNumberMenu;
    void (*callbackAux)(int);


    LCD_I2C lcd;
    int8_t cursor;
    int8_t oldCursor;
    uint8_t windowMin;
    uint8_t windowMax;

    //Constructor: init with an empty menu, prepares LCD screen
    LCDMenu();

    void pcIntInit();

  public:

    static uint8_t portStatus;

    //getNumberMenu attributes
    uint16_t number;

    // Get a pointer to the one singleton instance of this class
    static LCDMenu& get();

    // Get a pointer to the one singleton instance of this class and point it
    // to the current menu
    static const LCDMenu& get(LCDMenuEntry* array, uint8_t arraySize);

    void lcdBegin();

    // Install the current menu to display
    inline void load(LCDMenuEntry* array, uint8_t arraySize)
    {
      //reset screen values
      cursor=0;
      oldCursor=0;

      menu = array;
      size = arraySize;
      Serial.print("LoadingMenu ");
      Serial.println(menu[0].message);
    //  lcd.backlight();
    }

    // Display the current menu on the Serial console
    void show();

    // Uses I2C_LC2 print, so there is no need of instanciating
    // a new object if we just want to show a result on screen, for instance.
    void print(const char* text, const uint8_t delayMs = 0); // prints text and shows menu after delayMs if set
    void print (const char* text1, const char* text2);
    void print(int integer, const uint8_t delayMs = 0);  // prints text and shows menu after delayMs if set
    void print (float number);
    void print (float number1,float number2);
    void print (float number1,float number2,float number3);
    void print (float number1,float number2,float number3, const char* text);


    // return a number input read form the LCD "console".
    // Note: this routine is showing a special menu
    // for a number to be typed through buttons use
    void getNumber(const char* message, const uint16_t startingValue, void (*callback)(int)){
      number=startingValue;
      Serial.println(number);

      getNumberMenuLabel = message;
      callbackAux = callback;
      inNumberMenu=true;

      show();
    }
///////////////////////////////////////////////////////////////////////////////
    // Run the menu.
    // LCDMenu::run modified so Menu can be navigated by typing
    //W (up) and S (down)
    bool run(const uint16_t loopDelayMs);

    static void enterSelected();
    static void upSelected();
    static void downSelected();


};

 #endif
