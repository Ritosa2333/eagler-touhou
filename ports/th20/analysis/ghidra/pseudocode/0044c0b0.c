/* Entry: 0x0044c0b0; symbol: FUN_0044c0b0; body bytes: 155 */

int FUN_0044c0b0(int param_1,int param_2,int param_3,int param_4)

{
  char **ppcVar1;
  int iVar2;
  
  *(uint *)(param_1 + 0x14) = *(uint *)(param_1 + 0x14) & 0xfffffffc | 2;
  ppcVar1 = (char **)FUN_00412730(0x5c4d40);
  iVar2 = (**(code **)(*ppcVar1 + 0x5c))
                    (ppcVar1,param_2,param_3,1,0x200,*(undefined4 *)(&DAT_0056e208 + param_4 * 4),0,
                     param_1,0);
  if (iVar2 != 0) {
    std::_Adl_verify_range<char*,char_const*>((char **)"error D3DXCreateTexture\n",ppcVar1);
  }
  *(undefined4 *)(param_1 + 0xc) = *(undefined4 *)(&DAT_0056e22c + param_4 * 4);
  return param_2 * param_3 * *(int *)(&DAT_0056e22c + param_4 * 4);
}

