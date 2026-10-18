/* Entry: 0x00488fb0; symbol: FUN_00488fb0; body bytes: 192 */

_Ref_count_base * __thiscall FUN_00488fb0(void *this,undefined4 param_1,undefined4 param_2)

{
  undefined4 uVar1;
  undefined4 uVar2;
  default_delete<class_std::_Facet_base> *pdVar3;
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056928d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0047a770((void *)((int)this + 0xc),param_1);
  *(undefined4 *)this = 0;
  *(undefined4 *)((int)this + 4) = 0;
  *(undefined4 *)((int)this + 8) = 0;
  std::_Ref_count_base::_Ref_count_base((_Ref_count_base *)this);
  local_8 = 0;
  *(undefined ***)this = &PTR_FUN_0056fd20;
  uVar1 = move<>(param_2);
  uVar2 = move<>((int)this + 0xc);
  pdVar3 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)((int)this + 0xc));
  FUN_0048ad30(pdVar3,uVar2,uVar1);
  *unaff_FS_OFFSET = local_10;
  return (_Ref_count_base *)this;
}

