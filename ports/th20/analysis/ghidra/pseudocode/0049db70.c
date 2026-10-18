/* Entry: 0x0049db70; symbol: FUN_0049db70; body bytes: 227 */

undefined4 * FUN_0049db70(undefined4 *param_1,int param_2,undefined4 param_3,int param_4)

{
  undefined4 *puVar1;
  undefined4 local_24;
  code *local_20;
  undefined4 local_1c;
  int local_18;
  int local_14;
  void *local_10;
  int local_c;
  short *local_8;
  
  local_8 = (short *)(&DAT_005afab8 + param_2 * 0x20);
  FUN_00425cc0(param_1);
  FUN_0049da50(param_1);
  if (-1 < local_8[1]) {
    if (param_4 == 0) {
      local_10 = (void *)(local_c + 0x10);
      puVar1 = (undefined4 *)FUN_00414580(local_10,(int)*local_8);
      local_1c = *puVar1;
      local_14 = *(int *)(local_c + 0x1303c);
      local_18 = (int)local_8[1];
      puVar1 = FUN_00450c70(&local_24,(uint *)0x0,local_18,local_14,(undefined4 *)0x0);
      *param_1 = *puVar1;
      param_4 = FUN_0044ced0(param_1);
      FUN_0044fa70(param_4);
    }
    else {
      FUN_004117a0(param_1,0);
    }
    if (*(int *)(local_8 + 2) != 0) {
      local_20 = *(code **)(local_8 + 2);
      (*local_20)(param_4,param_3,*(undefined4 *)(local_c + 0x1303c));
    }
  }
  return param_1;
}

