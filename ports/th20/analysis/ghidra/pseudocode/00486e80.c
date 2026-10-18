/* Entry: 0x00486e80; symbol: FUN_00486e80; body bytes: 111 */

void __fastcall FUN_00486e80(undefined4 *param_1)

{
  undefined4 *puVar1;
  undefined1 auStack_50 [40];
  undefined4 uStack_28;
  undefined8 local_20;
  undefined1 local_18 [4];
  undefined1 *local_14;
  int local_10;
  undefined4 local_c;
  undefined4 *local_8;
  
  uStack_28 = 0x486e95;
  local_8 = param_1;
  uStack_28 = FUN_0044baf0((int)(param_1 + 1));
  local_10 = local_8[8];
  local_14 = auStack_50;
  local_c = uStack_28;
  puVar1 = FUN_004142a0(local_18,*local_8);
  FUN_00486740(auStack_50,puVar1);
  FUN_0044b020(DAT_005c0028,&local_20,local_10,0xffffff,0xff000000,4,0);
  return;
}

