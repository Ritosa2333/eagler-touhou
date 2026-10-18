/* Entry: 0x005081e0; symbol: FUN_005081e0; body bytes: 294 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_005081e0(int param_1)

{
  uint uVar1;
  int iVar2;
  uint local_10;
  uint local_c;
  
  if (DAT_005ba828 == 0) {
    return 1;
  }
  if (*(int *)(param_1 + 0x10) == 1) {
    if (DAT_005b889c == (void *)0x0) {
      local_c = 0;
    }
    else {
      local_c = FUN_004495a0(DAT_005b889c,0x200);
    }
    if (local_c == 0) {
      if (DAT_005b889c == (void *)0x0) {
        local_10 = 0;
      }
      else {
        local_10 = FUN_004495a0(DAT_005b889c,1);
      }
      if (local_10 == 0) {
        if (*(int *)(param_1 + 0x14) != 0) {
          iVar2 = FUN_00498f40();
          iVar2 = FUN_0050a050(iVar2);
          FUN_004e68c0(&DAT_005ba830,(double)(iVar2 + -8) / _DAT_0056c470);
        }
        *(undefined4 *)(param_1 + 0x14) = 0;
        return 1;
      }
    }
    *(undefined4 *)(param_1 + 0x14) = 1;
    uVar1 = *(uint *)(param_1 + 0x250) & 0x80000007;
    if ((int)uVar1 < 0) {
      uVar1 = (uVar1 - 1 | 0xfffffff8) + 1;
    }
    if (uVar1 != 0) {
      return 6;
    }
  }
  return 1;
}

