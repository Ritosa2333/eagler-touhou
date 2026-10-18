/* Entry: 0x005620b0; symbol: FUN_005620b0; body bytes: 906 */

uint __cdecl FUN_005620b0(uint param_1,uint param_2)

{
  ushort uVar1;
  uint uVar2;
  uint uVar3;
  uint uVar4;
  uint uVar5;
  ushort in_FPUControlWord;
  
  uVar2 = (in_FPUControlWord & 1) << 4;
  uVar5 = uVar2 | 8;
  if ((in_FPUControlWord & 4) == 0) {
    uVar5 = uVar2;
  }
  uVar2 = uVar5 | 4;
  if ((in_FPUControlWord & 8) == 0) {
    uVar2 = uVar5;
  }
  uVar5 = uVar2 | 2;
  if ((in_FPUControlWord & 0x10) == 0) {
    uVar5 = uVar2;
  }
  uVar2 = uVar5 | 1;
  if ((in_FPUControlWord & 0x20) == 0) {
    uVar2 = uVar5;
  }
  uVar5 = uVar2 | 0x80000;
  if ((in_FPUControlWord & 2) == 0) {
    uVar5 = uVar2;
  }
  uVar1 = in_FPUControlWord & 0xc00;
  if (uVar1 < 0x801) {
    if (uVar1 == 0x800) {
      uVar5 = uVar5 | 0x200;
    }
    else if (((in_FPUControlWord & 0xc00) != 0) && (uVar1 == 0x400)) {
      uVar5 = uVar5 | 0x100;
    }
  }
  else if (uVar1 == 0xc00) {
    uVar5 = uVar5 | 0x300;
  }
  if ((in_FPUControlWord & 0x300) == 0) {
    uVar5 = uVar5 | 0x20000;
  }
  else if ((in_FPUControlWord & 0x300) == 0x200) {
    uVar5 = uVar5 | 0x10000;
  }
  uVar2 = uVar5 | 0x40000;
  if ((in_FPUControlWord & 0x1000) == 0) {
    uVar2 = uVar5;
  }
  uVar5 = ~param_2 & uVar2 | param_1 & param_2;
  if (uVar5 != uVar2) {
    uVar2 = FUN_00562440(uVar5);
    uVar3 = (uVar2 & 1) << 4;
    uVar5 = uVar3 | 8;
    if ((uVar2 & 4) == 0) {
      uVar5 = uVar3;
    }
    uVar3 = uVar5 | 4;
    if ((uVar2 & 8) == 0) {
      uVar3 = uVar5;
    }
    uVar5 = uVar3 | 2;
    if ((uVar2 & 0x10) == 0) {
      uVar5 = uVar3;
    }
    uVar3 = uVar5 | 1;
    if ((uVar2 & 0x20) == 0) {
      uVar3 = uVar5;
    }
    uVar4 = uVar3 | 0x80000;
    if ((uVar2 & 2) == 0) {
      uVar4 = uVar3;
    }
    uVar5 = uVar2 & 0xc00;
    if (uVar5 < 0x801) {
      if (uVar5 == 0x800) {
        uVar4 = uVar4 | 0x200;
      }
      else if (((uVar2 & 0xc00) != 0) && (uVar5 == 0x400)) {
        uVar4 = uVar4 | 0x100;
      }
    }
    else if (uVar5 == 0xc00) {
      uVar4 = uVar4 | 0x300;
    }
    if ((uVar2 & 0x300) == 0) {
      uVar4 = uVar4 | 0x20000;
    }
    else if ((uVar2 & 0x300) == 0x200) {
      uVar4 = uVar4 | 0x10000;
    }
    uVar5 = uVar4 | 0x40000;
    if ((uVar2 & 0x1000) == 0) {
      uVar5 = uVar4;
    }
  }
  if (0 < DAT_005e5288) {
    uVar3 = MXCSR >> 3 & 0x10;
    uVar2 = uVar3 | 8;
    if ((MXCSR & 0x200) == 0) {
      uVar2 = uVar3;
    }
    uVar3 = uVar2 | 4;
    if ((MXCSR & 0x400) == 0) {
      uVar3 = uVar2;
    }
    uVar2 = uVar3 | 2;
    if ((MXCSR & 0x800) == 0) {
      uVar2 = uVar3;
    }
    uVar3 = uVar2 | 1;
    if ((MXCSR & 0x1000) == 0) {
      uVar3 = uVar2;
    }
    uVar2 = uVar3 | 0x80000;
    if ((MXCSR & 0x100) == 0) {
      uVar2 = uVar3;
    }
    uVar3 = MXCSR & 0x6000;
    if (uVar3 < 0x4001) {
      if (uVar3 == 0x4000) {
        uVar2 = uVar2 | 0x200;
      }
      else if ((uVar3 != 0) && (uVar3 == 0x2000)) {
        uVar2 = uVar2 | 0x100;
      }
    }
    else if (uVar3 == 0x6000) {
      uVar2 = uVar2 | 0x300;
    }
    uVar3 = MXCSR & 0x8040;
    if (uVar3 == 0x40) {
      uVar2 = uVar2 | 0x2000000;
    }
    else if (uVar3 == 0x8000) {
      uVar2 = uVar2 | 0x3000000;
    }
    else if (uVar3 == 0x8040) {
      uVar2 = uVar2 | 0x1000000;
    }
    uVar3 = ~(param_2 & 0x308031f) & uVar2 | param_2 & 0x308031f & param_1;
    if (uVar3 != uVar2) {
      uVar2 = FUN_00561ec0(uVar3);
      FUN_0055dae0(uVar2);
      uVar3 = MXCSR >> 3 & 0x10;
      uVar2 = uVar3 | 8;
      if ((MXCSR & 0x200) == 0) {
        uVar2 = uVar3;
      }
      uVar3 = uVar2 | 4;
      if ((MXCSR & 0x400) == 0) {
        uVar3 = uVar2;
      }
      uVar2 = uVar3 | 2;
      if ((MXCSR & 0x800) == 0) {
        uVar2 = uVar3;
      }
      uVar3 = uVar2 | 1;
      if ((MXCSR & 0x1000) == 0) {
        uVar3 = uVar2;
      }
      uVar2 = uVar3 | 0x80000;
      if ((MXCSR & 0x100) == 0) {
        uVar2 = uVar3;
      }
      uVar3 = MXCSR & 0x6000;
      if (uVar3 < 0x4001) {
        if (uVar3 == 0x4000) {
          uVar2 = uVar2 | 0x200;
        }
        else if ((uVar3 != 0) && (uVar3 == 0x2000)) {
          uVar2 = uVar2 | 0x100;
        }
      }
      else if (uVar3 == 0x6000) {
        uVar2 = uVar2 | 0x300;
      }
      uVar3 = MXCSR & 0x8040;
      if (uVar3 == 0x40) {
        uVar2 = uVar2 | 0x2000000;
      }
      else if (uVar3 == 0x8000) {
        uVar2 = uVar2 | 0x3000000;
      }
      else if (uVar3 == 0x8040) {
        uVar2 = uVar2 | 0x1000000;
      }
    }
    uVar3 = uVar2 | uVar5;
    if (((uVar2 ^ uVar5) & 0x8031f) != 0) {
      uVar3 = uVar2 | uVar5 | 0x80000000;
    }
    return uVar3;
  }
  return uVar5;
}

