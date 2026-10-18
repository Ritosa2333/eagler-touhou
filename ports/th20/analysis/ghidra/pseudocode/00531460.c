/* Entry: 0x00531460; symbol: FUN_00531460; body bytes: 236 */

void FUN_00531460(void)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 local_54;
  void *local_50;
  undefined4 local_4c;
  void *local_48;
  int local_44;
  undefined4 local_40;
  undefined4 local_3c;
  undefined4 local_38;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar1 = FUN_00534260();
  FUN_00423520((void *)(local_44 + 0x14),iVar1);
  *(undefined1 *)(local_44 + 0x34) = 1;
  FUN_0044fcd0((undefined4 *)(local_44 + 0x38));
  FUN_0047ba30(&local_40);
  iVar1 = FUN_00460830(0);
  puVar2 = (undefined4 *)FUN_00460850(iVar1);
  local_40 = *puVar2;
  local_3c = puVar2[1];
  local_38 = puVar2[2];
  local_4c = FUN_00437520(0);
  puVar2 = FUN_0049db70(&local_54,8,&local_40,0);
  *(undefined4 *)(local_44 + 0x38) = *puVar2;
  local_50 = (void *)(local_44 + 0x38);
  FUN_004506b0(local_50,DAT_0056fb80,DAT_0056c8cc);
  local_48 = (void *)FUN_0044ced0((undefined4 *)(local_44 + 0x38));
  *(int *)((int)local_48 + 0x5c8) = local_44;
  FUN_00438a50(local_48,FUN_00531050);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

