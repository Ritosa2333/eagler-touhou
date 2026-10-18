/* Entry: 0x0046b7f0; symbol: FUN_0046b7f0; body bytes: 424 */

void __fastcall FUN_0046b7f0(int param_1)

{
  bool bVar1;
  undefined4 *puVar2;
  int iVar3;
  uint *puVar4;
  uint *puVar5;
  uint uVar6;
  undefined1 local_6c [8];
  undefined1 local_64 [4];
  undefined4 local_60;
  undefined4 local_5c;
  void *local_54;
  undefined4 local_50;
  undefined4 local_4c;
  COleCurrency *local_48;
  undefined4 local_44;
  int local_40;
  uint *local_3c;
  int local_38;
  int local_34;
  undefined1 local_2e;
  undefined1 local_2d;
  COleCurrency local_2c [12];
  int local_20 [2];
  undefined1 local_15;
  uint local_14;
  
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  local_38 = 0;
  local_40 = -1;
  local_34 = param_1;
  puVar2 = (undefined4 *)FUN_00467ff0(local_64,(void *)(param_1 + 0x11bc));
  local_50 = *puVar2;
  local_4c = local_50;
  iVar3 = FUN_00471050();
  if (iVar3 < *(int *)(local_34 + 0x1a1bc)) {
    local_44 = FUN_00471050();
  }
  else {
    local_44 = *(undefined4 *)(local_34 + 0x1a1bc);
  }
  puVar2 = FUN_0045f310(local_6c,local_50,local_44);
  local_60 = *puVar2;
  local_5c = puVar2[1];
  local_2d = DAT_0056f348;
  local_2e = DAT_0056f348;
  FUN_00467f50((char **)local_2c);
  local_48 = local_2c;
  FUN_0040c080(local_20,8);
  COleCurrency::operator_union_tagCY(local_48);
  FUN_00470540(&local_15);
  while( true ) {
    bVar1 = FUN_0046b1e0((int)local_20);
    if (bVar1) break;
    puVar4 = (uint *)FUN_0046b250(local_20);
    local_40 = local_40 + 1;
    puVar4[0x4d] = puVar4[0x4d] - 1;
    local_3c = puVar4;
    if (-1 < (int)puVar4[0x4d]) {
      if (local_38 != local_40) {
        local_54 = (void *)(local_34 + 0x11bc);
        uVar6 = 0x140;
        puVar5 = (uint *)FUN_0046b210(local_54,local_38);
        FUN_00543e20(puVar5,puVar4,uVar6);
      }
      local_38 = local_38 + 1;
    }
    FUN_0046b2b0(local_20);
  }
  *(int *)(local_34 + 0x1a1bc) = local_38;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffff0);
  return;
}

