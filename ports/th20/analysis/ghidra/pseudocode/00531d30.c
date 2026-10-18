/* Entry: 0x00531d30; symbol: FUN_00531d30; body bytes: 163 */

undefined4 FUN_00531d30(void *param_1)

{
  int iVar1;
  int iVar2;
  float10 fVar3;
  
  iVar1 = *(int *)((int)param_1 + 0x5c8);
  fVar3 = FUN_0042a110((float *)(iVar1 + 0x60));
  *(float *)(iVar1 + 0x5c) = (float)fVar3;
  iVar2 = FUN_00534260();
  *(float *)(iVar1 + 0x90) = *(float *)(iVar1 + 0x90) - DAT_00575d58 / (float)iVar2;
  FUN_0045dd60(param_1,(undefined4 *)(iVar1 + 0x8c));
  FUN_00470e50(param_1,*(undefined4 *)(iVar1 + 0x5c),DAT_0056c8cc);
  return 0;
}

