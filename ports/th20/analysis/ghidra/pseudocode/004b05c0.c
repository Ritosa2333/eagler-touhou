/* Entry: 0x004b05c0; symbol: FUN_004b05c0; body bytes: 110 */

void __fastcall FUN_004b05c0(int *param_1)

{
  undefined1 auStack_4c [40];
  char *pcStack_24;
  undefined8 local_20;
  undefined1 *local_18;
  int local_14;
  undefined4 local_10;
  int *local_c;
  undefined1 local_5;
  
  local_10 = *(undefined4 *)(*param_1 + 0x118 + *(int *)(*param_1 + 0x114) * 4);
  local_14 = param_1[1];
  pcStack_24 = "                                                                     ";
  local_18 = auStack_4c;
  local_c = param_1;
  FUN_004ad110(auStack_4c,&local_5);
  FUN_0044ac20(DAT_005c0028,&local_20,local_14,local_10,0x20ffffff,0xb,0,0);
  return;
}

