/* Entry: 0x005388c0; symbol: FUN_005388c0; body bytes: 180 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_005388c0(void)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 local_4c;
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
  FUN_0047ba30(&local_40);
  iVar1 = FUN_00460830(0);
  puVar2 = (undefined4 *)FUN_00460850(iVar1);
  local_40 = *puVar2;
  local_3c = puVar2[1];
  local_38 = puVar2[2];
  FUN_0044fcd0((undefined4 *)&DAT_005c69dc);
  local_48 = FUN_00437520(0);
  puVar2 = FUN_0049db70(&local_4c,7,&local_40,0);
  _DAT_005c69dc = *puVar2;
  FUN_00450850(&DAT_005c69dc,local_44);
  FUN_0044f100(&DAT_005c69dc,FUN_00534e30);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

