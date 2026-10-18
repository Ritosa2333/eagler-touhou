/* Entry: 0x004e54c0; symbol: FUN_004e54c0; body bytes: 350 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_004e54c0(void *param_1)

{
  bool bVar1;
  int iVar2;
  int iVar3;
  char *pcVar4;
  float fVar5;
  char *pcVar6;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_00422dd0(&local_14,DAT_0056fb7c,DAT_0056fa28,0);
  iVar2 = FUN_0040c300((undefined4 *)((int)param_1 + 0x30));
  bVar1 = FUN_00423560((void *)((int)param_1 + 0x10),10);
  if (bVar1) {
    fVar5 = DAT_0056ed14 - ((float)iVar2 * _DAT_0056f2fc + DAT_0056fa28);
    iVar3 = FUN_0040ff90((int)param_1 + 0x10);
    local_10 = (fVar5 * (float)iVar3) / DAT_0056e734 + (float)iVar2 * _DAT_0056f2fc + DAT_0056fa28;
  }
  else {
    local_10 = DAT_0056ed14;
  }
  local_14 = local_14 + _DAT_00572650;
  FUN_004e5020(param_1,local_14);
  local_14 = DAT_0056fb7c;
  pcVar6 = "        ";
  iVar3 = 9;
  pcVar4 = (char *)FUN_00421810(DAT_005c60fc);
  FUN_00548610(pcVar4,iVar3,(int)pcVar6);
  iVar3 = FUN_00421810(DAT_005c60fc);
  FUN_004e5390(iVar2,&local_14,iVar3);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

