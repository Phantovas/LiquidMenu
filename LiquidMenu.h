/*
  LiquidMenu.h - Library for build menu on LiquidCrystal.
  Created by Vasiliy A. Ponomarjov, August 30, 2013.
  Email: vas@vingrad.ru
*/
#ifndef LiquidMenu_h
#define LiquidMenu_h

//#include "WProgram.h"

#include "Arduino.h"

struct menuItem {
  uint8_t id;
  uint8_t parent;
  char *title;
  uint8_t param;
  void (*_pfunc)(int);
};

class LiquidMenu {
  public:
    LiquidMenu(uint8_t aCol, uint8_t aRow);
    void start();
    void show(int aBtn, void (*aFunc)(uint8_t, char*));
    void addMenuItem(uint8_t aId, uint8_t aParent, char *aTitle, boolean aParam, void (*f)(int));
    menuItem* find(uint8_t aId);
    void exit();
    //setters & getters
    int getSelectItemIndex();
    int getSelectLine();
    menuItem* getSelectItem();
    char getCursor();
    void setCursor(char aCursor);
    uint8_t getCountItem();
    uint8_t getCountLevelItem();
    boolean isEdit();
  private:
    int _lcdCol;
    int _lcdRow;
    char _cursor;
    char _lineScreen[33];     //максимальная длина знаков в строке экрана 32+\0
    int _selectItemIndex;
    int _selectLine;
    boolean _state;
    menuItem* _selectItem;
    menuItem _listMenu[50];   //максималное число пунктов меню 50
    uint8_t _countItem;          //число добавленных пунктов меню
    menuItem* _mList[10];     //текущий список пунктов меню
    uint8_t _countLevelItem;     //число пунктов меню в текущем уровне
    int _clickBtn;
    uint8_t _tabItem;
    void (*_fpaint)(uint8_t, char*); //функция вывода строки на экран
    //methods
    void paint();
    void right();
    void left();
    void up();
    void down();
    void buildLine(char*);
    boolean hasChild(uint8_t aID);
    int findIdInSelArray(uint8_t aId);
    //setters & getters
    void setSelectItemIndex(int aItem);
    void setSelectLine(int aLine);
    void setClickBtn(int aBtn);
    void setCountItem(uint8_t aCount);
    void setCountLevelItem(uint8_t aParent);
    void setSelectItem(menuItem *aiItem);
    void edit(uint8_t aState);
};

#endif