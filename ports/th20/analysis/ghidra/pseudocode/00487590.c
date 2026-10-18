/* Entry: 0x00487590; symbol: FUN_00487590; body bytes: 797 */

void __fastcall FUN_00487590(int param_1)

{
  bool bVar1;
  int iVar2;
  uint local_40;
  undefined4 local_2c [3];
  undefined4 local_20 [3];
  undefined4 local_14 [3];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (((*(uint *)(param_1 + 0x78) & 1) != 0) &&
     (iVar2 = FUN_0044ced0((undefined4 *)(param_1 + 0x1c)), iVar2 != 0)) {
    FUN_00488b00(DAT_005c0698,2);
    FUN_00488a00(DAT_005c0698,2);
    iVar2 = FUN_0045d0e0(iVar2);
    FUN_00488940(DAT_005c0698,*(undefined1 *)(iVar2 + 3));
    if ((*(uint *)(param_1 + 0x78) >> 1 & 1) == 0) {
      FUN_00422dd0(local_2c,DAT_0056fe84,DAT_0056fe78,0);
      FUN_0040ddd0(&stack0xffffff84,"$");
      FUN_0046ce90(DAT_005c0698,local_2c);
    }
    else {
      FUN_00422dd0(local_20,DAT_0056fe80,DAT_0056fe78,0);
      FUN_0046cd40(DAT_005c0698,local_20,*(uint *)(param_1 + 0x7c),8,' ');
    }
    FUN_00422dd0(local_14,DAT_0056fe88,DAT_0056fe78,0);
    bVar1 = FUN_00488850(0x5ba568);
    local_40 = (uint)bVar1;
    iVar2 = FUN_0050fc50(DAT_005c6108);
    iVar2 = FUN_00486e10((void *)(iVar2 + 0xb08),*(int *)(param_1 + 0x74));
    if (*(int *)(iVar2 + 0xc0 + local_40 * 4) < 100) {
      iVar2 = FUN_0050fc50(DAT_005c6108);
      iVar2 = FUN_00486e10((void *)(iVar2 + 0xb08),*(int *)(param_1 + 0x74));
      if (*(int *)(iVar2 + 200 + local_40 * 4) < 100) {
        iVar2 = FUN_0050fc50(DAT_005c6108);
        FUN_00486e10((void *)(iVar2 + 0xb08),*(int *)(param_1 + 0x74));
        iVar2 = FUN_0050fc50(DAT_005c6108);
        FUN_00486e10((void *)(iVar2 + 0xb08),*(int *)(param_1 + 0x74));
        FUN_0046c990(DAT_005c0698,local_14,(wchar_t *)"%.2d/%.2d");
      }
      else {
        iVar2 = FUN_0050fc50(DAT_005c6108);
        FUN_00486e10((void *)(iVar2 + 0xb08),*(int *)(param_1 + 0x74));
        FUN_0046c990(DAT_005c0698,local_14,(wchar_t *)"%.2d/99+");
      }
    }
    else {
      FUN_0040ddd0(&stack0xffffff84,"MASTER");
      FUN_0046ce90(DAT_005c0698,local_14);
    }
    FUN_00488b00(DAT_005c0698,0);
    FUN_00488a00(DAT_005c0698,0);
    FUN_00488940(DAT_005c0698,0xff);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

