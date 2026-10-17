/* Entry: 0x004cf940; symbol: FUN_004cf940; body bytes: 235 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_004cf940(int *param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = (**(code **)(*param_1 + 0x18))();
  if (iVar1 == 0) {
    (**(code **)(*param_1 + 0x4c))(0);
    iVar1 = FUN_00437cd0((int)(param_1 + 0x1dc));
    FUN_004777f0(param_1 + 0x1dc,(float)param_1[0x1e] / *(float *)(iVar1 + 0x4c));
    iVar1 = FUN_00437cd0((int)(param_1 + 0x1dc));
    FUN_004777c0(param_1 + 0x1dc,(float)param_1[0x1d] / *(float *)(iVar1 + 0x48));
    FUN_0042b5d0(param_1 + 0x1dc);
    if ((float)param_1[0x20] == _DAT_0056e0e8) {
      FUN_0042b5d0(param_1 + 0x355);
    }
    param_1[7] = param_1[7] & 0xffffffdf;
    uVar2 = 0;
  }
  else {
    uVar2 = 1;
  }
  return uVar2;
}

