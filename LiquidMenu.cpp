/*
  LiquidMenu.cpp - Library for build menu on LiquidCrystal.
  Created by Vasiliy A. Ponomarjov, August 30, 2013
  Modified by Vasiliy A. Ponomarjov, August 5, 2016
  Email: vas@vingrad.ru
*/

//#include "WProgram.h"
#include "LiquidMenu.h"

//конструктор
LiquidMenu::LiquidMenu(uint8_t aCol, uint8_t aRow) {
  //задаем размерность экрана
  _lcdCol = aCol;
  _lcdRow = aRow;
  //инициализаируем свойства
  LiquidMenu::setCursor('>');
}

//запуск на построение
void LiquidMenu::start() {
  //выбираем всех 0 уровня без родителей
  LiquidMenu::setCountLevelItem(0);
  //выбрана 1 строка
  LiquidMenu::setSelectItemIndex(0);
  //курсор на 1 строку
  LiquidMenu::setSelectLine(0);
  //выбираем 1 элемент
  LiquidMenu::setSelectItem(_mList[0]);
  //еще не редактируем параметр
  LiquidMenu::edit(false);
}

//основная функция вывода меню
void LiquidMenu::show(int aBtn, void (*aFunc)(uint8_t, char*)) {

  LiquidMenu::setClickBtn(aBtn);

  _fpaint = aFunc;

  //если не параметр, то обрабатываем нажатия клавиш, ну и рисуем
  if (!LiquidMenu::isEdit()) {
    switch (_clickBtn) {
      //btnRight
      case 0: {
        LiquidMenu::right();
        break;
      }
      //btnUp
      case 1: {
        LiquidMenu::up();
        break;
      }
      //btnDown
      case 2: {
        LiquidMenu::down();
        break;
      }
      //btnLeft
      case 3: {
        LiquidMenu::left();
        break;
      }
    }
  } else {
    //а вдруг параметр без процедуры и мы в него естественно вошли, надо как-то выйти
    if ((_clickBtn == 3) && (LiquidMenu::getSelectItem()->_pfunc == NULL))
      LiquidMenu::left();
  }
  //вызвали прорисовку
  LiquidMenu::paint();
}

void LiquidMenu::addMenuItem(uint8_t aId, uint8_t aParent, char *aTitle, boolean aParam, void(*f)(int)) {
  uint8_t index;

  index = LiquidMenu::getCountItem();

  _listMenu[index].id = aId;
  _listMenu[index].parent = aParent;
  _listMenu[index].title = aTitle;
  _listMenu[index].param = aParam;
  _listMenu[index]._pfunc = f;

  //считаем индексы, всего пунктов меню и пунктов меню в 0 уровне
  LiquidMenu::setCountItem(index+1);
}

void LiquidMenu::paint() {
  register uint8_t i,j,c;

  //если редактируем параметр, то вызываем подвязанную к нему функцию она сама прорисует что надо
  if (LiquidMenu::isEdit() && (LiquidMenu::getSelectItem()->_pfunc != NULL)) {
    LiquidMenu::getSelectItem()->_pfunc(_clickBtn);
    return;
  }

  //если выбран последний пункт меню, то курсор надо поместить в самый низ экрана
  if (_selectItemIndex == (_countLevelItem - 1)) {
      LiquidMenu::setSelectLine(_lcdCol-1);
  }

  //устанавливаем счетчик на первую строку экрана
  j=0;
  //определяем количество отображаемых строк, либо размер экрана, либо число пунктов в текущем уровне
  c = min(_lcdRow, _countLevelItem); 
  //запускаем цикл от текущего меню - положение курсора до + число строк дисплея
  for (i = _selectItemIndex - _selectLine; i < _selectItemIndex - _selectLine + c; i++) {
    //рисуем строку меню
    LiquidMenu::buildLine(_mList[i]->title);
    //рисуем курсор
    if (j == _selectLine) {
      _lineScreen[0] = LiquidMenu::getCursor();
      LiquidMenu::setSelectItem(_mList[i]);
    }
    //вызвали пользовательскую функцию
    (*_fpaint)(j, _lineScreen);
    //переходим на следующую строку экрана
    j++;
  }
}

/*
 *  Создание строки для вывода на экран
 */
void LiquidMenu::buildLine(char *str) {
  //обнуляем буфер вывода
  memset(_lineScreen, ' ', sizeof(_lineScreen)/sizeof(_lineScreen[0]));
  //отступ
  _lineScreen[_tabItem] = '\0';
  //DONE: -o Vas -u LiquidMenu: Сделать конкатенацию строк с учетом разрешения дисплея (защититься от переполнения буфера)
  //копируем число символов (длина массива - отступ - завершающий ноль)
  strncat(_lineScreen, str, sizeof(_lineScreen)/sizeof(_lineScreen[0])-_tabItem-1);
  //завершаем нулем
  _lineScreen[_lcdCol] = '\0';
}

/*
 * Поиск по id по всему меню
 */
menuItem* LiquidMenu::find(uint8_t aId) {
  register uint8_t i;
  for (i = 0; i < sizeof(_listMenu)/sizeof(_listMenu[0]); i++) {
    if (_listMenu[i].id == aId)
      return &_listMenu[i];
  }
  return NULL;
}

/*
 * Переход на уровень ниже
 * TODO -o Vas Отказаться от хранения уровня в структуре (освободим чуть памяти)
 */
void LiquidMenu::right() {
  if (LiquidMenu::hasChild(LiquidMenu::getSelectItem()->id)) {
    LiquidMenu::setCountLevelItem(LiquidMenu::getSelectItem()->id);
    LiquidMenu::setSelectItemIndex(0);
    LiquidMenu::setSelectLine(0);
  }
  //если это параметр и флаг входа в режим редактирования не установлен
  if ((LiquidMenu::getSelectItem()->param) && (!LiquidMenu::isEdit()))  {
    LiquidMenu::edit(true);
  }
}

/*
 * Переход на уровень выше
 */
void LiquidMenu::left() {
  //а вдруг нулевой уровень или параметр
  if ((LiquidMenu::getSelectItem()->parent > 0) && (!LiquidMenu::isEdit())) {
    //ищем родителя и берем его родителя и завполняем новый массив
    LiquidMenu::setCountLevelItem(find(_mList[LiquidMenu::getSelectItemIndex()]->parent)->parent);
    LiquidMenu::setSelectItemIndex(LiquidMenu::findIdInSelArray(LiquidMenu::getSelectItem()->parent));
    //если выбранный пункт последний в списке, то необходимо курсор поставить на 2 строку
    if (LiquidMenu::getSelectItemIndex() == LiquidMenu::getCountLevelItem()-1) {
      /*TODO -o Vas Последняя строка, должна быть от количества строк экрана, а не 1*/
      //LiquidMenu::setSelectLine(1);
      LiquidMenu::setSelectLine(_lcdRow-1);
    }
    else {
      /*TODO -o Vas Если строк больше 2-х то надо найти правильное положение курсора*/
      LiquidMenu::setSelectLine(0);
    }
  }
  LiquidMenu::exit();
}

/*
 * Движение вверх
 */
void LiquidMenu::up() {
  LiquidMenu::setSelectItemIndex(_selectItemIndex-1);
  LiquidMenu::setSelectLine(_selectLine-1);
}

/*
 * Движение вниз
 */
void LiquidMenu::down() {
  LiquidMenu::setSelectItemIndex(_selectItemIndex+1);
  LiquidMenu::setSelectLine(_selectLine+1);
}

/*
 * Выход из редактирования
 */
void LiquidMenu::exit() {
  //выходим, значит либо отредктировали, либо просто на уровень выше, но явно вываливаемся не в параметр
  LiquidMenu::edit(false);
  //вызываем прорисовку
  LiquidMenu::paint();
}

/*
 * Получение и установка выбранного пункта
 */
int LiquidMenu::getSelectItemIndex() {
  return _selectItemIndex;
}

void LiquidMenu::setSelectItemIndex(int aItem){
  _selectItemIndex = constrain(aItem, 0, _countLevelItem - 1);
}


/*
 * Получение и установка текущего положения указателя меню
 */
int LiquidMenu::getSelectLine() {
  return _selectLine;
}

void LiquidMenu::setSelectLine(int aLine) {
  //между нулевой строкой и установленным кол-вом строк дисплея
  _selectLine = constrain(aLine, 0, _lcdRow-1);
  //а вдруг максимальное число пунктов меню меньше, чем строк экрана
  _selectLine = constrain(_selectLine, 0, _countLevelItem - 1);
}

/*
 * Получение и установка текущего положения указателя меню
 */
void LiquidMenu::setClickBtn(int aBtn) {
  _clickBtn = aBtn;
}

/*
 * Получение и установка курсора
 */
char LiquidMenu::getCursor() {
  return _cursor;
}

void LiquidMenu::setCursor(char aCursor) {
  _cursor = aCursor;
  //отступ без курсора, последний симво \0 для конкатенации
  //_tabItem[0] = ' '; _tabItem[1] = ' '; _tabItem[2] = '\0';
  _tabItem = strlen(&_cursor)+1;
}

/*
 * Получение и установка числа элементов меню
 */
uint8_t LiquidMenu::getCountItem() {
  return _countItem;
}

void LiquidMenu::setCountItem(uint8_t aCount) {
  _countItem = aCount;
}

/*
 * Получение и установка числа элементов меню определенного уровня и родителя
 */
uint8_t LiquidMenu::getCountLevelItem() {
  return _countLevelItem;
}

void LiquidMenu::setCountLevelItem(uint8_t aParent) {
  register uint8_t i, j;
   //обнуляем счетчик и массивы
  _countLevelItem = 0;
  memset(_mList, 0, sizeof(_mList)/sizeof(_mList[0]));
  j = 0;
  for (i = 0; i < LiquidMenu::getCountItem(); i++) {
    if ((_listMenu[i].parent == aParent)) {
      _countLevelItem++;
      _mList[j] = &_listMenu[i];
      j++;
    }
  }
}

/*
 * Получение и установка текущего ID элемента меню
 */
menuItem* LiquidMenu::getSelectItem() {
  return _selectItem;
}

void LiquidMenu::setSelectItem(menuItem *aItem) {
  _selectItem = aItem;
}

/*
 * Поиск дочек
 */
boolean LiquidMenu::hasChild(uint8_t aId) {
  register uint8_t i;
  for (i = 0; i < LiquidMenu::getCountItem(); i++) {
    if (_listMenu[i].parent == aId)
      return true;
  }
  return false;
}

/*
 * Поиск по id
 */
int LiquidMenu::findIdInSelArray(uint8_t aId) {
  register uint8_t i;
  for (i = 0; i < sizeof(_mList)/sizeof(_mList[0]); i++) {
    if (_mList[i]->id == aId)
      return i;
  }
  return 0;
}

/*
 * Работаем с параметром (сеттеры и геттеры)
 */
boolean LiquidMenu::isEdit() {
  return _state;
}

void LiquidMenu::edit(uint8_t aState) {
  //входим в режим редактирования параметра, значит не надо передавать кнопку
  LiquidMenu::setClickBtn(-1);
  _state = aState;
}