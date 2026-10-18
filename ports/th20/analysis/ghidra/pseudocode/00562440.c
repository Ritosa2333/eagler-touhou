/* Entry: 0x00562440; symbol: FUN_00562440; body bytes: 242 */

uint __cdecl FUN_00562440(uint param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar2 = param_1 >> 4 & 1;
  uVar1 = uVar2 | 4;
  if ((param_1 & 8) == 0) {
    uVar1 = uVar2;
  }
  uVar2 = uVar1 | 8;
  if ((param_1 & 4) == 0) {
    uVar2 = uVar1;
  }
  uVar1 = uVar2 | 0x10;
  if ((param_1 & 2) == 0) {
    uVar1 = uVar2;
  }
  uVar2 = uVar1 | 0x20;
  if ((param_1 & 1) == 0) {
    uVar2 = uVar1;
  }
  uVar1 = uVar2 | 2;
  if ((param_1 & 0x80000) == 0) {
    uVar1 = uVar2;
  }
  uVar2 = param_1 & 0x300;
  if (uVar2 < 0x201) {
    if (uVar2 == 0x200) {
      uVar1 = uVar1 | 0x800;
    }
    else if ((uVar2 != 0) && (uVar2 == 0x100)) {
      uVar1 = uVar1 | 0x400;
    }
  }
  else if (uVar2 == 0x300) {
    uVar1 = uVar1 | 0xc00;
  }
  if ((param_1 & 0x30000) == 0) {
    uVar1 = uVar1 | 0x300;
  }
  else if ((param_1 & 0x30000) == 0x10000) {
    uVar1 = uVar1 | 0x200;
  }
  uVar2 = uVar1 | 0x1000;
  if ((param_1 & 0x40000) == 0) {
    uVar2 = uVar1;
  }
  return uVar2;
}

