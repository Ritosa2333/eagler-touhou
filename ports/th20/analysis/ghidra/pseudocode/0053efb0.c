/* Entry: 0x0053efb0; symbol: FUN_0053efb0; body bytes: 152 */

float __thiscall FUN_0053efb0(int param_1,byte param_2,float param_3)

{
  code *pcVar1;
  int iVar2;
  float *pfVar3;
  
  iVar2 = FUN_005403d0(param_1);
  if (((uint)*(ushort *)(iVar2 + 8) & 1 << (param_2 & 0x1f)) != 0) {
    if ((int)param_3 < 0) {
      if ((int)param_3 < -100) {
        pcVar1 = *(code **)(**(int **)(param_1 + 0x28) + 8);
        param_3 = (float)(*pcVar1)(param_3,pcVar1,*(undefined4 *)(param_1 + 0x28));
      }
      else {
        FUN_0053f0b0((void *)(param_1 + 0xc),4,&param_3,'i');
      }
    }
    else {
      pfVar3 = (float *)FUN_0053e6a0((void *)(param_1 + 0xc),(int)param_3);
      param_3 = *pfVar3;
    }
  }
  return param_3;
}

