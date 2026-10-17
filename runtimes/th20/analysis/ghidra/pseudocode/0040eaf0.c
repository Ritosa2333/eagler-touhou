/* Entry: 0x0040eaf0; symbol: FUN_0040eaf0; body bytes: 46 */

exception * __thiscall FUN_0040eaf0(void *this,uint param_1)

{
  std::exception::~exception((exception *)this);
  if ((param_1 & 1) != 0) {
    FUN_0054278d(this);
  }
  return (exception *)this;
}

