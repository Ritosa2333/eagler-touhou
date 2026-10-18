/* Entry: 0x004b6460; symbol: FUN_004b6460; body bytes: 60 */

undefined4 __cdecl FUN_004b6460(byte param_1)

{
  undefined4 uVar1;
  
  if (((param_1 < 0x81) || (0x9f < param_1)) && ((param_1 < 0xe0 || (0xfc < param_1)))) {
    uVar1 = 0;
  }
  else {
    uVar1 = 1;
  }
  return uVar1;
}

