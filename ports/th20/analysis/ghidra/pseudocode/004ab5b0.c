/* Entry: 0x004ab5b0; symbol: FUN_004ab5b0; body bytes: 81 */

void __thiscall FUN_004ab5b0(void *this,int param_1,undefined4 param_2)

{
  undefined4 local_10;
  undefined4 local_8;
  
  if (param_1 < 100) {
    local_8 = param_1;
  }
  else {
    local_8 = 99;
  }
  *(int *)((int)this + 0x1c4) = local_8;
  if (param_1 < 100) {
    local_10 = param_2;
  }
  else {
    local_10 = 99;
  }
  *(undefined4 *)((int)this + 0x1c8) = local_10;
  return;
}

