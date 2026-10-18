/* Entry: 0x004b0480; symbol: FUN_004b0480; body bytes: 195 */

void FUN_004b0480(void)

{
  undefined4 *puVar1;
  undefined1 auStack_58 [36];
  undefined4 uStack_34;
  undefined8 local_28;
  undefined1 local_20 [4];
  undefined1 *local_1c;
  int local_18;
  undefined4 local_14;
  undefined4 local_10;
  int local_c;
  int *local_8;
  
  uStack_34 = 0x4b049d;
  FUN_00416cf0(DAT_00570ab0);
  local_10 = FUN_0044baf0((int)(local_8 + 1));
  if ((*(uint *)(*local_8 + 0x104) >> 1 & 1) == 0) {
    local_c = 4;
  }
  else {
    local_c = 0xd;
  }
  local_14 = *(undefined4 *)(*local_8 + 0x118 + *(int *)(*local_8 + 0x114) * 4);
  local_18 = local_8[8];
  local_1c = auStack_58;
  puVar1 = FUN_004142a0(local_20,*local_8);
  FUN_004ad030(auStack_58,puVar1);
  FUN_0044ac20(DAT_005c0028,&local_28,local_18,local_14,0x20000000,local_c,0,0);
  return;
}

