/* Entry: 0x0047a660; symbol: FUN_0047a660; body bytes: 173 */

_Ref_count_base * __thiscall FUN_0047a660(void *this,undefined4 param_1)

{
  undefined4 uVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00568fdd;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0047a770((void *)((int)this + 0xc),param_1);
  *(undefined4 *)this = 0;
  *(undefined4 *)((int)this + 4) = 0;
  *(undefined4 *)((int)this + 8) = 0;
  std::_Ref_count_base::_Ref_count_base((_Ref_count_base *)this);
  local_8 = 0;
  *(undefined ***)this = &PTR_FUN_0056fd20;
  uVar1 = move<>((int)this + 0xc);
  pdVar2 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)((int)this + 0xc));
  FUN_0047b410(pdVar2,uVar1);
  *unaff_FS_OFFSET = local_10;
  return (_Ref_count_base *)this;
}

