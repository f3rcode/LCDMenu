///////////////////////////////////////////////////////////////////////////////
// LCDMenu - by f3rcode (based on SerialMenu (Dan Truong's library)
///////////////////////////////////////////////////////////////////////////////

#include <LCDMenu.hpp>

LCDMenu* LCDMenu::singleton = nullptr;
LCDMenuEntry* LCDMenu::menu = nullptr;
uint8_t LCDMenu::size = uint8_t(0);
int8_t LCDMenu::cursor = int8_t(0);
uint8_t LCDMenu::windowMin = uint8_t(0);
uint8_t LCDMenu::windowMax = LCD_MAX_ROWS-1;
uint8_t LCDMenu::portStatus =  uint8_t(0);
uint16_t LCDMenu::number;
boolean LCDMenu::inNumberMenu=false;

char LCDMenu::getNumberMenuIntro[15];
char LCDMenu::getNumberMenuValue[15];




LCDMenu::LCDMenu() :
    lcd(0x27),
    oldCursor(0)
    {

    Serial.begin(9600);
    while (!Serial);

    pcIntInit();
    pinMode(ENTER_BUTTON, INPUT);
    digitalWrite(ENTER_BUTTON, HIGH); //PULLUP //NOW EXTERNAL PULLUP!!!!!
    pinMode(UP_BUTTON, INPUT);
    digitalWrite(UP_BUTTON, HIGH); //PULLUP
    pinMode(DOWN_BUTTON, INPUT);
    digitalWrite(DOWN_BUTTON, HIGH); //PULLUP
}

void LCDMenu::pcIntInit()
   {
   PCMSK0 |= (1 << PCINTx(0))|(1 << PCINTx(1))|(1 << PCINTx(2));  //enables pin <--PREFERIBLY NOT HARDCODED
   PCIFR |= (1 << PCIF0); //clear any oustanding interrupt
   PCICR |= (1 << PCIE0); //enables interrupt for the group PCINT0..7 corresponding to portB (D8 to D13)
   }

//static
LCDMenu &LCDMenu::get()
  {
      if (!singleton)
        singleton = new LCDMenu;

      return *singleton;
    }

//static
const LCDMenu& LCDMenu::get(LCDMenuEntry* array, uint8_t arraySize)
  {
      (void) LCDMenu::get();
      singleton->load(array, arraySize);
      return *singleton;
  }

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
    if (!inNumberMenu)
    {
      for (uint8_t i = windowMin; i <= windowMax; i++)
      {
         lcd.setCursor(0, i-windowMin);
         Serial.println(menu[i].getMenu());
         if (i==cursor) lcd.print(">");
         else lcd.print(" ");
         lcd.print(menu[i].getMenu());
      }
    }
    else
    {
      //void print (const char* text1, const char* text2)
      lcd.setCursor(0, 0);
      Serial.println(menu[0].getMenu());
      lcd.print(menu[0].getMenu());
      lcd.setCursor(0, 1);
      reckonNumberMenu();
      lcd.print(menu[1].getMenu());
    }

}

void LCDMenu::reckonNumberMenu()
{
  sprintf(getNumberMenuValue,"%d   [%d]   %d",number-1,number,number+1);
}

void LCDMenu::print(const char* text, const uint8_t delayMs)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(text);
  if (delayMs != 0 )
  {
    delay(delayMs);
    show();
  }
}

void LCDMenu::print (const char* text1, const char* text2)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  Serial.print(text1);
  Serial.print(":");
  Serial.println(text2);
  lcd.print(text1);
  lcd.setCursor(0, 1);
  lcd.print(text2);
}

void LCDMenu::print(int integer, const uint8_t delayMs)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(integer);
  if (delayMs != 0 )
  {
    delay(delayMs);
    show();
  }
}

void LCDMenu::print (float number)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(number);
}

void LCDMenu::print (float number1,float number2)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(number1);
  lcd.setCursor(0, 1);
  lcd.print(number2);
}

void LCDMenu::print (float number1,float number2,float number3)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(number1);
  lcd.setCursor(0, 1);
  lcd.print(number2);
  lcd.setCursor(12, 1);
  lcd.print(number3);
}

void LCDMenu::print (float number1,float number2,float number3,const char* text)
{
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print(number1);
  lcd.setCursor(11, 0);
  lcd.print(text);
  lcd.setCursor(0, 1);
  lcd.print(number2);
  lcd.setCursor(12, 1);
  lcd.print(number3);
}

////GET RID OF MAGIC NUMBERS IN DEFAULT ARGS!!!
/*
template <class T>
T LCDMenu::getNumber(const char* message, const T startingValue){

  T number=(T)0;

  this->load(getNumberMenu,numberMenuSize);
  this->show();

  return number;
  //
  //while()
  //  run(loopDelayMs);

}
*/
/*
* SerialMenu::run function mod
*/
bool LCDMenu::run(const uint16_t loopDelayMs)
{
  if (!inNumberMenu)
  {
    if (oldCursor!=cursor)
    {
      oldCursor=cursor;
      show();
      return true;
    }

    delay(loopDelayMs);

    return false;
 }
 else
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

  if (!inNumberMenu)
  {
    menu[LCDMenu::cursor].actionCallback();
  }
  else
  {
    menu[1].actionCallback();
  }

  return;
 }

//static
void LCDMenu::upSelected()
{

   digitalWrite(5,HIGH);//DEBUG

   if (!inNumberMenu)
   {
     if (cursor == windowMax)
     {
       if (windowMax == size-1)
       {
         return;
       }
       else
       {
         cursor = windowMax;
         windowMax++;
         windowMin++;
       }
     }
     else
     {
       cursor++;
     }
  }
  else
  {
    cursor++;
  }
  return;
}

 //static
 void LCDMenu::downSelected()
{

 digitalWrite(5,HIGH);//DEBUG

 if (!inNumberMenu)
 {
   if (cursor == windowMin)
   {
     if (!windowMin) //(=0)
     {
       return;
     }
     else
     {
       cursor = windowMin;
       windowMin--;
       windowMax--;
     }
   }
   else
   {
     cursor--;
   }
 }
 else
 {
   cursor--;
 }
 return;
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
