/* Entry: 0x0053aae0; symbol: FUN_0053aae0; body bytes: 243 */

void __thiscall FUN_0053aae0(void *this,uint param_1)

{
  char **ppcVar1;
  undefined4 uVar2;
  int *piVar3;
  int iVar4;
  int *piVar5;
  uint uVar6;
  
  uVar2 = FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  piVar3 = (int *)((int)this + 4);
  piVar5 = (int *)((int)this + 8);
  uVar6 = *piVar5 - *piVar3 >> 2;
  if (param_1 < uVar6) {
    ppcVar1 = (char **)(*piVar3 + param_1 * 4);
    FUN_00414b60();
    std::_Adl_verify_range<char*,char_const*>(ppcVar1,(char **)*piVar5);
    *piVar5 = (int)ppcVar1;
  }
  else if (uVar6 < param_1) {
    if ((uint)(*(int *)((int)this + 0xc) - *piVar3 >> 2) < param_1) {
      FUN_0053ace0(this,param_1);
    }
    else {
      iVar4 = FUN_0053b050(*piVar5,param_1 - uVar6,uVar2);
      *piVar5 = iVar4;
      FUN_00414b60();
    }
  }
  return;
}

