/* Entry: 0x00551606; symbol: FUN_00551606; body bytes: 95 */

uint __cdecl FUN_00551606(uint param_1)

{
  uint *puVar1;
  uint uVar2;
  
  if (param_1 != 0) {
    if (param_1 < 0xe) {
      return *(uint *)((int)&DAT_005989d0 + param_1 * 8 + 4);
    }
    if ((param_1 < 0x719) && (puVar1 = FUN_00551587(param_1,0x5989d8,0x2d), puVar1 != (uint *)0x0))
    {
      return puVar1[1];
    }
  }
  if (param_1 - 0x13 < 0x12) {
    uVar2 = 0xd;
  }
  else {
    uVar2 = (-(uint)(0xe < param_1 - 0xbc) & 0xe) + 8;
  }
  return uVar2;
}

