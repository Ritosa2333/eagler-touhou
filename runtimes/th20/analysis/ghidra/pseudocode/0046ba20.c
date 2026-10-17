/* Entry: 0x0046ba20; symbol: FUN_0046ba20; body bytes: 365 */

void __thiscall FUN_0046ba20(void *this,int param_1)

{
  bool bVar1;
  undefined4 *puVar2;
  int iVar3;
  undefined1 local_5c [8];
  undefined1 local_54 [4];
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined4 local_44;
  byte *local_40;
  COleCurrency *local_3c;
  undefined4 local_38;
  void *local_34;
  undefined1 local_2e;
  undefined1 local_2d;
  COleCurrency local_2c [12];
  int local_20 [2];
  undefined1 local_15;
  uint local_14;
  
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  local_34 = this;
  puVar2 = (undefined4 *)FUN_00467ff0(local_54,(void *)((int)this + 0x11bc));
  local_48 = *puVar2;
  local_44 = local_48;
  iVar3 = FUN_00471050();
  if (iVar3 < *(int *)((int)local_34 + 0x1a1bc)) {
    local_38 = FUN_00471050();
  }
  else {
    local_38 = *(undefined4 *)((int)local_34 + 0x1a1bc);
  }
  puVar2 = FUN_0045f310(local_5c,local_48,local_38);
  local_50 = *puVar2;
  local_4c = puVar2[1];
  local_2d = DAT_0056f348;
  local_2e = DAT_0056f348;
  FUN_00467f50((char **)local_2c);
  local_3c = local_2c;
  FUN_0040c080(local_20,8);
  COleCurrency::operator_union_tagCY(local_3c);
  FUN_00470540(&local_15);
  while (bVar1 = FUN_0046b1e0((int)local_20), !bVar1) {
    local_40 = (byte *)FUN_0046b250(local_20);
    if (*(int *)(local_40 + 0x130) == param_1) {
      FUN_0046d300(local_34,local_40);
    }
    FUN_0046b2b0(local_20);
  }
  FUN_0046bb90(local_34,param_1);
  FUN_004455c0(DAT_005c0028);
  FUN_0041dce0(&DAT_005c4d40,2);
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffff0);
  return;
}

