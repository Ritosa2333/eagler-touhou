/* Entry: 0x00483150; symbol: FUN_00483150; body bytes: 277 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00483150(int param_1)

{
  int iVar1;
  undefined4 uVar2;
  float10 fVar3;
  float fVar4;
  undefined4 local_c;
  
  iVar1 = FUN_0040c300(*(undefined4 **)(param_1 + 0x520));
  iVar1 = FUN_00485990(iVar1);
  if (0.0 < *(float *)(iVar1 + 4)) {
    iVar1 = FUN_0040c300(*(undefined4 **)(param_1 + 0x520));
    iVar1 = FUN_00485990(iVar1);
    local_c = *(float *)(iVar1 + 4);
  }
  else {
    iVar1 = FUN_0047c680((void *)(param_1 + 0xa0),4);
    local_c = *(float *)(iVar1 + 0x1c);
  }
  fVar4 = DAT_0056d7c0 / DAT_0056c8d0 - local_c / DAT_0056c8d0;
  if (fVar4 < *(float *)(param_1 + 0x68) || fVar4 == *(float *)(param_1 + 0x68)) {
    uVar2 = 0;
  }
  else {
    iVar1 = FUN_0047c680((void *)(param_1 + 0xa0),4);
    if ((*(uint *)(iVar1 + 0x3c) & 0x10) == 0) {
      fVar3 = FUN_004292e0((float *)(param_1 + 0x84));
      FUN_00452f20((void *)(param_1 + 0x84),(float)((uint)(float)fVar3 ^ _DAT_0056d7d0));
      *(float *)(param_1 + 0x68) = (DAT_0056d7c0 - local_c) - *(float *)(param_1 + 0x68);
    }
    uVar2 = 1;
  }
  return uVar2;
}

