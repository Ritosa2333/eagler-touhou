/* Entry: 0x00554430; symbol: FUN_00554430; body bytes: 74 */

int __cdecl FUN_00554430(uint param_1,uint param_2,int param_3)

{
  __acrt_ptd *p_Var1;
  SIZE_T SVar2;
  uint uVar3;
  
  if (param_1 == 0) {
    p_Var1 = FUN_005516c1();
    *(undefined4 *)p_Var1 = 0x16;
    FUN_005480bc();
    return -1;
  }
  SVar2 = __msize_base(*(LPCVOID *)((param_1 & 0xfffffffc) - 4));
  uVar3 = 4;
  if (4 < param_2) {
    uVar3 = param_2;
  }
  return SVar2 + ((-3 - uVar3) - (-param_3 & 3U));
}

