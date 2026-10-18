/* Entry: 0x004f5a00; symbol: FUN_004f5a00; body bytes: 259 */

undefined4 * FUN_004f5a00(undefined4 *param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  undefined4 extraout_EDX;
  undefined4 local_38;
  undefined1 *local_34;
  undefined1 *local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  int local_20;
  int local_1c;
  int local_18;
  undefined4 local_14;
  undefined4 local_10;
  int *local_c;
  undefined1 local_5;
  
  puVar1 = (undefined4 *)move<>(&param_2);
  local_14 = *puVar1;
  local_18 = *local_c + 2;
  local_1c = *local_c;
  local_10 = local_14;
  FUN_004e73b0();
  param_2 = extraout_EDX;
  if ((*(char *)local_c[1] != '\0') && (*(int *)local_c[2] < *(int *)local_c[3])) {
    local_5 = 0x30;
    local_20 = *(int *)local_c[3] - *(int *)local_c[2];
    puVar1 = (undefined4 *)move<>(&param_2);
    local_24 = *puVar1;
    puVar1 = (undefined4 *)FUN_004e6f50(&local_38,local_24,local_20,&local_5);
    param_2 = *puVar1;
  }
  puVar1 = (undefined4 *)move<>(&param_2);
  local_2c = *puVar1;
  local_30 = *(undefined1 **)local_c[5];
  local_34 = (undefined1 *)local_c[4];
  local_28 = local_2c;
  FUN_004ef4d0(param_1,local_34,local_30,local_2c);
  return param_1;
}

