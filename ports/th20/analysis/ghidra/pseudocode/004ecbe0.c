/* Entry: 0x004ecbe0; symbol: FUN_004ecbe0; body bytes: 57 */

undefined4 __cdecl FUN_004ecbe0(uint param_1,int param_2)

{
  undefined4 uVar1;
  
  if ((param_2 == 0) && (param_1 < 0x80)) {
    uVar1 = 0xffffff01;
  }
  else {
    uVar1 = 0xffffff00;
  }
  return uVar1;
}

