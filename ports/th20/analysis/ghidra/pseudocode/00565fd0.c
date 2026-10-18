/* Entry: 0x00565fd0; symbol: FUN_00565fd0; body bytes: 94 */

undefined4 __cdecl FUN_00565fd0(uint *param_1)

{
  bool bVar1;
  int iVar2;
  undefined3 extraout_var;
  uint local_c;
  uint local_8;
  
  local_c = 0;
  local_8 = 0;
  iVar2 = FUN_00565f50(&local_c);
  if (iVar2 == 0) {
    *param_1 = local_c;
    local_c = local_c | 0x1f;
    param_1[1] = local_8;
    bVar1 = FUN_00565f70(&local_c);
    if (CONCAT31(extraout_var,bVar1) == 0) {
      FUN_00561f90();
      return 0;
    }
  }
  return 1;
}

