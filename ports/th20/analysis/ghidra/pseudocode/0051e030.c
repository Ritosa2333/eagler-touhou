/* Entry: 0x0051e030; symbol: FUN_0051e030; body bytes: 79 */

void __thiscall FUN_0051e030(void *this,uint param_1)

{
  *(uint *)((int)this + 0xc) = param_1 >> 0x18;
  *(uint *)((int)this + 8) = (param_1 & 0xff0000) >> 0x10;
  *(uint *)((int)this + 4) = (param_1 & 0xff00) >> 8;
  *(uint *)this = param_1 & 0xff;
  return;
}

