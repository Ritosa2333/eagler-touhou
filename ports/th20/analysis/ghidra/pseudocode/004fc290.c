/* Entry: 0x004fc290; symbol: FUN_004fc290; body bytes: 398 */

void __thiscall FUN_004fc290(void *this,_Locinfo *param_1,char param_2)

{
  undefined1 uVar1;
  lconv *plVar2;
  char *pcVar3;
  int *unaff_FS_OFFSET;
  char *local_50;
  void *local_44;
  uint local_40 [11];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a59d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  plVar2 = std::_Locinfo::_Getlconv(param_1);
  FUN_004fc130(local_40);
  *(undefined4 *)((int)this + 8) = 0;
  *(undefined4 *)((int)this + 0x10) = 0;
  *(undefined4 *)((int)this + 0x14) = 0;
  local_8 = 0;
  if (param_2 == '\0') {
    local_50 = plVar2->grouping;
  }
  else {
    local_50 = "";
  }
  local_44 = this;
  pcVar3 = FUN_004ed5b0(local_50);
  *(char **)((int)this + 8) = pcVar3;
  pcVar3 = FUN_004fc1e0();
  pcVar3 = FUN_004ed5b0(pcVar3);
  *(char **)((int)this + 0x10) = pcVar3;
  pcVar3 = FUN_004fc200();
  pcVar3 = FUN_004ed5b0(pcVar3);
  *(char **)((int)this + 0x14) = pcVar3;
  local_44 = (void *)0x0;
  if (param_2 == '\0') {
    uVar1 = FUN_004ed5a0(*plVar2->decimal_point);
    *(undefined1 *)((int)this + 0xc) = uVar1;
    uVar1 = FUN_004ed5a0(*plVar2->thousands_sep);
    *(undefined1 *)((int)this + 0xd) = uVar1;
  }
  else {
    uVar1 = FUN_004ed5a0(0x2e);
    *(undefined1 *)((int)this + 0xc) = uVar1;
    uVar1 = FUN_004ed5a0(0x2c);
    *(undefined1 *)((int)this + 0xd) = uVar1;
  }
  local_8 = 0xffffffff;
  ~_Tidy_guard<>((int *)&local_44);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

