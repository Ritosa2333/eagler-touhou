/* Entry: 0x0040f9e0; symbol: FUN_0040f9e0; body bytes: 136 */

void __fastcall
FUN_0040f9e0(_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             *param_1)

{
  bool bVar1;
  undefined4 uVar2;
  undefined1 local_5;
  
  FUN_0040e5e0();
  bVar1 = FUN_0040f180((int)param_1);
  if (bVar1) {
    uVar2 = FUN_0040f160(param_1);
    FUN_0040f040(uVar2,*(LPVOID *)param_1,*(int *)(param_1 + 0x14));
    FUN_0040f6a0((char **)param_1);
  }
  *(undefined4 *)(param_1 + 0x10) = 0;
  *(undefined4 *)(param_1 + 0x14) = 0xf;
  local_5 = 0;
  FUN_0040fe90(param_1,&local_5);
  return;
}

