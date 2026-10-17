/* Entry: 0x00421990; symbol: FUN_00421990; body bytes: 49 */

undefined4 __thiscall FUN_00421990(void *this,int param_1)

{
  undefined4 local_8;
  
  if (param_1 == 1) {
    local_8 = 0xffffffff;
  }
  else {
    local_8 = *(undefined4 *)((int)this + param_1 * 4 + 0x2e24);
  }
  return local_8;
}

