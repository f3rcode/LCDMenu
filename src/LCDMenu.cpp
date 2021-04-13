///////////////////////////////////////////////////////////////////////////////
// LCDMenu - by f3rcode (based on SerialMenu (Dan Truong's library)
///////////////////////////////////////////////////////////////////////////////

#include <LCDMenu.hpp>

LCDMenu* LCDMenu::singleton = nullptr;
const LCDMenuEntry* LCDMenu::menu = nullptr;
uint8_t LCDMenu::size = uint8_t(0);
uint8_t LCDMenu::cursor = uint8_t(0);
uint8_t LCDMenu::windowMin = uint8_t(0);
uint8_t LCDMenu::windowMax = LCD_MAX_ROWS-1;
uint8_t LCDMenu::portStatus =  uint8_t(0);

LCDMenu::LCDMenu() :
    lcd(0x27)
    {

    Serial.begin(9600);
    while (!Serial);

    pcIntInit();
    pinMode(ENTER_BUTTON, INPUT);
    digitalWrite(ENTER_BUTTON, HIGH); //PULLUP
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

static LCDMenu &LCDMenu::get()
  {
      if (!singleton)
        singleton = new LCDMenu;

      return *singleton;
    }

static const LCDMenu& LCDMenu::get(const LCDMenuEntry* array, uint8_t arraySize)
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

void LCDMenu::show() const
{
    //by now, just printing data from SRAM
    for (uint8_t i = windowMin; i <= windowMax; i++)
    {
       lcd.setCursor(0, i-windowMin);
       Serial.println(menu[i].getMenu());
       if (i==cursor) lcd.print(">");
       else lcd.print(" ");
       lcd.print(menu[i].getMenu());
    }

}

/*
* SerialMenu::run function mod
*/
bool LCDMenu::run(const uint16_t loopDelayMs)
  {

      const bool userInputAvailable = Serial.available();

      // Code block to display a heartbeat as a dot on the Serial console and
      // also by blinking the status LED on the board.
      #if LCDMenu_SHOW_HEARTBEAT_ON_IDLE == true
      {
        const uint16_t callsPerSecond = 1000 / loopDelayMs;
        const uint16_t loopsPerTick = 1000 * callsPerSecond; //f3rcode (test)
        const uint16_t loopsPerBlink = callsPerSecond; // blink every second

        // Waiting for input
        if (!userInputAvailable)
        {
          ++waiting;
          // After waiting for 10s, heartbeat blink the LED every second.
          if (waiting >= loopsPerTick && waiting % loopsPerBlink == 0)
          {
            digitalWrite(LED_BUILTIN, ((waiting / loopsPerBlink) & 0x01) ? HIGH : LOW);
         }
          // Print heartbeat every 10s on console.
          if (waiting % loopsPerTick == 0)
          {
            Serial.print(".");
          }
        }
        else
        {
          // New input: Clear to a new line if we printed ticks.
          if (waiting >= loopsPerTick)
          {
            Serial.println("");
            waiting = 0;

          }
        }
      }
      #endif

      // Process the input
      if (!userInputAvailable)
      {
        return false;
      }
      else
      {
        // Read one character from the Serial console as a menu choice.
        char menuChoice = Serial.read();

        // Carriage return is not a menu choice
        if (menuChoice == 0x0A)
        {
          return false;
        }
        if (menuChoice == 0x53) //S = DOWN
        {
          if (cursor == windowMin){
            if (!windowMin) //(=0)
              return false;
            else{
              cursor=--windowMin;
              windowMax--;
            }
          }else
            cursor--;

          show();
          return true;

        }
        if (menuChoice == 0x57) //W = UP
        {
           if (cursor == windowMax){
            if (windowMax== size-1)
              return false;
            else{
              windowMin++;
              cursor=++windowMax;
            }
          }else
            cursor++;

          show();
          return true;

        }

        //SerialMenu's block
        uint8_t i;
        for (i = 0; i < size; ++i)
        {
          if (menu[i].isChosen(menuChoice))
          {
            menu[i].actionCallback();
            break;
          }
        }
        if (i == size)
        {
          Serial.print(menuChoice);
          Serial.println(": Invalid menu choice.");
        }
        return true;


    }
  }

static void LCDMenu::enterSelected()
{

  digitalWrite(5,LOW);//DEBUG

  menu[LCDMenu::cursor].actionCallback();

  return;
 }

static void LCDMenu::upSelected()
{

 digitalWrite(5,HIGH);//DEBUG

 if (cursor == windowMax){
   if (windowMax== size-1)
     return false;
   else{
     windowMin++;
     cursor=++windowMax;
   }
 }else
   cursor++;

 return;
}

 static void LCDMenu::downSelected()
{

 digitalWrite(5,HIGH);//DEBUG

 if (cursor == windowMin){
   if (!windowMin) //(=0)
     return false;
   else{
     cursor=--windowMin;
     windowMax--;
   }
 }else
   cursor--;

 return;
}
//ISR_NOBLOCK insert an SEI() instruction right at the beginning
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
