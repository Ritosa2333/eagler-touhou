/* Entry: 0x004aa1b0; symbol: FUN_004aa1b0; body bytes: 146 */

undefined4 __cdecl FUN_004aa1b0(int param_1)

{
  undefined4 uVar1;
  uint uVar2;
  
  if (DAT_005ba828 == 0) {
    uVar1 = 1;
  }
  else {
    uVar2 = FUN_00424030(DAT_005ba828);
    if (uVar2 == 0) {
      if ((*(uint *)(DAT_005ba828 + 0xe8) >> 0xb & 1) == 0) {
        uVar2 = FUN_00424060(DAT_005ba828);
        if (uVar2 == 0) {
          uVar2 = FUN_004ab270(DAT_005ba828);
          if ((uVar2 & 0xff) == 0) {
            uVar2 = FUN_004ab2e0(DAT_005ba828);
            if (uVar2 == 0) {
              uVar1 = FUN_004a5040(param_1);
            }
            else {
              uVar1 = 1;
            }
          }
          else {
            uVar1 = 1;
          }
        }
        else {
          uVar1 = 1;
        }
      }
      else {
        uVar1 = 1;
      }
    }
    else {
      uVar1 = 1;
    }
  }
  return uVar1;
}

