///////////////////////////////////////////////////////////////////////////////
// LCDMenu - by f3rcode (based on SerialMenu (Dan Truong's library)
///////////////////////////////////////////////////////////////////////////////

#include <LCDMenu.hpp>

LCDMenu* LCDMenu::singleton = nullptr;
uint8_t LCDMenu::portStatus =  uint8_t(0);


LCDMenu::LCDMenu(LCD_I2C& lcd) :
    lcd(lcd),
    oldCursor(0),
    cursor(0),
    windowMin(0),
    windowMax(LCD_MAX_ROWS-1),
    size(0),
    number(0),
    inNumberMenu(false)
    {

    //Serial.begin(9600);
    //while (!Serial);

    pcIntInit();
    pinMode(ENTER_BUTTON, INPUT);
    digitalWrite(ENTER_BUTTON, HIGH); //PULLUP //NOW EXTERNAL PULLUP!!!!!
    pinMode(UP_BUTTON, INPUT);
    digitalWrite(UP_BUTTON, HIGH); //PULLUP
    pinMode(DOWN_BUTTON, INPUT);
    digitalWrite(DOWN_BUTTON, HIGH); //PULLUP
}

LCDMenu::~LCDMenu()
{
  pcIntDestroy();
}

void LCDMenu::pcIntInit()
{
  PCMSK0 |= (1 << PCINTx(0))|(1 << PCINTx(1))|(1 << PCINTx(2));  //enables pin <--PREFERIBLY NOT HARDCODED
  PCIFR |= (1 << PCIF0); //clear any oustanding interrupt
  PCICR |= (1 << PCIE0); //enables interrupt for the group PCINT0..7 corresponding to portB (D8 to D13)
}

void LCDMenu::pcIntDestroy()
{
  PCMSK0 &= (0 << PCINTx(0))&(0 << PCINTx(1))&(0 << PCINTx(2));  //enables pin <--PREFERIBLY NOT HARDCODED
  //PCIFR &= (0 << PCIF0); //clear any oustanding interrupt
  PCICR &= (0 << PCIE0); //disables interrupt for the group PCINT0..7 corresponding to portB (D8 to D13)
}

//static
LCDMenu& LCDMenu::get(LCD_I2C& lcd)
  {
      if (!singleton)
        singleton = new LCDMenu(lcd);

      return *singleton;
    }

//static
/*const LCDMenu& LCDMenu::get(LCDMenuEntry* array, uint8_t arraySize)
  {
      (void) LCDMenu::get();
      singleton->load(array, arraySize);
      return *singleton;
  }
*/
//method to init lcd
//Wire::begin() hangs when LCD_I2C::begin() called from LCDMenu constructor
void LCDMenu::lcdBegin()
{
    lcd.begin();
    lcd.backlight();
}

void LCDMenu::show()
{
    //by now, just printing data from SRAM
    lcd.clear();
    /*if (!inNumberMenu)
    {//Legacy
      for (uint8_t i = windowMin; i <= windowMax; i++)
      {
         lcd.setCursor(0, i-windowMin);
         //Serial.println(menu[i].message);
         if (i==cursor) lcd.print(">");
         else lcd.print(" ");
         lcd.print(menu[i].message);
      }
    }
    else*/
    {
      //void print (const char* text1, const char* text2)
      lcd.setCursor(0, 0);
      //Serial.println(getNumberMenuLabel);
      lcd.print(getNumberMenuLabel);
      lcd.setCursor(0, 1);
      lcd.print(number-1);
      lcd.print("   [");
      lcd.print(number);
      lcd.print("]   ");
      lcd.print(number+1);
    }
}

void LCDMenu::getNumber(const char* message, const uint16_t startingValue, void (*callback)(int))
{
  toNumberMenu(message, startingValue, callback);

  while(singleton->inNumberMenu)
  {
    run(700);
  }
}


bool LCDMenu::run(const uint16_t loopDelayMs)
{
  /*if (!inNumberMenu)
  {//Legacy
    if (oldCursor!=cursor)
    {
      oldCursor=cursor;
      show();
      return true;
    }

    delay(loopDelayMs);
    return false;
 }
 else*/
 {
   if (cursor<oldCursor)
   {
      number--;
   }
   else if (cursor>oldCursor)
   {
     number++;
   }
   else
   {
     delay(loopDelayMs);
     return false;
   }
   cursor=oldCursor;
   delay(loopDelayMs);
   show();
   return true;
 }
}

//static
void LCDMenu::enterSelected()
{

  digitalWrite(5,LOW);//DEBUG

  /*if (!singleton->inNumberMenu)
  {//Legacy
    singleton->menu[singleton->cursor].actionCallback();
  }
  else*/
  {
    singleton->cursor = 0;
    singleton->oldCursor = 0;
    singleton->windowMin = 0;
    singleton->windowMax = LCD_MAX_ROWS-1;
    delay(400);
    singleton->callbackAux(singleton->number);
    singleton->inNumberMenu = !singleton->inNumberMenu;
  }
 }

//static
void LCDMenu::upSelected()
{

   digitalWrite(5,HIGH);//DEBUG

   /*if (!singleton->inNumberMenu)
   {//Legacy
     if (singleton->cursor == singleton->windowMax)
     {
       if (singleton->windowMax == singleton->size-1)
       {
         return;
       }
       else
       {
         singleton->windowMax++;
         singleton->windowMin++;
       }
     }
  }*/
  singleton->cursor++;
}

 //static
 void LCDMenu::downSelected()
{

 digitalWrite(5,HIGH);//DEBUG

 /*if (!singleton->inNumberMenu)
 {//Legacy
   if (singleton->cursor == singleton->windowMin)
   {
     if (!singleton->windowMin) //(=0)
     {
       return;
     }
     else
     {
       singleton->windowMin--;
       singleton->windowMax--;
     }
   }
 }*/
 singleton->cursor--;
}
//ISR_NOBLOCK insert a SEI() instruction right at the beginning
// in order to not defer any other interrupt more than absolutely needed.
// This way, nested interrupts are enabled giving buttons interrupts low priority
ISR(PCINT0_vect, ISR_NOBLOCK)
 {
    uint8_t newPortStatus=PINB;
    uint8_t triggerPins=PCMSK0 & (LCDMenu::portStatus ^ newPortStatus) & ~newPortStatus; //^ = xor detects a change and the final & detects rising edge


    LCDMenu::portStatus=newPortStatus;


   if (triggerPins & _BV(digitalPinToPCMSKbit(UP_BUTTON)))  {LCDMenu::upSelected();}

   else if (triggerPins & _BV(digitalPinToPCMSKbit(DOWN_BUTTON)))  {LCDMenu::downSelected();}

   else if (triggerPins & _BV(digitalPinToPCMSKbit(ENTER_BUTTON))) {LCDMenu::enterSelected();}

 }
