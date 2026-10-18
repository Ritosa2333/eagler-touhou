/* Entry: 0x005348d0; symbol: FUN_005348d0; body bytes: 467 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_005348d0(int param_1)

{
  int *piVar1;
  int iVar2;
  undefined4 *puVar3;
  float10 fVar4;
  undefined4 local_64;
  undefined4 local_60;
  undefined4 local_5c;
  undefined4 local_58;
  void *local_54;
  int local_50;
  int local_4c;
  int local_48;
  undefined4 local_44;
  undefined4 local_40;
  undefined4 local_3c;
  float local_2c;
  float local_28;
  undefined1 local_24 [24];
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_48 = param_1;
  FUN_004a2800(*(void **)(param_1 + 0x40));
  *(undefined4 *)(local_48 + 0x40) = 0;
  piVar1 = FUN_004712d0();
  *(int **)(local_48 + 0x40) = piVar1;
  FUN_00423520((void *)(local_48 + 0x44),0);
  local_54 = *(void **)(local_48 + 0x40);
  FUN_0052cd50(local_54,FUN_00532720);
  FUN_0047ba30(&local_44);
  FUN_00429210(&local_c,0.0);
  iVar2 = FUN_00460830(0);
  puVar3 = (undefined4 *)FUN_00460850(iVar2);
  local_44 = *puVar3;
  local_40 = puVar3[1];
  local_3c = puVar3[2];
  local_28 = DAT_0056e0f0 / _DAT_0056ec8c;
  FUN_0045d650(local_24,0xffffc080);
  for (local_4c = 0; local_4c < 0x18; local_4c = local_4c + 1) {
    fVar4 = FUN_004292e0(&local_c);
    local_2c = (float)fVar4;
    local_58 = FUN_00437520(0);
    FUN_0049db70(&local_60,5,&local_44,0);
    FUN_004530c0(&local_c,(DAT_0056e0f0 * DAT_0056c8d0) / DAT_0056f7b4);
  }
  local_28 = DAT_0056e0f8 / _DAT_0056ec8c;
  for (local_50 = 0; local_50 < 0x18; local_50 = local_50 + 1) {
    fVar4 = FUN_004292e0(&local_c);
    local_2c = (float)fVar4;
    local_5c = FUN_00437520(0);
    FUN_0049db70(&local_64,5,&local_44,0);
    FUN_00472060(&local_c,(DAT_0056e0f0 * DAT_0056c8d0) / DAT_0056f7b4);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

