/* Entry: 0x00530a00; symbol: FUN_00530a00; body bytes: 189 */

void FUN_00530a00(void)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 local_20;
  undefined4 local_1c;
  int local_18;
  float local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar1 = FUN_00534260();
  FUN_00423520((void *)(local_18 + 0x14),iVar1);
  *(undefined1 *)(local_18 + 0x34) = 1;
  FUN_0044fcd0((undefined4 *)(local_18 + 0x38));
  *(undefined4 *)(local_18 + 0x3c) = DAT_0056d7c0;
  FUN_00422dd0(local_14,0,*(undefined4 *)(local_18 + 0x3c),0);
  iVar1 = FUN_00460830(0);
  local_1c = FUN_00421810(iVar1);
  puVar2 = FUN_004790e0(&local_20,(uint *)&DAT_0056fb8c,0x19,local_14,0,-1,(undefined4 *)0x0);
  *(undefined4 *)(local_18 + 0x38) = *puVar2;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

