/* Entry: 0x004afff0; symbol: FUN_004afff0; body bytes: 182 */

void __fastcall FUN_004afff0(int *param_1)

{
  undefined4 *puVar1;
  undefined1 auStack_58 [36];
  undefined4 uStack_34;
  undefined8 local_28;
  undefined1 local_20 [4];
  undefined1 *local_1c;
  int local_18;
  undefined4 local_14;
  undefined *local_10;
  int local_c;
  int *local_8;
  
  uStack_34 = 0x4b000e;
  local_8 = param_1;
  local_10 = FUN_004b7c90((byte *)(*(int *)(*param_1 + 0xcc) + 4));
  if ((*(uint *)(*local_8 + 0x104) >> 1 & 1) == 0) {
    local_c = 4;
  }
  else {
    local_c = 0xd;
  }
  local_14 = *(undefined4 *)(*local_8 + 0x118 + *(int *)(*local_8 + 0x114) * 4);
  local_18 = local_8[1];
  local_1c = auStack_58;
  puVar1 = FUN_004142a0(local_20,*local_8);
  FUN_004acbd0(auStack_58,puVar1);
  FUN_0044ac20(DAT_005c0028,&local_28,local_18,local_14,0,local_c,0,0);
  return;
}

