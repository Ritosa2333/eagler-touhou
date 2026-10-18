/* Entry: 0x00450430; symbol: FUN_00450430; body bytes: 48 */

void __thiscall FUN_00450430(void *this,uint param_1)

{
  *(uint *)((int)this + 0x490) = *(uint *)((int)this + 0x490) & 0xff000000 | param_1 & 0xffffff;
  return;
}

