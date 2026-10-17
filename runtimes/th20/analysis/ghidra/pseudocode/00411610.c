/* Entry: 0x00411610; symbol: FUN_00411610; body bytes: 67 */

void __thiscall FUN_00411610(void *this,uint param_1)

{
  int iVar1;
  
  if (*(uint *)((int)this + 0x14) < param_1) {
    iVar1 = *(int *)((int)this + 0x10);
    FUN_0040d5e0(this,param_1 - iVar1);
    *(int *)((int)this + 0x10) = iVar1;
  }
  return;
}

