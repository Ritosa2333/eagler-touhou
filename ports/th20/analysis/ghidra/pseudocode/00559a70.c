/* Entry: 0x00559a70; symbol: FUN_00559a70; body bytes: 38 */

uint __cdecl FUN_00559a70(uint param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar2 = param_1 >> 0xe & 0x300;
  uVar1 = 0xffffffff;
  if (uVar2 == (param_1 >> 0x16 & 0x300)) {
    uVar1 = uVar2;
  }
  return uVar1;
}

