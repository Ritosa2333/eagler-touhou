/* Entry: 0x0046b390; symbol: FUN_0046b390; body bytes: 334 */

void FUN_0046b390(void)

{
  char cVar1;
  undefined4 uVar2;
  int *unaff_FS_OFFSET;
  undefined1 auStack_78 [36];
  undefined4 uStack_54;
  undefined8 local_48;
  undefined1 *local_40;
  int local_3c;
  undefined4 local_38;
  undefined4 local_34;
  int local_30;
  int local_2c;
  int local_28;
  undefined4 local_24;
  undefined1 local_1d;
  int *local_1c;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00567a8d;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uStack_54 = 0x46b3c7;
  uVar2 = FUN_0040c310(&DAT_005c0240,0x12);
  uStack_54 = 0x46b3d0;
  FUN_0040b730(&local_18,uVar2);
  local_8 = 0;
  cVar1 = FUN_0046b230(*local_1c + 0x680);
  if (cVar1 == '\0') {
    local_24 = FUN_0044baf0((int)(local_1c + 1));
    local_28 = *local_1c + 0x678;
    local_2c = *(int *)(*local_1c + 0x630);
    local_30 = *(int *)(*local_1c + 0x62c);
    local_34 = *(undefined4 *)(*local_1c + 0x654);
    local_38 = *(undefined4 *)(*local_1c + 0x650);
    local_3c = *local_1c + 0x14;
    local_40 = auStack_78;
    FUN_00467e20(auStack_78,&local_1d);
    FUN_0044ac20(DAT_005c0028,&local_48,local_3c,local_38,local_34,local_30,0,local_2c);
    local_8 = 0xffffffff;
    FUN_0040b900(&local_18);
  }
  else {
    uStack_54 = 0x46b3ff;
    FUN_0046a100((void *)*local_1c);
    local_8 = 0xffffffff;
    FUN_0040b900(&local_18);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

