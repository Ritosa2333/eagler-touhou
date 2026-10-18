/* Entry: 0x0044e150; symbol: FUN_0044e150; body bytes: 504 */

undefined4 * FUN_0044e150(undefined4 *param_1)

{
  int iVar1;
  char **ppcVar2;
  int local_20;
  uint local_1c;
  int local_18;
  int local_14;
  int local_10;
  int *local_c;
  int local_8;
  
  std::_Adl_verify_range<char*,char_const*>((char **)"::postloadAnim : %d\n",(char **)*param_1);
  local_c = (int *)param_1[0xf];
  local_10 = 0;
  local_18 = 0;
  local_14 = 0;
  local_1c = 0;
  local_20 = 0;
  while( true ) {
    iVar1 = FUN_004292c0(param_1 + 0x17);
    if (local_10 == iVar1 + -1) {
      iVar1 = FUN_0044e350(param_1,local_1c,local_14,local_18,local_c);
      if (iVar1 < 0) {
        ppcVar2 = (char **)FUN_0044baf0((int)(param_1 + 1));
        std::_Adl_verify_range<char*,char_const*>((char **)"Error ::postload: %s\n",ppcVar2);
        FUN_004493c0(param_1 + 0x17,0);
        return (undefined4 *)0x0;
      }
      local_20 = local_20 + 1;
    }
    local_14 = (uint)*(ushort *)(local_c + 1) + local_14;
    local_18 = (uint)*(ushort *)((int)local_c + 6) + local_18;
    local_1c = local_1c + 1;
    if (local_c[9] == 0) break;
    local_c = (int *)((int)local_c + local_c[9]);
    local_10 = local_10 + 1;
    iVar1 = FUN_004292c0(param_1 + 0x17);
    if ((local_10 == iVar1) || (local_20 != 0)) {
      FUN_00449530(param_1 + 0x17);
      return param_1;
    }
  }
  for (local_8 = 0; local_8 < (int)param_1[0x12]; local_8 = local_8 + 1) {
    FUN_004299d0(local_8 * 0x5e4 + param_1[0x10]);
    FUN_00429e30((void *)(local_8 * 0x5e4 + param_1[0x10]));
    FUN_00438940(param_1,(void *)(local_8 * 0x5e4 + param_1[0x10]),local_8);
    FUN_00423520((void *)(param_1[0x10] + 0x4c8 + local_8 * 0x5e4),-1);
    FUN_00423520((void *)(param_1[0x10] + 0x4d8 + local_8 * 0x5e4),-1);
    FUN_0042b5d0((void *)(local_8 * 0x5e4 + param_1[0x10]));
  }
  FUN_004493c0(param_1 + 0x17,0);
  return param_1;
}

