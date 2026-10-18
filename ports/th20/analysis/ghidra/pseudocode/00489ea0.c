/* Entry: 0x00489ea0; symbol: FUN_00489ea0; body bytes: 251 */

void __thiscall FUN_00489ea0(void *this,uint param_1)

{
  undefined4 uVar1;
  int *piVar2;
  uint uVar3;
  char **ppcVar4;
  int iVar5;
  int *piVar6;
  
  uVar1 = FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  piVar2 = (int *)((int)this + 4);
  piVar6 = (int *)((int)this + 8);
  uVar3 = (*piVar6 - *piVar2) / 0x14;
  if (param_1 < uVar3) {
    ppcVar4 = (char **)(param_1 * 0x14 + *piVar2);
    FUN_00414b60();
    std::_Adl_verify_range<char*,char_const*>(ppcVar4,(char **)*piVar6);
    *piVar6 = (int)ppcVar4;
  }
  else if (uVar3 < param_1) {
    if ((uint)((*(int *)((int)this + 0xc) - *piVar2) / 0x14) < param_1) {
      FUN_0048a0a0(this,param_1);
    }
    else {
      iVar5 = FUN_0048a6c0(*piVar6,param_1 - uVar3,uVar1);
      *piVar6 = iVar5;
      FUN_00414b60();
    }
  }
  return;
}

