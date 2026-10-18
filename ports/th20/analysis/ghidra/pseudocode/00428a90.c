/* Entry: 0x00428a90; symbol: FUN_00428a90; body bytes: 84 */

uint __thiscall FUN_00428a90(void *this,uint param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = FUN_00428e50((int)this);
  uVar2 = FUN_00429000((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  if ((uVar1 <= uVar2 - (uVar1 >> 1)) && (uVar2 = (uVar1 >> 1) + uVar1, uVar2 < param_1)) {
    uVar2 = param_1;
  }
  return uVar2;
}

