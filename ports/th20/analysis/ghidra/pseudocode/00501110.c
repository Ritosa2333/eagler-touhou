/* Entry: 0x00501110; symbol: FUN_00501110; body bytes: 182 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

undefined4 __fastcall FUN_00501110(int param_1)

{
  int iVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  float10 fVar3;
  float10 fVar4;
  
  iVar1 = FUN_0040ff90(*(int *)(param_1 + 0x120));
  pdVar2 = std::
           _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
           ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         *)(param_1 + 0x50));
  fVar3 = FUN_004ff3f0(iVar1,(float *)pdVar2);
  fVar4 = FUN_004298e0(0x5ba4a8);
  FUN_0047a560((void *)(param_1 + 0x50),
               (float)fVar3 + (float)fVar4 * (DAT_0056e0f0 / _DAT_00570db0) * DAT_0056c8d0);
  FUN_0047a5c0((void *)(param_1 + 0x50),DAT_0056cd90);
  return 0;
}

