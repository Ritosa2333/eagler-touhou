/* Entry: 0x0049dcf0; symbol: FUN_0049dcf0; body bytes: 202 */

undefined4 * FUN_0049dcf0(undefined4 *param_1,int param_2,undefined4 param_3,int param_4)

{
  undefined4 *puVar1;
  undefined4 local_20;
  code *local_1c;
  undefined4 local_18;
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
      local_18 = *puVar1;
      local_14 = (int)local_8[1];
      puVar1 = FUN_0049e080(&local_20,(uint *)0x0,local_14,(undefined4 *)0x0);
      *param_1 = *puVar1;
      param_4 = FUN_0044ced0(param_1);
    }
    else {
      FUN_004117a0(param_1,0);
    }
    if (*(int *)(local_8 + 2) != 0) {
      local_1c = *(code **)(local_8 + 2);
      (*local_1c)(param_4,param_3,*(undefined4 *)(local_c + 0x1303c));
    }
  }
  return param_1;
}

