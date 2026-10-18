/* Entry: 0x005372c0; symbol: FUN_005372c0; body bytes: 332 */

void __fastcall FUN_005372c0(int param_1)

{
  bool bVar1;
  char cVar2;
  undefined3 extraout_var;
  int iVar3;
  void *pvVar4;
  float *pfVar5;
  undefined1 auStack_60 [32];
  undefined4 uStack_40;
  int local_24;
  int local_1c;
  float local_18;
  float local_14;
  float local_10;
  undefined1 local_c [4];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *(undefined1 *)(param_1 + 0x35) = 0;
  bVar1 = FUN_0045d030((int *)(param_1 + 0x38));
  if (CONCAT31(extraout_var,bVar1) != 0) {
    local_1c = 0;
    while( true ) {
      uStack_40 = 0x537306;
      iVar3 = FUN_00464080(0);
      iVar3 = FUN_004b81a0(iVar3);
      if (iVar3 <= local_1c) break;
      uStack_40 = 0x537320;
      iVar3 = FUN_00464080(0);
      cVar2 = FUN_00464400(iVar3);
      if (cVar2 == '\0') {
        local_24 = 0x10;
      }
      else {
        local_24 = 0x16;
      }
      iVar3 = FUN_0040ff90(param_1 + 0x38);
      if (iVar3 % local_24 == local_1c << 2) {
        uStack_40 = 0x53736b;
        pvVar4 = (void *)FUN_00460830(0);
        uStack_40 = 0x53737d;
        iVar3 = FUN_005357c0(pvVar4,local_1c);
        pfVar5 = (float *)FUN_00421970(iVar3);
        local_18 = *pfVar5;
        local_14 = pfVar5[1];
        local_10 = pfVar5[2];
        uStack_40 = 0x53739f;
        FUN_0040c080(local_c,4);
        uStack_40 = 0x5373ab;
        FUN_004142a0(local_c,param_1);
        uStack_40 = 0x5373b3;
        pvVar4 = (void *)FUN_00478e80(0);
        FUN_00536f10(auStack_60,local_c);
        FUN_0047d240(pvVar4,&local_18,DAT_0056c8d8);
      }
      local_1c = local_1c + 1;
    }
  }
  uStack_40 = 0x5373fd;
  FUN_00423540((undefined4 *)(param_1 + 0x38));
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

