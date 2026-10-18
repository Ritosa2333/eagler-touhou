/* Entry: 0x004f8420; symbol: FUN_004f8420; body bytes: 663 */

void __fastcall FUN_004f8420(void *param_1)

{
  bool bVar1;
  int iVar2;
  void *pvVar3;
  float *pfVar4;
  default_delete<class_std::_Facet_base> *pdVar5;
  default_delete<class_std::_Facet_base> *local_10;
  default_delete<class_std::_Facet_base> *local_c;
  
  bVar1 = FUN_00488850(0x5ba568);
  if (!bVar1) {
    FUN_00478160(DAT_005c06a4);
  }
  iVar2 = FUN_00478060(0);
  FUN_00478190(iVar2);
  pvVar3 = (void *)FUN_0041cad0(*(int *)((int)param_1 + 0x14858));
  pfVar4 = (float *)FUN_00460850((int)param_1);
  FUN_004fb820(pvVar3,0,0x15,pfVar4,0);
  pvVar3 = (void *)FUN_0041caf0(*(int *)((int)param_1 + 0x14858));
  FUN_004e1250(pvVar3,-1);
  pvVar3 = (void *)FUN_0041caf0(*(int *)((int)param_1 + 0x14858));
  FUN_004fe950(pvVar3,1);
  pvVar3 = (void *)FUN_00464080(0);
  FUN_004fea60(pvVar3,-3);
  pvVar3 = (void *)FUN_00464080(0);
  FUN_004fe9b0(pvVar3,-3);
  pvVar3 = (void *)FUN_00464080(0);
  FUN_004feab0(pvVar3,-3);
  pvVar3 = (void *)FUN_00464080(0);
  FUN_004fea00(pvVar3,-3);
  pvVar3 = (void *)FUN_00464080(0);
  FUN_00477f60(pvVar3,-100);
  FUN_00412da0(param_1,2);
  FUN_00423520((void *)((int)param_1 + 0x644),0);
  FUN_00423520((void *)((int)param_1 + 0x2050),0xb4);
  FUN_004382b0(*(void **)((int)param_1 + 0x1c),(void *)((int)param_1 + 0x28),0,0);
  local_c = std::
            _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
            ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                          *)((int)param_1 + 0x684));
  pdVar5 = (default_delete<class_std::_Facet_base> *)FUN_004fce60((int)param_1 + 0x684);
  for (; local_c != pdVar5; local_c = local_c + 300) {
    *(undefined4 *)local_c = 0;
    FUN_00479040(local_c + 0xdc);
    FUN_00479040(local_c + 0xe0);
  }
  *(undefined4 *)((int)param_1 + 0x680) = 0;
  local_10 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)((int)param_1 + 0x123c));
  pdVar5 = (default_delete<class_std::_Facet_base> *)FUN_004fce80((int)param_1 + 0x123c);
  for (; local_10 != pdVar5; local_10 = local_10 + 300) {
    *(undefined4 *)local_10 = 0;
    FUN_00479040(local_10 + 0xdc);
    FUN_00479040(local_10 + 0xe0);
  }
  iVar2 = FUN_00411700(*(int *)((int)param_1 + 0x14858));
  FUN_00487bb0(iVar2);
  pvVar3 = (void *)FUN_00412730(*(int *)((int)param_1 + 0x14858));
  FUN_004fe930(pvVar3,1);
  pvVar3 = (void *)FUN_00412730(*(int *)((int)param_1 + 0x14858));
  FUN_004ab630(pvVar3,0);
  pvVar3 = (void *)FUN_00498f40();
  FUN_004feb00(pvVar3,1);
  return;
}

