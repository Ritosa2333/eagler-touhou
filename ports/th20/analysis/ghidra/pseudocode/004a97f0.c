/* Entry: 0x004a97f0; symbol: FUN_004a97f0; body bytes: 84 */

uint __thiscall FUN_004a97f0(void *this,uint param_1)

{
  uint uVar1;
  uint uVar2;
  
  uVar1 = FUN_004aa640((int)this);
  uVar2 = FUN_004ab440((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)this);
  if ((uVar1 <= uVar2 - (uVar1 >> 1)) && (uVar2 = (uVar1 >> 1) + uVar1, uVar2 < param_1)) {
    uVar2 = param_1;
  }
  return uVar2;
}

