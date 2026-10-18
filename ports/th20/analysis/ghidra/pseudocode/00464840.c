/* Entry: 0x00464840; symbol: FUN_00464840; body bytes: 159 */

undefined4 __fastcall FUN_00464840(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  undefined4 local_10;
  undefined4 local_c;
  
  local_10 = 0;
  for (local_c = 0; local_c < 4; local_c = local_c + 1) {
    iVar1 = FUN_0042b5d0((void *)(param_1 + 8 + local_c * 0x5e4));
    if (iVar1 != 0) {
      local_10 = local_10 + 1;
    }
  }
  if (local_10 < 4) {
    FUN_0042b5d0((void *)(param_1 + 0x1798));
    *(int *)(param_1 + 0x1d80) = *(int *)(param_1 + 0x1d80) + 1;
    if ((*(int *)(param_1 + 0x1d84) == 0) || (*(int *)(param_1 + 0x1d80) < 0x3c)) {
      uVar2 = 0;
    }
    else {
      uVar2 = 1;
    }
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}

