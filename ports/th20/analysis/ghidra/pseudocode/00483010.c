/* Entry: 0x00483010; symbol: FUN_00483010; body bytes: 319 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00483010(int param_1)

{
  int iVar1;
  float *pfVar2;
  undefined4 uVar3;
  float10 fVar4;
  float fVar5;
  undefined4 local_c;
  
  iVar1 = FUN_0040c300(*(undefined4 **)(param_1 + 0x520));
  pfVar2 = (float *)FUN_00485990(iVar1);
  if (0.0 < *pfVar2) {
    iVar1 = FUN_0040c300(*(undefined4 **)(param_1 + 0x520));
    pfVar2 = (float *)FUN_00485990(iVar1);
    local_c = *pfVar2;
  }
  else {
    iVar1 = FUN_0047c680((void *)(param_1 + 0xa0),4);
    local_c = *(float *)(iVar1 + 0x18);
  }
  if (*(float *)(param_1 + 100) < local_c / DAT_0056c8d0) {
    uVar3 = 0;
  }
  else {
    iVar1 = FUN_0047c680((void *)(param_1 + 0xa0),4);
    if ((*(uint *)(iVar1 + 0x3c) & 0x10) == 0) {
      fVar4 = FUN_004292e0((float *)(param_1 + 0x84));
      FUN_00452f20((void *)(param_1 + 0x84),
                   (float)((uint)(float)fVar4 ^ _DAT_0056d7d0) - DAT_0056e0f0);
      fVar5 = 0.0;
      fVar4 = FUN_004292e0((float *)(param_1 + 0x84));
      fVar4 = (float10)FUN_00438600((float)fVar4,fVar5);
      FUN_00452f20((void *)(param_1 + 0x84),(float)fVar4);
      *(float *)(param_1 + 100) = local_c - *(float *)(param_1 + 100);
    }
    uVar3 = 1;
  }
  return uVar3;
}

