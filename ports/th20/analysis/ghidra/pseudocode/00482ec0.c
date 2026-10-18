/* Entry: 0x00482ec0; symbol: FUN_00482ec0; body bytes: 329 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00482ec0(int param_1)

{
  int iVar1;
  float *pfVar2;
  uint *puVar3;
  undefined4 uVar4;
  float10 fVar5;
  float fVar6;
  undefined4 local_c;
  
  iVar1 = FUN_0040c300(*(undefined4 **)(param_1 + 0x520));
  pfVar2 = (float *)FUN_00485990(iVar1);
  if (0.0 < *pfVar2) {
    iVar1 = FUN_0040c300(*(undefined4 **)(param_1 + 0x520));
    puVar3 = (uint *)FUN_00485990(iVar1);
    local_c = *puVar3;
  }
  else {
    iVar1 = FUN_0047c680((void *)(param_1 + 0xa0),4);
    local_c = *(uint *)(iVar1 + 0x18);
  }
  fVar6 = (float)(local_c ^ _DAT_0056d7d0) / DAT_0056c8d0;
  if (fVar6 < *(float *)(param_1 + 100) || fVar6 == *(float *)(param_1 + 100)) {
    uVar4 = 0;
  }
  else {
    iVar1 = FUN_0047c680((void *)(param_1 + 0xa0),4);
    if ((*(uint *)(iVar1 + 0x3c) & 0x10) == 0) {
      fVar5 = FUN_004292e0((float *)(param_1 + 0x84));
      FUN_00452f20((void *)(param_1 + 0x84),
                   (float)((uint)(float)fVar5 ^ _DAT_0056d7d0) - DAT_0056e0f0);
      fVar6 = 0.0;
      fVar5 = FUN_004292e0((float *)(param_1 + 0x84));
      fVar5 = (float10)FUN_00438600((float)fVar5,fVar6);
      FUN_00452f20((void *)(param_1 + 0x84),(float)fVar5);
      *(float *)(param_1 + 100) = (float)(local_c ^ _DAT_0056d7d0) - *(float *)(param_1 + 100);
    }
    uVar4 = 1;
  }
  return uVar4;
}

