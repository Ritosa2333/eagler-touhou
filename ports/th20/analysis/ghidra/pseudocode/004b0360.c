/* Entry: 0x004b0360; symbol: FUN_004b0360; body bytes: 284 */

void FUN_004b0360(void)

{
  undefined4 *puVar1;
  undefined4 uVar2;
  int *unaff_FS_OFFSET;
  undefined1 auStack_74 [36];
  undefined4 uStack_50;
  undefined8 local_40;
  undefined1 local_38 [4];
  undefined1 *local_34;
  void *local_30;
  int local_2c;
  int local_28;
  int local_24;
  undefined4 local_20;
  int *local_1c;
  undefined4 local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005698fd;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  uStack_50 = 0x4b039f;
  FUN_00416cf0(DAT_00570ab0);
  local_20 = FUN_0044baf0((int)(local_1c + 1));
  local_24 = local_1c[9];
  local_28 = local_1c[8];
  local_2c = local_1c[10];
  local_34 = auStack_74;
  puVar1 = FUN_004142a0(local_38,*local_1c);
  FUN_004acfc0(auStack_74,puVar1);
  FUN_0044ac20(DAT_005c0028,&local_40,local_2c,0,0x20ffffff,0x12,local_28,local_24);
  uStack_50 = 0x4b0423;
  uVar2 = FUN_0040c310(&DAT_005c0240,9);
  uStack_50 = 0x4b042c;
  FUN_0040b730(&local_18,uVar2);
  local_8 = 0;
  local_30 = (void *)(*local_1c + 100);
  uStack_50 = 0x4b0452;
  FUN_004502c0(local_30,(undefined4 *)(*local_1c + 0x128));
  local_8 = 0xffffffff;
  FUN_0040b900(&local_18);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

