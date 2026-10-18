/* Entry: 0x0040d9e0; symbol: FUN_0040d9e0; body bytes: 105 */

void * __thiscall
FUN_0040d9e0(void *this,
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            *param_1)

{
  undefined4 uVar1;
  undefined1 local_7;
  
  uVar1 = FUN_0040f160(param_1);
  uVar1 = move<>(uVar1);
  FID_conflict__Compressed_pair<class_std::allocator<char>,class_std::_String_val<struct_std::_Simple_types<char>_>,1><class_std::allocator<char>_>
            (this,(uint)local_7,uVar1);
  FUN_0040ec90();
  FUN_0040f740(this,(char **)param_1);
  return this;
}

