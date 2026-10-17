/* Entry: 0x004253d0; symbol: FUN_004253d0; body bytes: 243 */

void __thiscall FUN_004253d0(void *this,uint param_1)

{
  undefined4 uVar1;
  int *piVar2;
  int iVar3;
  char **ppcVar4;
  int *piVar5;
  uint uVar6;
  
  uVar1 = FUN_0040f160((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  piVar2 = (int *)((int)this + 4);
  piVar5 = (int *)((int)this + 8);
  uVar6 = *piVar5 - *piVar2 >> 4;
  if (param_1 < uVar6) {
    ppcVar4 = (char **)(param_1 * 0x10 + *piVar2);
    FUN_00414b60();
    std::_Adl_verify_range<char*,char_const*>(ppcVar4,(char **)*piVar5);
    *piVar5 = (int)ppcVar4;
  }
  else if (uVar6 < param_1) {
    if ((uint)(*(int *)((int)this + 0xc) - *piVar2 >> 4) < param_1) {
      FUN_004254d0(this,param_1);
    }
    else {
      iVar3 = FUN_00425710(*piVar5,param_1 - uVar6,uVar1);
      *piVar5 = iVar3;
      FUN_00414b60();
    }
  }
  return;
}

