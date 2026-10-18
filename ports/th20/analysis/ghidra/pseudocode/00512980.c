/* Entry: 0x00512980; symbol: FUN_00512980; body bytes: 285 */

void __fastcall FUN_00512980(int param_1)

{
  bool bVar1;
  int iVar2;
  uint uVar3;
  undefined4 *puVar4;
  undefined4 local_54;
  undefined4 *local_50;
  undefined4 local_4c;
  void *local_48;
  int local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  char local_20;
  char local_1f;
  char local_1e;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_44 = param_1;
  iVar2 = FUN_004aaac0((int *)(param_1 + 0x14));
  if (iVar2 != 0) {
    local_48 = (void *)(local_44 + 0x1c);
    bVar1 = FUN_00461070(local_48,0x3c);
    if (bVar1) {
      FUN_0047ba30(&local_40);
      FUN_0045d650(&local_20,*(undefined4 *)(&DAT_005b0a38 + *(int *)(local_44 + 0x2c) * 4));
      uVar3 = FUN_00423ee0(0x5ba4a8);
      local_1e = local_1e - (char)((ulonglong)uVar3 % 0x40);
      uVar3 = FUN_00423ee0(0x5ba4a8);
      local_1f = local_1f - (char)((ulonglong)uVar3 % 0x40);
      uVar3 = FUN_00423ee0(0x5ba4a8);
      local_20 = local_20 - (char)((ulonglong)uVar3 % 0x40);
      puVar4 = (undefined4 *)FUN_004aadb0((int *)(local_44 + 0x14));
      local_40 = *puVar4;
      local_3c = puVar4[1];
      local_38 = puVar4[2];
      local_4c = FUN_00437520(0);
      FUN_0049db70(&local_54,10,&local_40,0);
    }
    local_50 = (undefined4 *)(local_44 + 0x1c);
    FUN_00423540(local_50);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

