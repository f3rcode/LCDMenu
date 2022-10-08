#ifndef LCDCMENU_t
#define LCDMENU_t
#include <LCDMenu.hpp>

//A GETNUMBER MENU
char getNumberMenuIntro[]  = "intro smthing";
char getNumberMenuValue[]  = "value";
LCDMenuEntry getNumberMenu[] = {
  {getNumberMenuIntro, false, '1', [](){} },
  {getNumberMenuValue, false, '2', [](){} }, //IT NEEDS A CALLBACK FUNCTION!
};
uint8_t numberMenuSize = GET_MENU_SIZE(getNumberMenu);


template <typename T>
T getNumber(const char* message, const T startingValue){

  T number=(T)0;

  this->load(getNumberMenu,numberMenuSize);
  this->show();

  return number;
  //
  //while()
  //  run(loopDelayMs);

}

extern template <> int getNumber(const char* message, const int startingValue);
extern template <> double getNumber(const char* message, const double startingValue);


#endif
