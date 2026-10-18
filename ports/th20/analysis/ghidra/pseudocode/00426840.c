/* Entry: 0x00426840; symbol: FUN_00426840; body bytes: 71 */

undefined4 __thiscall FUN_00426840(void *this,undefined1 (*param_1) [16])

{
  undefined4 uVar1;
  undefined4 local_c;
  
  uVar1 = FUN_00410aa0(param_1,(SIZE_T *)0x0,0);
  *(undefined4 *)((int)this + 0x1890) = uVar1;
  if (*(int *)((int)this + 0x1890) == 0) {
    local_c = 0xffffffff;
  }
  else {
    local_c = 0;
  }
  return local_c;
}

