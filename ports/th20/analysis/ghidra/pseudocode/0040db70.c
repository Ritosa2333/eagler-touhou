/* Entry: 0x0040db70; symbol: FUN_0040db70; body bytes: 128 */

_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1> *
__thiscall FUN_0040db70(void *this,uint *param_1)

{
  int iVar1;
  uint uVar2;
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
  iVar1 = FUN_00411180((char *)param_1);
  uVar2 = move<>(iVar1);
  FUN_0040c9e0(this,param_1,uVar2);
  *unaff_FS_OFFSET = local_10;
  return (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
          *)this;
}

