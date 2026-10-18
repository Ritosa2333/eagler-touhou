/* Entry: 0x0044c150; symbol: FUN_0044c150; body bytes: 139 */

undefined4 FUN_0044c150(int param_1,undefined4 param_2,undefined4 param_3)

{
  char **ppcVar1;
  int iVar2;
  undefined4 local_c;
  
  *(uint *)(param_1 + 0x14) = *(uint *)(param_1 + 0x14) & 0xfffffffc | 1;
  ppcVar1 = (char **)FUN_00412730(0x5c4d40);
  iVar2 = (**(code **)(*ppcVar1 + 0x5c))(ppcVar1,param_2,param_3,1,1,0x15,0,param_1,0);
  if (iVar2 != 0) {
    std::_Adl_verify_range<char*,char_const*>((char **)"error D3DXCreateTexture\n",ppcVar1);
  }
  if (DAT_005c4e2c == 0x15) {
    local_c = 4;
  }
  else {
    local_c = 2;
  }
  *(undefined4 *)(param_1 + 0xc) = local_c;
  return 0;
}

