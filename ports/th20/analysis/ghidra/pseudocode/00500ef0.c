/* Entry: 0x00500ef0; symbol: FUN_00500ef0; body bytes: 226 */

void __fastcall FUN_00500ef0(int param_1)

{
  void *this;
  default_delete<class_std::_Facet_base> *pdVar1;
  int iVar2;
  float *pfVar3;
  float10 fVar4;
  int local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  this = (void *)FUN_00412730(*(int *)(param_1 + 0x120));
  pdVar1 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)(param_1 + 0x50));
  FUN_004aac00(this,&local_c,(float *)pdVar1,DAT_0056cd98);
  iVar2 = FUN_0040c300(&local_c);
  if (iVar2 == 0) {
    FUN_0047a560((void *)(param_1 + 0x50),DAT_0056e0f8 / DAT_0056c8d0);
  }
  else {
    iVar2 = FUN_004aaac0(&local_c);
    pfVar3 = (float *)FUN_0047a2c0(iVar2);
    pdVar1 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(param_1 + 0x50));
    fVar4 = (float10)FUN_0047a2e0((float *)pdVar1,pfVar3);
    FUN_0047a560((void *)(param_1 + 0x50),(float)fVar4);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

