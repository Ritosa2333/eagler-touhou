/* Entry: 0x00535d00; symbol: FUN_00535d00; body bytes: 279 */

void __fastcall FUN_00535d00(int param_1)

{
  bool bVar1;
  void *pvVar2;
  int iVar3;
  float *pfVar4;
  undefined4 *puVar5;
  float10 fVar6;
  float fVar7;
  undefined1 auStack_5c [20];
  undefined4 uStack_48;
  undefined1 local_c [4];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  bVar1 = FUN_00461070((void *)(param_1 + 0x48),0);
  if (bVar1) {
    FUN_0040c080(local_c,4);
    FUN_004142a0(local_c,param_1);
    fVar6 = FUN_00423bd0(param_1 + 0x48);
    fVar7 = (DAT_0056e048 - (float)fVar6) * DAT_0056e734;
    pvVar2 = (void *)FUN_00478e80(0);
    iVar3 = FUN_00460830(0);
    pfVar4 = (float *)FUN_00460850(iVar3);
    FUN_00535850(auStack_5c,local_c);
    FUN_0047d240(pvVar2,pfVar4,fVar7);
    pvVar2 = (void *)FUN_00478ee0(0);
    iVar3 = FUN_00460830(0);
    puVar5 = (undefined4 *)FUN_00460850(iVar3);
    uStack_48 = 0x535df5;
    FUN_004caad0(pvVar2,puVar5,fVar7,0,1);
    FUN_00429420((void *)(param_1 + 0x48));
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

