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
class LCDMenuEntry {
  public:
    // Callback function that performs this menu's action
    void (*actionCallback)();

  private:
    // Message to display via getMenu()
    // The pointer can be in SRAM or in FLASH (requires PROGMEM to access)
    char * message;
    // Keyboard character entry to select this menu entry, overloaded:
    // We set bit 0x20 to 0 for normal message, to 1 for a PROGMEM message
    const char key;

  public:
    // Constructor: init the array of menu entries
    LCDMenuEntry(char * const  m, bool isprogMem, char k, void (*c)()) :
      message(m),
      //#if SerialMenu_DISABLE_PROGMEM_SUPPORT != true
      key(((isprogMem) ? (k|0x20) : (k&(~0x20)))),
      //#else
      //  key(k),
      //#endif
      actionCallback(c)
    {}

    // Get the menu message to display
    inline const char * getMenu() const
    {
      return message;
    }

    inline char* setMenu(char* menuEntry)
    {
      message=menuEntry;
    }

    inline bool isProgMem() const
    {
      return key & 0x20;
    }

    // Check if the user input k matches this menu entry
    // Characters are converted to lowercase ASCII.
    // @note this impacts also symbols, not numbers, so test before using those
    inline bool isChosen(const char k) const
    {
      return (k|0x20) == (key|0x20);
    }
};


//A GETNUMBER MENU
extern char getNumberMenuIntro[];
extern char getNumberMenuValue[];
extern LCDMenuEntry getNumberMenu[];
extern uint8_t numberMenuSize;

///////////////////////////////////////////////////////////////////////////////
// The menu is a singleton class in which you load an array of menu entries.
///////////////////////////////////////////////////////////////////////////////
class LCDMenu
{
  private:

    // If PROGMEM is used, copy using this SRAM buffer size.
    static constexpr uint8_t PROGMEM_BUF_SIZE = 8;

    // This class implements a singleton design pattern with one static instance
    static LCDMenu * singleton;

    // Points to the array of menu entries for the current menu
    static LCDMenuEntry * menu;

    // number of entries in the current menu
    static uint8_t size;

    LCDMenuEntry * formerMenu;
    uint8_t formerMenuSize;

    static char getNumberMenuIntro[];
    static char getNumberMenuValue[];
    LCDMenuEntry * getNumberMenu;
    uint8_t numberMenuSize;
    static boolean inNumberMenu;

    LCD_I2C lcd;
    static int8_t cursor;
    int8_t oldCursor;
    static uint8_t windowMin;
    static uint8_t windowMax;

    //Constructor: init with an empty menu, prepares LCD screen
    LCDMenu();

    void pcIntInit();

  public:

    static uint8_t portStatus;

    //getNumber value
    static uint16_t number;

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

      windowMin=0;
      windowMax=LCD_MAX_ROWS-1;

      menu = array;
      size = arraySize;
      Serial.print("LoadingMenu ");
      Serial.println(menu[0].getMenu());
    //  lcd.backlight();
    }

    // Display the current menu on the Serial console
    void show();

    void reckonNumberMenu();

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
    template <typename T>
    T getNumber(const char* message, const T startingValue){

      formerMenu=menu;
      formerMenuSize=size;

      LCDMenuEntry getNumberMenu[] = {
        {getNumberMenuIntro, false, '1', [](){} },
        {getNumberMenuValue, false, '2', [](){
          //DEBUGGING PURPOSES
          Serial.println("Lambda world");
          singleton->inNumberMenu = !singleton->inNumberMenu;
        }}
      };

      uint8_t numberMenuSize = GET_MENU_SIZE(getNumberMenu);

      number=startingValue;
      //Serial.println(number);

      sprintf(getNumberMenuIntro, "%s", message);
      reckonNumberMenu();
      sprintf(getNumberMenuValue, "%d", startingValue);

      inNumberMenu=true;

      load(getNumberMenu,numberMenuSize);

      show();

      while (inNumberMenu)
      {
        run(500);
      }

      //Display former menu
      load(singleton->formerMenu, singleton->formerMenuSize);
      show();
      Serial.print("--->");
      Serial.println((T)number);
      return (T)number;
    }
///////////////////////////////////////////////////////////////////////////////
    // Run the menu.
    // SerialMenu::run modified so Menu can be navigated by typing
    //W (up) and S (down)
    bool run(const uint16_t loopDelayMs);

    static void enterSelected();
    static void upSelected();
    static void downSelected();


};

 #endif
