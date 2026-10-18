/* Entry: 0x0049f200; symbol: FUN_0049f200; body bytes: 149 */

void __fastcall FUN_0049f200(int *param_1)

{
  undefined4 *puVar1;
  undefined1 auStack_5c [40];
  undefined4 uStack_34;
  undefined8 local_2c;
  undefined1 local_24 [4];
  undefined1 *local_20;
  int local_1c;
  undefined4 local_18;
  int local_14;
  int local_10;
  undefined4 local_c;
  int *local_8;
  
  uStack_34 = 0x49f215;
  local_8 = param_1;
  uStack_34 = FUN_0044baf0((int)(param_1 + 1));
  local_10 = local_8[9];
  local_14 = local_8[8];
  local_18 = *(undefined4 *)(*local_8 + 0x88);
  local_1c = local_8[10];
  local_20 = auStack_5c;
  local_c = uStack_34;
  puVar1 = FUN_0040b670(local_24,local_8 + 10);
  FUN_0049e240(auStack_5c,puVar1);
  FUN_0044ac20(DAT_005c0028,&local_2c,local_1c,local_18,0x20ffffff,0x12,local_14,local_10);
  return;
}

