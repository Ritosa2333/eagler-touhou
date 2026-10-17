/* Entry: 0x00561ec0; symbol: FUN_00561ec0; body bytes: 199 */

uint __cdecl FUN_00561ec0(uint param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar2 = (param_1 & 0x10) << 3;
  uVar1 = uVar2 | 0x200;
  if ((param_1 & 8) == 0) {
    uVar1 = uVar2;
  }
  uVar2 = uVar1 | 0x400;
  if ((param_1 & 4) == 0) {
    uVar2 = uVar1;
  }
  uVar1 = uVar2 | 0x800;
  if ((param_1 & 2) == 0) {
    uVar1 = uVar2;
  }
  uVar2 = uVar1 | 0x1000;
  if ((param_1 & 1) == 0) {
    uVar2 = uVar1;
  }
  uVar1 = uVar2 | 0x100;
  if ((param_1 & 0x80000) == 0) {
    uVar1 = uVar2;
  }
  uVar2 = param_1 & 0x300;
  if (uVar2 < 0x201) {
    if (uVar2 == 0x200) {
      uVar1 = uVar1 | 0x4000;
    }
    else if ((uVar2 != 0) && (uVar2 == 0x100)) {
      uVar1 = uVar1 | 0x2000;
    }
  }
  else if (uVar2 == 0x300) {
    uVar1 = uVar1 | 0x6000;
  }
  uVar2 = param_1 & 0x3000000;
  if (uVar2 == 0x1000000) {
    uVar1 = uVar1 | 0x8040;
  }
  else {
    if (uVar2 == 0x2000000) {
      return uVar1 | 0x40;
    }
    if (uVar2 == 0x3000000) {
      return uVar1 | 0x8000;
    }
  }
  return uVar1;
}

