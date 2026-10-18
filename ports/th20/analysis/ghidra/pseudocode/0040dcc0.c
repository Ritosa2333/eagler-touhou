/* Entry: 0x0040dcc0; symbol: FUN_0040dcc0; body bytes: 164 */

void * __thiscall
FUN_0040dcc0(void *this,
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *param_1)

{
  uint uVar1;
  undefined4 uVar2;
  uint *puVar3;
  int *unaff_FS_OFFSET;
  byte local_13;
  undefined1 local_12 [2];
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056772d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_0040f160(param_1);
  uVar2 = move<>(local_12);
  FID_conflict__Compressed_pair<class_std::allocator<char>,class_std::_String_val<struct_std::_Simple_types<char>_>,1><class_std::allocator<char>_>
            (this,(uint)local_13,uVar2);
  local_8 = 0;
  uVar1 = *(uint *)(param_1 + 0x10);
  puVar3 = Myptr((undefined4 *)param_1);
  FUN_0040cc50(this,puVar3,uVar1);
  *unaff_FS_OFFSET = local_10;
  return this;
}

