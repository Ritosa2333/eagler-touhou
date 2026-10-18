/* Entry: 0x004ecb80; symbol: FUN_004ecb80; body bytes: 84 */

undefined4 __cdecl FUN_004ecb80(uint param_1,uint param_2)

{
  undefined4 uVar1;
  
  if ((param_2 < 0x80000000) || ((-2 < (int)param_2 && (0xffffff7f < param_1)))) {
    if (((int)param_2 < 0) || (((int)param_2 < 1 && (param_1 < 0x80)))) {
      uVar1 = 1;
    }
    else {
      uVar1 = 0;
    }
  }
  else {
    uVar1 = 0;
  }
  return uVar1;
}

