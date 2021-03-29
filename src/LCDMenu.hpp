#ifndef LDCMENU_h
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
    const char * message;
    // Keyboard character entry to select this menu entry, overloaded:
    // We set bit 0x20 to 0 for normal message, to 1 for a PROGMEM message
    const char key;

  public:
    // Constructor: init the array of menu entries
    LCDMenuEntry(const char * m, bool isprogMem, char k, void (*c)()) :
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

///////////////////////////////////////////////////////////////////////////////
// The menu is a singleton class in which you load an array of menu entries.
///////////////////////////////////////////////////////////////////////////////
class LCDMenu
{
  private:

    // If PROGMEM is used, copy using this SRAM buffer size.
    static constexpr uint8_t PROGMEM_BUF_SIZE = 8;

    // This class implements a singleton desgin pattern with one static instance
    static LCDMenu * singleton;

    // Points to the array of menu entries for the current menu
    static const LCDMenuEntry * menu;

    // number of entries in the current menu
    static uint8_t size;


    LCD_I2C lcd;
    static uint8_t cursor;
    uint8_t oldCursor;
    static uint8_t windowMin;
    static uint8_t windowMax;


    //Constructor: init with an empty menu, prepares LCD screen
    LCDMenu();


  public:

    // Get a pointer to the one singleton instance of this class
    static LCDMenu& get();

    // Get a pointer to the one singleton instance of this class and point it
    // to the current menu
    static const LCDMenu& get(const LCDMenuEntry* array, uint8_t arraySize);

    void lcdBegin();

    // Install the current menu to display
    inline void load(const LCDMenuEntry* array, uint8_t arraySize)
    {
      menu = array;
      size = arraySize;
      lcd.backlight();
    }

    // Display the current menu on the Serial console
    void show() const;

///////////////////////////////////////////////////////////////////////////////
    // Run the menu.
    // SerialMenu::run modified so Menu can be navigated by typing
    //W (up) and S (down)
    bool run(const uint16_t loopDelayMs);


};

 #endif
