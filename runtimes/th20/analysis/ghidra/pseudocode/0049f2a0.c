/* Entry: 0x0049f2a0; symbol: FUN_0049f2a0; body bytes: 140 */

void __fastcall FUN_0049f2a0(int *param_1)

{
  undefined4 *puVar1;
  undefined1 auStack_58 [40];
  undefined4 uStack_30;
  undefined8 local_28;
  undefined1 local_20 [4];
  undefined1 *local_1c;
  int local_18;
  undefined4 local_14;
  undefined4 local_10;
  undefined4 local_c;
  int *local_8;
  
  uStack_30 = 0x49f2b5;
  local_8 = param_1;
  uStack_30 = FUN_0044baf0((int)(param_1 + 1));
  local_10 = *(undefined4 *)(*local_8 + 0x88);
  local_14 = *(undefined4 *)(*local_8 + 0x84);
  local_18 = local_8[8];
  local_1c = auStack_58;
  local_c = uStack_30;
  puVar1 = FUN_0040b670(local_20,local_8 + 8);
  FUN_0049e2b0(auStack_58,puVar1);
  FUN_0044ac20(DAT_005c0028,&local_28,local_18,local_14,local_10,4,0,0);
  return;
}

