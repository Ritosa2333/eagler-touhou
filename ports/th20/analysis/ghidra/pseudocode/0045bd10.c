/* Entry: 0x0045bd10; symbol: FUN_0045bd10; body bytes: 409 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

float10 __fastcall FUN_0045bd10(int param_1)

{
  int iVar1;
  int iVar2;
  float10 fVar3;
  double local_28;
  
  fVar3 = (float10)FUN_0041cb10();
  iVar1 = *(int *)(*(int *)(*(int *)(param_1 + 0x10) + 0x90) + 0x24);
  iVar2 = *(int *)(*(int *)(*(int *)(param_1 + 0x10) + 0x90) + 0x24);
  for (local_28 = (double)fVar3 - (*(double *)(param_1 + 0x38) + *(double *)(param_1 + 0x48));
      (((double)*(int *)(*(int *)(*(int *)(param_1 + 0x10) + 0x90) + 0x1c) /
       (((double)iVar1 + *(double *)(&DAT_0056cdd0 + (iVar1 >> 0x1f) * -8)) / _DAT_0056ec08)) /
      (double)*(ushort *)(*(int *)(*(int *)(param_1 + 0x10) + 0x90) + 0x2e)) /
      (double)*(ushort *)(*(int *)(*(int *)(param_1 + 0x10) + 0x90) + 0x22) <= local_28;
      local_28 = local_28 -
                 ((((double)*(int *)(*(int *)(*(int *)(param_1 + 0x10) + 0x90) + 0x1c) -
                   (double)*(int *)(*(int *)(*(int *)(param_1 + 0x10) + 0x90) + 0x18)) /
                  ((double)iVar2 + *(double *)(&DAT_0056cdd0 + (iVar2 >> 0x1f) * -8))) /
                 ((double)*(ushort *)(*(int *)(*(int *)(param_1 + 0x10) + 0x90) + 0x2e) /
                 _DAT_0056ec08)) /
                 (double)*(ushort *)(*(int *)(*(int *)(param_1 + 0x10) + 0x90) + 0x22)) {
  }
  return (float10)local_28;
}

