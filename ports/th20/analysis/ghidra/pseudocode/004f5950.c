/* Entry: 0x004f5950; symbol: FUN_004f5950; body bytes: 167 */

void __fastcall FUN_004f5950(int *param_1)

{
  undefined4 *puVar1;
  undefined1 local_38 [8];
  undefined4 local_30;
  undefined4 local_2c;
  undefined4 local_28;
  undefined4 local_24;
  int local_20;
  int local_1c;
  int *local_18;
  undefined1 local_11;
  
  local_18 = param_1;
  if (*param_1 < 0x9c) {
    puVar1 = FUN_00446da0(local_38,param_1 + 4);
    local_30 = *puVar1;
    local_2c = puVar1[1];
    local_1c = local_18[2];
    local_20 = local_18[1];
    local_28 = local_30;
    local_24 = local_2c;
    FUN_004f33d0();
  }
  else {
    local_11 = 0;
    FUN_004f37d0(param_1[1],param_1[2],(uint *)(param_1 + 4));
  }
  return;
}

