/* Entry: 0x0040fa70; symbol: FUN_0040fa70; body bytes: 138 */

void __fastcall
FUN_0040fa70(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  bool bVar1;
  undefined4 uVar2;
  undefined2 local_6;
  
  FUN_0040e5e0();
  bVar1 = FUN_0040f1b0((int)param_1);
  if (bVar1) {
    uVar2 = FUN_0040f160(param_1);
    FUN_0040f060(uVar2,*(LPVOID *)param_1,*(int *)(param_1 + 0x14));
    FUN_0040f6f0((char **)param_1);
  }
  *(undefined4 *)(param_1 + 0x10) = 0;
  *(undefined4 *)(param_1 + 0x14) = 7;
  local_6 = 0;
  FUN_0040ff00((undefined2 *)param_1,&local_6);
  return;
}

