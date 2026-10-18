/* Entry: 0x004dbd70; symbol: FUN_004dbd70; body bytes: 891 */

void __fastcall FUN_004dbd70(int param_1)

{
  byte bVar1;
  int *piVar2;
  int iVar3;
  undefined3 extraout_var;
  undefined4 *puVar4;
  undefined4 *puVar5;
  
  if (*(int *)(param_1 + 0x19c) == 0) {
    if (*(int *)(param_1 + 0x1a4) == 0) {
      piVar2 = (int *)FUN_00412730(param_1);
      iVar3 = (**(code **)(*piVar2 + 0x48))(piVar2,0,0,0,param_1 + 0x1a4);
      if (iVar3 != 0) {
        return;
      }
    }
    piVar2 = (int *)FUN_004bfb00(*(void **)(param_1 + 0xb40),0);
    (**(code **)(*piVar2 + 0x48))(piVar2,0,param_1 + 0x19c);
    piVar2 = (int *)FUN_004bfb00(*(void **)(param_1 + 0xb40),1);
    (**(code **)(*piVar2 + 0x48))(piVar2,0,param_1 + 0x1a0);
    bVar1 = FUN_00445ab0(*(int *)(param_1 + 0x1ac));
    if (CONCAT31(extraout_var,bVar1) == 0) {
      if (DAT_005b87f8 == 0x280) {
        FUN_004382b0(*(void **)(param_1 + 0xb40),*(void **)(param_1 + 0x1ac),0,0);
        FUN_004382b0(*(void **)(param_1 + 0xb40),*(void **)(param_1 + 0x1b0),7,0);
        FUN_004382b0(*(void **)(param_1 + 0xb40),*(void **)(param_1 + 0x1b4),4,0);
        FUN_004382b0(*(void **)(param_1 + 0xb40),*(void **)(param_1 + 0x1b8),10,0);
        FUN_004382b0(*(void **)(param_1 + 0xb40),*(void **)(param_1 + 0x1bc),0xd,0);
      }
      else if (DAT_005b87f8 == 0x3c0) {
        FUN_004382b0(*(void **)(param_1 + 0xb40),*(void **)(param_1 + 0x1ac),1,0);
        FUN_004382b0(*(void **)(param_1 + 0xb40),*(void **)(param_1 + 0x1b0),8,0);
        FUN_004382b0(*(void **)(param_1 + 0xb40),*(void **)(param_1 + 0x1b4),5,0);
        FUN_004382b0(*(void **)(param_1 + 0xb40),*(void **)(param_1 + 0x1b8),0xb,0);
        FUN_004382b0(*(void **)(param_1 + 0xb40),*(void **)(param_1 + 0x1bc),0xe,0);
      }
      else if (DAT_005b87f8 == 0x500) {
        FUN_004382b0(*(void **)(param_1 + 0xb40),*(void **)(param_1 + 0x1ac),2,0);
        FUN_004382b0(*(void **)(param_1 + 0xb40),*(void **)(param_1 + 0x1b0),9,0);
        FUN_004382b0(*(void **)(param_1 + 0xb40),*(void **)(param_1 + 0x1b4),6,0);
        FUN_004382b0(*(void **)(param_1 + 0xb40),*(void **)(param_1 + 0x1b8),0xc,0);
        FUN_004382b0(*(void **)(param_1 + 0xb40),*(void **)(param_1 + 0x1bc),0xf,0);
      }
    }
    if (DAT_005b8818 == DAT_0056cd68) {
      FUN_004dda30(*(void **)(param_1 + 0x1b4),0);
    }
  }
  else {
    puVar4 = (undefined4 *)(param_1 + 0x278);
    puVar5 = (undefined4 *)(param_1 + 0x6bc);
    for (iVar3 = 0x5b; iVar3 != 0; iVar3 = iVar3 + -1) {
      *puVar5 = *puVar4;
      puVar4 = puVar4 + 1;
      puVar5 = puVar5 + 1;
    }
  }
  return;
}

