/* Entry: 0x00530d60; symbol: FUN_00530d60; body bytes: 134 */

void FUN_00530d60(void *param_1,int param_2)

{
  int iVar1;
  float fVar2;
  
  *(code **)((int)param_1 + 0x11c) = FUN_00530e80;
  iVar1 = FUN_00464080(0);
  iVar1 = FUN_004b81a0(iVar1);
  if (iVar1 != 0) {
    fVar2 = DAT_0056e0f0 * DAT_0056c8d0;
    iVar1 = FUN_00464080(0);
    iVar1 = FUN_004b81a0(iVar1);
    FUN_005305c0(param_1,(float)param_2 * (fVar2 / (float)iVar1));
  }
  return;
}

