/* Entry: 0x004fbf20; symbol: FUN_004fbf20; body bytes: 78 */

locale * __thiscall FUN_004fbf20(void *this,locale *param_1)

{
                    /* WARNING: Load size is inaccurate */
  if (*this == 0) {
    std::locale::locale(param_1);
  }
  else {
                    /* WARNING: Load size is inaccurate */
    FUN_004f5110(param_1,*this);
  }
  return param_1;
}

