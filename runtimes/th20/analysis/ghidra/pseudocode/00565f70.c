/* Entry: 0x00565f70; symbol: FUN_00565f70; body bytes: 84 */

bool __cdecl FUN_00565f70(uint *param_1)

{
  int iVar1;
  uint local_c [2];
  
  FUN_00559d30(*param_1);
  FUN_00559f80(param_1[1]);
  local_c[0] = 0;
  local_c[1] = 0;
  iVar1 = FUN_00565f50(local_c);
  if ((iVar1 == 0) && (*param_1 == local_c[0])) {
    return param_1[1] != local_c[1];
  }
  return true;
}

