/* Entry: 0x004e7b70; symbol: FUN_004e7b70; body bytes: 106 */

undefined8 __cdecl FUN_004e7b70(undefined1 *param_1,undefined1 *param_2)

{
  undefined4 uVar1;
  default_delete<class_std::_Facet_base> *this;
  undefined4 *puVar2;
  undefined1 *puVar3;
  
  while (param_1 != param_2) {
    puVar3 = param_1;
    this = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)&stack0x0000000c);
    FUN_004f5690(this,puVar3);
    param_1 = param_1 + 1;
    std::
    _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>::
    _Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)&stack0x0000000c);
  }
  puVar2 = (undefined4 *)move<>(&param_1);
  uVar1 = *puVar2;
  puVar2 = (undefined4 *)move<>(&stack0x0000000c);
  return CONCAT44(*puVar2,uVar1);
}

