/* Entry: 0x004c6010; symbol: FUN_004c6010; body bytes: 102 */

void __fastcall FUN_004c6010(undefined4 *param_1)

{
  char cVar1;
  undefined4 *puVar2;
  
  cVar1 = FUN_004c5d90((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                        *)(param_1 + 8));
  if (cVar1 != '\x01') {
    puVar2 = (undefined4 *)
             FUN_004c7f00((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(param_1 + 8));
    *param_1 = *puVar2;
    FUN_004c5ff0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)(param_1 + 8));
    puVar2 = (undefined4 *)
             FUN_004c7f00((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(param_1 + 0xd));
    param_1[2] = *puVar2;
    FUN_004c5ff0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                  *)(param_1 + 0xd));
  }
  FUN_00497f40((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 4));
  return;
}

