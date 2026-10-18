/* Entry: 0x004e6f50; symbol: FUN_004e6f50; body bytes: 164 */

void __cdecl FUN_004e6f50(undefined4 *param_1,undefined4 param_2,int param_3,undefined1 *param_4)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  default_delete<class_std::_Facet_base> *this;
  undefined1 *puVar3;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (0 < param_3) {
    FUN_0040c080(&local_c,4);
    uVar1 = move<>(&param_2);
    puVar2 = (undefined4 *)move<>(uVar1);
    local_c = *puVar2;
    FUN_00411170();
    for (; 0 < param_3; param_3 = param_3 + -1) {
      puVar3 = param_4;
      this = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)&local_c);
      FUN_004f5690(this,puVar3);
      std::
      _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
      ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                    *)&local_c);
    }
    uVar1 = move<>(&local_c);
    FUN_004ee760(&param_2,uVar1);
  }
  *param_1 = param_2;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

