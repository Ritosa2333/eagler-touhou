/* Entry: 0x00531a50; symbol: FUN_00531a50; body bytes: 351 */

void FUN_00531a50(void)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 local_64;
  void *local_60;
  void *local_5c;
  void *local_58;
  undefined4 local_54;
  undefined4 local_50;
  void *local_4c;
  undefined4 local_48;
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
  local_48 = FUN_00437520(0);
  puVar2 = FUN_0049db70(&local_64,9,&local_40,0);
  *(undefined4 *)(local_44 + 0x38) = *puVar2;
  *(undefined4 *)(local_44 + 0x68) = DAT_0056cd90;
  local_4c = (void *)(local_44 + 0x38);
  FUN_004506b0(local_4c,*(undefined4 *)(local_44 + 0x68),DAT_0056c8cc);
  iVar1 = FUN_00460830(0);
  puVar2 = (undefined4 *)FUN_00460850(iVar1);
  *(undefined4 *)(local_44 + 0x5c) = *puVar2;
  *(undefined4 *)(local_44 + 0x60) = puVar2[1];
  *(undefined4 *)(local_44 + 100) = puVar2[2];
  local_58 = (void *)(local_44 + 0x6c);
  local_50 = DAT_0056fe7c;
  local_54 = DAT_0056cd90;
  FUN_00439550(local_58,0x14,4,&local_54,&local_50);
  local_5c = (void *)(local_44 + 0x38);
  FUN_00450850(local_5c,local_44);
  local_60 = (void *)(local_44 + 0x38);
  FUN_0044f100(local_60,FUN_005315a0);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

