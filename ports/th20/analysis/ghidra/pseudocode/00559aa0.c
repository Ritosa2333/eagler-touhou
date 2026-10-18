/* Entry: 0x00559aa0; symbol: FUN_00559aa0; body bytes: 471 */

uint FUN_00559aa0(void)

{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  uint uVar6;
  ushort in_FPUControlWord;
  undefined4 local_24;
  undefined4 local_8;
  
  local_24 = (uint)in_FPUControlWord;
  if ((in_FPUControlWord & 0x300) == 0) {
    local_8 = 0x2000;
  }
  else if ((local_24 & 0x300) == 0x200) {
    local_8 = 0x1000;
  }
  else {
    local_8 = 0;
  }
  uVar2 = local_24 & 0xc00;
  if (uVar2 < 0x801) {
    if (uVar2 == 0x800) {
      uVar2 = 0x200;
    }
    else {
      if (((in_FPUControlWord & 0xc00) == 0) || (uVar2 != 0x400)) goto LAB_00559b33;
      uVar2 = 0x100;
    }
  }
  else if (uVar2 == 0xc00) {
    uVar2 = 0x300;
  }
  else {
LAB_00559b33:
    uVar2 = 0;
  }
  uVar6 = (((local_24 & 7999) >> 2 & 8 | local_24 & 0x10) >> 2 | local_24 & 8) >> 1 |
          ((local_24 & 2) << 3 | local_24 & 4) * 2 | (local_24 & 1) << 4;
  if (DAT_005e5288 < 1) {
    uVar4 = 0;
  }
  else {
    uVar4 = MXCSR & 0xffc0;
  }
  uVar1 = (ushort)uVar4 & 0x8040;
  if (uVar1 == 0x8000) {
    uVar5 = 0xc00;
  }
  else if (uVar1 == 0x40) {
    uVar5 = 0x800;
  }
  else {
    uVar5 = 0;
    if (uVar1 == 0x8040) {
      uVar5 = 0x400;
    }
  }
  uVar3 = uVar4 & 0x6000;
  if (uVar3 < 0x4001) {
    if (uVar3 == 0x4000) {
      uVar3 = 0x200;
      goto LAB_00559c03;
    }
    if ((uVar3 != 0) && (uVar3 == 0x2000)) {
      uVar3 = 0x100;
      goto LAB_00559c03;
    }
  }
  else if (uVar3 == 0x6000) {
    uVar3 = 0x300;
    goto LAB_00559c03;
  }
  uVar3 = 0;
LAB_00559c03:
  uVar4 = ((((uVar4 >> 2 & 0x400 | uVar4 & 0x800) >> 2 | uVar4 & 0x400) >> 2 | uVar4 & 0x200) >> 3 |
          uVar4 & 0x180) >> 3;
  return (((uVar3 | uVar4 << 2) << 6 | uVar6) << 2 | uVar2) << 0xe | uVar4 | uVar5 | uVar3 |
         uVar6 | (local_24 & 0x1000) << 2 | local_8 | uVar2;
}

