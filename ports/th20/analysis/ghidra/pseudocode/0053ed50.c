/* Entry: 0x0053ed50; symbol: FUN_0053ed50; body bytes: 203 */

float __thiscall FUN_0053ed50(int param_1,float param_2)

{
  code *pcVar1;
  int iVar2;
  float *pfVar3;
  
  iVar2 = FUN_005403d0(param_1);
  if (((uint)*(ushort *)(iVar2 + 8) & 1 << (SUB41(param_2,0) & 0x1f)) == 0) {
    param_2 = *(float *)(iVar2 + 0x10 + (int)param_2 * 4);
  }
  else if (*(int *)(iVar2 + 0x10 + (int)param_2 * 4) < 0) {
    if (*(int *)(iVar2 + 0x10 + (int)param_2 * 4) < -100) {
      pcVar1 = *(code **)(**(int **)(param_1 + 0x28) + 8);
      param_2 = (float)(*pcVar1)(*(undefined4 *)(iVar2 + 0x10 + (int)param_2 * 4),pcVar1,
                                 *(undefined4 *)(param_1 + 0x28));
    }
    else {
      FUN_00540450((void *)(param_1 + 0xc),*(int *)(iVar2 + 0x10 + (int)param_2 * 4) << 3,&param_2,
                   'i');
    }
  }
  else {
    pfVar3 = (float *)FUN_0053e6a0((void *)(param_1 + 0xc),*(int *)(iVar2 + 0x10 + (int)param_2 * 4)
                                  );
    param_2 = *pfVar3;
  }
  return param_2;
}

