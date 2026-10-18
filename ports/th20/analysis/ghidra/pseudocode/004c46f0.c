/* Entry: 0x004c46f0; symbol: FUN_004c46f0; body bytes: 79 */

undefined4 __cdecl FUN_004c46f0(void *param_1)

{
  uint uVar1;
  undefined4 uVar2;
  
  if ((DAT_005ba828 != 0) && (uVar1 = FUN_00424030(DAT_005ba828), uVar1 != 0)) {
    return 1;
  }
  if ((DAT_005ba828 == 0) || ((*(uint *)(DAT_005ba828 + 0xe8) >> 0xb & 1) == 0)) {
    uVar2 = FUN_004c25a0(param_1);
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}

