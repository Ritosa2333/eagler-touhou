/* Entry: 0x00504af0; symbol: FUN_00504af0; body bytes: 216 */

undefined4 __fastcall FUN_00504af0(int param_1)

{
  void *pvVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  float10 fVar3;
  
  if (*(char *)(param_1 + 0x114) == '\0') {
    pvVar1 = (void *)FUN_0044ced0((undefined4 *)(param_1 + 0x18));
    pdVar2 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(param_1 + 0x50));
    *(undefined4 *)(pdVar2 + 8) = DAT_0056ef28;
    FUN_00506a00(pvVar1);
    *(undefined4 *)(param_1 + 0x9c) = 2;
    fVar3 = FUN_0047a320(param_1 + 0x50);
    FUN_0047a5c0((void *)(param_1 + 0x50),(float)fVar3 / DAT_0056d7bc);
    pdVar2 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(param_1 + 0x50));
    *(undefined4 *)((int)pvVar1 + 0x5bc) = *(undefined4 *)pdVar2;
    *(undefined4 *)((int)pvVar1 + 0x5c0) = *(undefined4 *)(pdVar2 + 4);
    *(undefined4 *)((int)pvVar1 + 0x5c4) = *(undefined4 *)(pdVar2 + 8);
    FUN_004c1f00((undefined4 *)(param_1 + 0xdc));
    FUN_004117a0((void *)(param_1 + 0xdc),0);
  }
  return *(undefined4 *)(param_1 + 0xac);
}

