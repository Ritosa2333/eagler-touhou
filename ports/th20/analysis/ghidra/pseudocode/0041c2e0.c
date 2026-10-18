/* Entry: 0x0041c2e0; symbol: FUN_0041c2e0; body bytes: 58 */

undefined1 __cdecl
FUN_0041c2e0(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  LPCWSTR pWVar1;
  undefined8 uVar2;
  undefined1 local_c;
  int local_8;
  
  pWVar1 = (LPCWSTR)FUN_0040ff70((undefined4 *)param_1);
  uVar2 = FUN_005411b9(pWVar1);
  local_8 = (int)((ulonglong)uVar2 >> 0x20);
  if (local_8 != 0) {
    FUN_0041bdb0((uint *)"create_directory",local_8,param_1);
  }
  local_c = (undefined1)uVar2;
  return local_c;
}

