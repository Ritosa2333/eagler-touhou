/* Entry: 0x004b90e0; symbol: FUN_004b90e0; body bytes: 1522 */

void __thiscall FUN_004b90e0(void *this,undefined4 param_1,int param_2)

{
  undefined4 *puVar1;
  undefined4 local_100;
  undefined4 local_fc;
  undefined4 local_f8;
  undefined4 local_f4;
  undefined4 local_f0;
  undefined4 local_ec;
  undefined4 local_e8;
  undefined4 local_e4;
  undefined4 local_e0;
  undefined4 local_dc;
  undefined4 local_d8;
  undefined4 local_d4;
  void *local_d0;
  undefined4 local_cc;
  void *local_c8;
  undefined4 local_c4;
  void *local_c0;
  undefined4 local_bc;
  void *local_b8;
  undefined4 local_b4;
  void *local_b0;
  undefined4 local_ac;
  void *local_a8;
  undefined4 local_a4;
  void *local_a0;
  undefined4 local_9c;
  void *local_98;
  undefined4 local_94;
  undefined4 local_90;
  void *local_8c;
  undefined4 local_88;
  void *local_84;
  undefined4 local_80;
  int local_7c;
  void *local_78;
  undefined4 local_74;
  void *local_70;
  undefined4 local_6c;
  void *local_68;
  int local_64;
  void *local_60;
  undefined4 local_5c;
  void *local_58;
  undefined4 local_54;
  void *local_50;
  void *local_4c;
  void *local_48;
  int local_44;
  void *local_40;
  undefined4 local_3c;
  void *local_38;
  undefined4 local_34;
  void *local_30;
  undefined4 local_2c;
  void *local_28;
  undefined4 local_24;
  void *local_20;
  int local_1c;
  undefined4 local_18;
  int local_14;
  int local_10;
  int local_c;
  void *local_8;
  
  local_18 = param_1;
  local_8 = this;
  switch(param_1) {
  case 0:
    local_20 = (void *)((int)this + 0xbc);
    puVar1 = (undefined4 *)FUN_00414580(local_20,0);
    FUN_0044fcd0(puVar1);
    local_24 = *(undefined4 *)((int)local_8 + 0x2cc);
    puVar1 = FUN_00450c70(&local_d8,(uint *)"front",0x31,-1,(undefined4 *)0x0);
    local_2c = *puVar1;
    local_28 = (void *)((int)local_8 + 0xbc);
    puVar1 = (undefined4 *)FUN_00414580(local_28,0);
    *puVar1 = local_2c;
    local_14 = param_2;
    local_10 = 10000000;
    local_1c = 0;
    for (local_c = 0; local_c < 8; local_c = local_c + 1) {
      local_30 = (void *)((int)local_8 + 0x94);
      puVar1 = (undefined4 *)FUN_00414580(local_30,local_c);
      FUN_0044fcd0(puVar1);
      local_34 = FUN_00437950(DAT_005c0698);
      puVar1 = FUN_00450c70(&local_dc,(uint *)0x0,local_c + 4,-1,(undefined4 *)0x0);
      local_3c = *puVar1;
      local_38 = (void *)((int)local_8 + 0x94);
      puVar1 = (undefined4 *)FUN_00414580(local_38,local_c);
      *puVar1 = local_3c;
      if (local_14 / local_10 != 0) {
        local_1c = 1;
      }
      local_40 = (void *)((int)local_8 + 0x94);
      local_44 = FUN_00414580(local_40,local_c);
      FUN_0044bd70(local_14 / local_10 + 0xef);
      if (local_1c == 0) {
        local_48 = (void *)((int)local_8 + 0x94);
        FUN_00414580(local_48,local_c);
        FUN_00450120();
      }
      else {
        local_4c = (void *)((int)local_8 + 0x94);
        FUN_00414580(local_4c,local_c);
        FUN_0044fa30();
      }
      local_14 = local_14 % local_10;
      local_10 = local_10 / 10;
    }
    local_50 = (void *)((int)local_8 + 0x94);
    puVar1 = (undefined4 *)FUN_00414580(local_50,8);
    FUN_0044fcd0(puVar1);
    if (999999 < param_2) {
      local_54 = FUN_00437950(DAT_005c0698);
      puVar1 = FUN_00450c70(&local_e0,(uint *)0x0,0xc,-1,(undefined4 *)0x0);
      local_5c = *puVar1;
      local_58 = (void *)((int)local_8 + 0x94);
      puVar1 = (undefined4 *)FUN_00414580(local_58,8);
      *puVar1 = local_5c;
      local_60 = (void *)((int)local_8 + 0x94);
      local_64 = FUN_00414580(local_60,8);
      FUN_0044bd70(0xfd);
    }
    local_68 = (void *)((int)local_8 + 0x94);
    puVar1 = (undefined4 *)FUN_00414580(local_68,9);
    FUN_0044fcd0(puVar1);
    if (999 < param_2) {
      local_6c = FUN_00437950(DAT_005c0698);
      puVar1 = FUN_00450c70(&local_e4,(uint *)0x0,0xd,-1,(undefined4 *)0x0);
      local_74 = *puVar1;
      local_70 = (void *)((int)local_8 + 0x94);
      puVar1 = (undefined4 *)FUN_00414580(local_70,9);
      *puVar1 = local_74;
      local_78 = (void *)((int)local_8 + 0x94);
      local_7c = FUN_00414580(local_78,9);
      FUN_0044bd70(0xfd);
    }
    *(undefined4 *)((int)local_8 + 0x138) = 1;
    local_80 = *(undefined4 *)((int)local_8 + 0x2cc);
    puVar1 = FUN_00450c70(&local_e8,(uint *)"front",0x54,-1,(undefined4 *)0x0);
    *(undefined4 *)((int)local_8 + 0x114) = *puVar1;
    break;
  case 1:
    local_84 = (void *)((int)this + 0xbc);
    puVar1 = (undefined4 *)FUN_00414580(local_84,0);
    FUN_0044fcd0(puVar1);
    local_88 = *(undefined4 *)((int)local_8 + 0x2cc);
    puVar1 = FUN_00450c70(&local_ec,(uint *)"front",0x32,-1,(undefined4 *)0x0);
    local_90 = *puVar1;
    local_8c = (void *)((int)local_8 + 0xbc);
    puVar1 = (undefined4 *)FUN_00414580(local_8c,0);
    *puVar1 = local_90;
    *(undefined4 *)((int)local_8 + 0x138) = 1;
    local_94 = *(undefined4 *)((int)local_8 + 0x2cc);
    puVar1 = FUN_00450c70(&local_f0,(uint *)"front",0x54,-1,(undefined4 *)0x0);
    *(undefined4 *)((int)local_8 + 0x114) = *puVar1;
    break;
  case 2:
    local_98 = (void *)((int)this + 0xbc);
    puVar1 = (undefined4 *)FUN_00414580(local_98,1);
    FUN_0044fcd0(puVar1);
    local_9c = *(undefined4 *)((int)local_8 + 0x2cc);
    puVar1 = FUN_00450c70(&local_f4,(uint *)"front",0x33,-1,(undefined4 *)0x0);
    local_a4 = *puVar1;
    local_a0 = (void *)((int)local_8 + 0xbc);
    puVar1 = (undefined4 *)FUN_00414580(local_a0,1);
    *puVar1 = local_a4;
    break;
  case 3:
    local_a8 = (void *)((int)this + 0xbc);
    puVar1 = (undefined4 *)FUN_00414580(local_a8,1);
    FUN_0044fcd0(puVar1);
    local_ac = *(undefined4 *)((int)local_8 + 0x2cc);
    puVar1 = FUN_00450c70(&local_f8,(uint *)"front",0x34,-1,(undefined4 *)0x0);
    local_b4 = *puVar1;
    local_b0 = (void *)((int)local_8 + 0xbc);
    puVar1 = (undefined4 *)FUN_00414580(local_b0,1);
    *puVar1 = local_b4;
    break;
  case 4:
    local_b8 = (void *)((int)this + 0xbc);
    puVar1 = (undefined4 *)FUN_00414580(local_b8,1);
    FUN_0044fcd0(puVar1);
    local_bc = *(undefined4 *)((int)local_8 + 0x2cc);
    puVar1 = FUN_00450c70(&local_fc,(uint *)"front",0x35,-1,(undefined4 *)0x0);
    local_c4 = *puVar1;
    local_c0 = (void *)((int)local_8 + 0xbc);
    puVar1 = (undefined4 *)FUN_00414580(local_c0,1);
    *puVar1 = local_c4;
    break;
  case 6:
    local_c8 = (void *)((int)this + 0xbc);
    puVar1 = (undefined4 *)FUN_00414580(local_c8,0);
    FUN_0044fcd0(puVar1);
    local_cc = *(undefined4 *)((int)local_8 + 0x2cc);
    puVar1 = FUN_00450c70(&local_100,(uint *)"front",0x36,-1,(undefined4 *)0x0);
    local_d4 = *puVar1;
    local_d0 = (void *)((int)local_8 + 0xbc);
    puVar1 = (undefined4 *)FUN_00414580(local_d0,0);
    *puVar1 = local_d4;
  }
  return;
}

