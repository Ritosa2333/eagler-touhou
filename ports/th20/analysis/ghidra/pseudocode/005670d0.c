/* Entry: 0x005670d0; symbol: FUN_005670d0; body bytes: 193 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __cdecl FUN_005670d0(int param_1,uint param_2)

{
  int iVar1;
  undefined4 uVar2;
  
  if ((param_2 & 0x7ff00000) != 0x7ff00000) {
    if (((param_2 & 0x7ff00000) == 0) && (param_1 != 0 || (param_2 & 0xfffff) != 0)) {
      uVar2 = 0x80;
      if ((param_2 & 0x80000000) != 0) {
        uVar2 = 0x10;
      }
      return uVar2;
    }
    if ((double)CONCAT26(param_2._2_2_,CONCAT24((undefined2)param_2,param_1)) == _DAT_0056cd50) {
      uVar2 = 0x40;
      if ((param_2 & 0x80000000) != 0) {
        uVar2 = 0x20;
      }
      return uVar2;
    }
    uVar2 = 0x100;
    if ((param_2 & 0x80000000) != 0) {
      uVar2 = 8;
    }
    return uVar2;
  }
  iVar1 = FUN_005524b0(param_1,(uint)(CONCAT26(param_2._2_2_,CONCAT24((undefined2)param_2,param_1))
                                     >> 0x20));
  if (iVar1 == 1) {
    return 0x200;
  }
  if (iVar1 != 2) {
    if (iVar1 != 3) {
      return 1;
    }
    return 2;
  }
  return 4;
}

