/* Entry: 0x00496a00; symbol: FUN_00496a00; body bytes: 398 */

undefined4 __fastcall FUN_00496a00(void *param_1)

{
  uint uVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  float10 fVar4;
  undefined4 local_44;
  void *local_40;
  void *local_3c;
  undefined4 local_38;
  void *local_34;
  undefined4 local_30;
  int local_2c;
  int local_28;
  void *local_24;
  uint local_20;
  int local_1c;
  void *local_18;
  void *local_14;
  int local_10;
  int local_c;
  void *local_8;
  
  local_8 = param_1;
  local_c = FUN_004ab080(param_1,0.0);
  local_1c = FUN_004ab080(local_8,1.4013e-45);
  uVar1 = FUN_0049cb30((int)local_8 + 0xc);
  if (uVar1 < local_c + 1U) {
    local_14 = (void *)((int)local_8 + 0xc);
    FUN_0049c2a0(local_14,local_c + 1);
  }
  local_18 = (void *)((int)local_8 + 0xc);
  puVar2 = (undefined4 *)FUN_0048bce0(local_18,local_c);
  FUN_0044fcd0(puVar2);
  if (-1 < local_1c) {
    local_24 = (void *)FUN_00412730(*(int *)((int)local_8 + 0x2ec));
    local_20 = *(uint *)((int)local_8 + 0x1c);
    local_30 = FUN_004aae10(local_24,local_20);
    local_28 = *(int *)((int)local_8 + 0x34) + 7;
    local_2c = FUN_004ab080(local_8,1.4013e-45);
    puVar2 = FUN_0049cbf0(&local_44,(uint *)0x0,local_2c,local_28,(undefined4 *)0x0);
    local_38 = *puVar2;
    local_34 = (void *)((int)local_8 + 0xc);
    puVar2 = (undefined4 *)FUN_0048bce0(local_34,local_c);
    *puVar2 = local_38;
    if (local_c == 0) {
      uVar3 = FUN_004ab080(local_8,1.4013e-45);
      *(undefined4 *)((int)local_8 + 0x24) = uVar3;
      *(undefined4 *)((int)local_8 + 0x20) = *(undefined4 *)((int)local_8 + 0x1c);
    }
    local_3c = (void *)((int)local_8 + 0xc);
    puVar2 = (undefined4 *)FUN_0048bce0(local_3c,local_c);
    local_10 = FUN_0044ced0(puVar2);
    if (local_c == 0) {
      fVar4 = FUN_004459a0(local_10);
      *(float *)((int)local_8 + 0x170) = (float)fVar4;
      fVar4 = FUN_004459d0(local_10);
      *(float *)((int)local_8 + 0x174) = (float)fVar4;
    }
    if ((*(uint *)((int)local_8 + 0x2c8) >> 5 & 1) != 0) {
      local_40 = (void *)((int)local_8 + 0xc);
      FUN_0048bce0(local_40,local_c);
      FUN_00450120();
    }
  }
  return 0;
}

