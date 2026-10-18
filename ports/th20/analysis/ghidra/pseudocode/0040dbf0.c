/* Entry: 0x0040dbf0; symbol: FUN_0040dbf0; body bytes: 110 */

_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1> *
__thiscall FUN_0040dbf0(void *this,uint *param_1,uint param_2)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056777d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  _Compressed_pair<><>
            ((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)this);
  local_8 = 0;
  FUN_0040c9e0(this,param_1,param_2);
  *unaff_FS_OFFSET = local_10;
  return (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
          *)this;
}

