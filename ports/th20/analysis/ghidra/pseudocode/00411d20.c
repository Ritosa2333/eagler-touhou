/* Entry: 0x00411d20; symbol: FUN_00411d20; body bytes: 47 */

void __thiscall FUN_00411d20(void *this,int param_1)

{
  undefined4 uVar1;
  
  if (*(int *)((int)this + 0x14) == param_1) {
    uVar1 = FUN_00412730(param_1);
    *(undefined4 *)((int)this + 0x14) = uVar1;
  }
  FUN_00411d50(param_1);
  return;
}

