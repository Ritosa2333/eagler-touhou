/* Entry: 0x0045fda0; symbol: FUN_0045fda0; body bytes: 93 */

undefined4 * FUN_0045fda0(undefined4 *param_1)

{
  int *piVar1;
  undefined1 local_18 [4];
  void *local_14;
  int local_10;
  int local_c;
  void *local_8;
  
  local_c = FUN_0045ef50();
  piVar1 = std::_Min_value<int>(&local_c,(int *)((int)local_8 + 4));
  local_10 = *piVar1;
  local_14 = FUN_0045eef0(local_18,local_8);
  FUN_0045f4c0(local_14,param_1,local_10);
  return param_1;
}

