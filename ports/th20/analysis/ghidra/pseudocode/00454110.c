/* Entry: 0x00454110; symbol: FUN_00454110; body bytes: 59 */

void __thiscall FUN_00454110(void *this,float param_1)

{
  *(undefined4 *)this = *(undefined4 *)((int)this + 4);
  *(float *)((int)this + 8) = *(float *)((int)this + 8) + param_1;
  *(int *)((int)this + 4) = (int)*(float *)((int)this + 8);
  return;
}

