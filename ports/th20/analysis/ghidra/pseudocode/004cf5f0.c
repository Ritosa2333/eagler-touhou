/* Entry: 0x004cf5f0; symbol: FUN_004cf5f0; body bytes: 138 */

undefined4 __fastcall FUN_004cf5f0(int *param_1)

{
  int iVar1;
  undefined4 uVar2;
  
  iVar1 = (**(code **)(*param_1 + 0x18))();
  if (iVar1 == 0) {
    (**(code **)(*param_1 + 0x4c))(0);
    FUN_0042b5d0(param_1 + 0x1d5);
    FUN_0042b5d0(param_1 + 0x34e);
    FUN_004c9100(param_1 + 0x12,(float)param_1[0x21] * (float)param_1[0x22]);
    param_1[7] = param_1[7] & 0xffffffdf;
    uVar2 = 0;
  }
  else {
    uVar2 = 0xffffffff;
  }
  return uVar2;
}

