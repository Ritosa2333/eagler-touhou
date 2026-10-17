/* Entry: 0x004995d0; symbol: FUN_004995d0; body bytes: 5102 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

float10 __thiscall FUN_004995d0(int param_1,undefined4 param_2)

{
  bool bVar1;
  uint uVar2;
  void *pvVar3;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var4;
  default_delete<class_std::_Facet_base> *pdVar5;
  int iVar6;
  float *pfVar7;
  float *pfVar8;
  float10 fVar9;
  float10 fVar10;
  int local_68;
  uint local_64;
  float local_5c;
  float local_58;
  float local_54;
  float local_50;
  float local_4c;
  float local_48;
  float local_44;
  float local_40;
  float local_3c;
  float local_38;
  float local_34;
  float local_30;
  float local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  float local_14;
  
  switch(param_2) {
  case 0xffffd8f0:
    uVar2 = FUN_00423ee0(0x5ba4a8);
    fVar10 = (float10)(float)((double)(uVar2 & 0x7fffffff) + _DAT_0056cdd0);
    break;
  case 0xffffd8f1:
    fVar10 = FUN_00429830(0x5ba4a8);
    break;
  case 0xffffd8f2:
    fVar10 = FUN_004298e0(0x5ba4a8);
    fVar10 = (float10)((float)fVar10 * DAT_0056e0f0);
    break;
  case 0xffffd8f3:
    pfVar7 = (float *)FUN_0047a2c0(param_1);
    fVar10 = (float10)*pfVar7;
    break;
  case 0xffffd8f4:
    iVar6 = FUN_0047a2c0(param_1);
    fVar10 = (float10)*(float *)(iVar6 + 4);
    break;
  case 0xffffd8f5:
    pfVar7 = (float *)FUN_0048bd10((void *)(param_1 + 0x1e0),0);
    fVar10 = FUN_004292e0(pfVar7);
    break;
  case 0xffffd8f6:
    iVar6 = FUN_0048bd10((void *)(param_1 + 0x1e0),0);
    fVar10 = FUN_00499390(iVar6);
    break;
  case 0xffffd8f7:
    pfVar7 = (float *)FUN_0048bd10((void *)(param_1 + 0x1e0),1);
    fVar10 = FUN_004292e0(pfVar7);
    break;
  case 0xffffd8f8:
    iVar6 = FUN_0048bd10((void *)(param_1 + 0x1e0),1);
    fVar10 = FUN_00499390(iVar6);
    break;
  case 0xffffd8f9:
    iVar6 = FUN_0040ff90(*(int *)(param_1 + 0x424));
    pfVar7 = (float *)FUN_00460850(iVar6);
    fVar10 = (float10)*pfVar7;
    break;
  case 0xffffd8fa:
    iVar6 = FUN_0040ff90(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00460850(iVar6);
    fVar10 = (float10)*(float *)(iVar6 + 4);
    break;
  case 0xffffd8fb:
    iVar6 = FUN_0040ff90(*(int *)(param_1 + 0x424));
    pfVar7 = (float *)FUN_0047a2c0(param_1);
    fVar10 = FUN_004ff350(iVar6,pfVar7);
    break;
  case 0xffffd8fc:
    fVar10 = FUN_00423bd0(param_1 + 0x130);
    break;
  case 0xffffd8fd:
    fVar10 = FUN_004298e0(0x5ba4a8);
    break;
  case 0xffffd8fe:
    fVar10 = (float10)(float)((double)(*(uint *)(param_1 + 0x354) >> 8 & 1) + _DAT_0056cdd0);
    break;
  case 0xffffd8ff:
    fVar10 = (float10)*(int *)(param_1 + 0x100);
    break;
  case 0xffffd900:
    fVar10 = (float10)*(int *)(param_1 + 0x104);
    break;
  case 0xffffd901:
    fVar10 = (float10)*(int *)(param_1 + 0x108);
    break;
  case 0xffffd902:
    fVar10 = (float10)*(int *)(param_1 + 0x10c);
    break;
  case 0xffffd903:
    fVar10 = (float10)*(float *)(param_1 + 0x110);
    break;
  case 0xffffd904:
    fVar10 = (float10)*(float *)(param_1 + 0x114);
    break;
  case 0xffffd905:
    fVar10 = (float10)*(float *)(param_1 + 0x118);
    break;
  case 0xffffd906:
    fVar10 = (float10)*(float *)(param_1 + 0x11c);
    break;
  case 0xffffd907:
    fVar10 = FUN_004292e0((float *)(param_1 + 0x198));
    fVar9 = FUN_004292e0((float *)(param_1 + 0x150));
    fVar10 = (float10)((float)fVar10 - (float)fVar9);
    break;
  case 0xffffd908:
    fVar10 = FUN_00499390(param_1 + 0x198);
    fVar9 = FUN_00499390(param_1 + 0x150);
    fVar10 = (float10)((float)fVar10 - (float)fVar9);
    break;
  case 0xffffd909:
    iVar6 = FUN_0048bd10((void *)(param_1 + 0x1e0),0);
    fVar10 = FUN_0049be00(iVar6);
    break;
  case 0xffffd90a:
    iVar6 = FUN_0048bd10((void *)(param_1 + 0x1e0),0);
    fVar10 = FUN_00474d40(iVar6);
    break;
  case 0xffffd90b:
    iVar6 = FUN_0048bd10((void *)(param_1 + 0x1e0),1);
    fVar10 = FUN_0049be00(iVar6);
    break;
  case 0xffffd90c:
    iVar6 = FUN_0048bd10((void *)(param_1 + 0x1e0),1);
    fVar10 = FUN_00474d40(iVar6);
    break;
  case 0xffffd90d:
    iVar6 = FUN_0048bd10((void *)(param_1 + 0x1e0),0);
    pfVar7 = (float *)FUN_00421830(iVar6);
    fVar10 = FUN_004292e0(pfVar7);
    break;
  case 0xffffd90e:
    iVar6 = FUN_0048bd10((void *)(param_1 + 0x1e0),1);
    pfVar7 = (float *)FUN_00421830(iVar6);
    fVar10 = FUN_004292e0(pfVar7);
    break;
  case 0xffffd90f:
    iVar6 = FUN_0048bd10((void *)(param_1 + 0x1e0),0);
    fVar10 = FUN_0047a320(iVar6);
    break;
  case 0xffffd910:
    iVar6 = FUN_0048bd10((void *)(param_1 + 0x1e0),1);
    fVar10 = FUN_0047a320(iVar6);
    break;
  case 0xffffd911:
    iVar6 = FUN_0048bd10((void *)(param_1 + 0x1e0),0);
    fVar10 = FUN_00499410(iVar6);
    break;
  case 0xffffd912:
    iVar6 = FUN_0048bd10((void *)(param_1 + 0x1e0),1);
    fVar10 = FUN_00499410(iVar6);
    break;
  case 0xffffd913:
    iVar6 = FUN_0040ff90(*(int *)(param_1 + 0x424));
    pfVar7 = (float *)FUN_00460850(iVar6);
    fVar10 = (float10)*pfVar7;
    break;
  case 0xffffd914:
    iVar6 = FUN_0040ff90(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00460850(iVar6);
    fVar10 = (float10)*(float *)(iVar6 + 4);
    break;
  case 0xffffd915:
    pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00485660(pvVar3,0);
    if (iVar6 == 0) {
      local_44 = 0.0;
    }
    else {
      pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
      iVar6 = FUN_00485660(pvVar3,0);
      pfVar7 = (float *)FUN_0047a2c0(iVar6);
      local_44 = *pfVar7;
    }
    fVar10 = (float10)local_44;
    break;
  case 0xffffd916:
    pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00485660(pvVar3,0);
    if (iVar6 == 0) {
      local_48 = DAT_0056cd98;
    }
    else {
      pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
      iVar6 = FUN_00485660(pvVar3,0);
      iVar6 = FUN_0047a2c0(iVar6);
      local_48 = *(float *)(iVar6 + 4);
    }
    fVar10 = (float10)local_48;
    break;
  default:
    fVar10 = (float10)0;
    break;
  case 0xffffd919:
    fVar10 = (float10)*(int *)(param_1 + 0xd8);
    break;
  case 0xffffd91a:
    fVar10 = (float10)FUN_00497540(param_1 + 0x198);
    break;
  case 0xffffd91b:
    fVar10 = (float10)1;
    break;
  case 0xffffd91c:
    iVar6 = FUN_0040ff90(*(int *)(param_1 + 0x424));
    p_Var4 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0048bd10((void *)(param_1 + 0x1e0),0);
    pdVar5 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(p_Var4);
    fVar10 = FUN_004ff350(iVar6,(float *)pdVar5);
    break;
  case 0xffffd91d:
    iVar6 = FUN_0040ff90(*(int *)(param_1 + 0x424));
    p_Var4 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0048bd10((void *)(param_1 + 0x1e0),1);
    pdVar5 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first(p_Var4);
    fVar10 = FUN_004ff350(iVar6,(float *)pdVar5);
    break;
  case 0xffffd91e:
    iVar6 = FUN_0040c300((undefined4 *)(param_1 + 0x214));
    fVar10 = (float10)iVar6;
    break;
  case 0xffffd91f:
    iVar6 = FUN_004640a0(0x5ba568);
    if (iVar6 == 0) {
      local_4c = DAT_0056c8cc;
    }
    else {
      local_4c = 0.0;
    }
    fVar10 = (float10)local_4c;
    break;
  case 0xffffd920:
    iVar6 = FUN_004640a0(0x5ba568);
    if (iVar6 == 1) {
      local_50 = DAT_0056c8cc;
    }
    else {
      local_50 = 0.0;
    }
    fVar10 = (float10)local_50;
    break;
  case 0xffffd921:
    iVar6 = FUN_004640a0(0x5ba568);
    if (iVar6 == 2) {
      local_54 = DAT_0056c8cc;
    }
    else {
      local_54 = 0.0;
    }
    fVar10 = (float10)local_54;
    break;
  case 0xffffd922:
    iVar6 = FUN_004640a0(0x5ba568);
    if (iVar6 == 3) {
      local_58 = DAT_0056c8cc;
    }
    else {
      local_58 = 0.0;
    }
    fVar10 = (float10)local_58;
    break;
  case 0xffffd923:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_004990f0(iVar6);
    fVar10 = (float10)iVar6;
    break;
  case 0xffffd924:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00438500(iVar6);
    fVar10 = (float10)iVar6;
    break;
  case 0xffffd925:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00498e50(iVar6);
    fVar10 = (float10)iVar6;
    break;
  case 0xffffd926:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00478080(iVar6);
    fVar10 = (float10)iVar6;
    break;
  case 0xffffd927:
    iVar6 = FUN_0041caf0(*(int *)(param_1 + 0x424));
    iVar6 = FUN_004989d0(iVar6);
    fVar10 = (float10)iVar6;
    break;
  case 0xffffd928:
    iVar6 = FUN_0040ff90(*(int *)(param_1 + 0x424));
    pfVar7 = (float *)FUN_00460850(iVar6);
    pfVar8 = (float *)FUN_0047a2c0(param_1);
    fVar10 = (float10)FUN_00456030(pfVar8,pfVar7);
    break;
  case 0xffffd929:
    pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00485660(pvVar3,0);
    if (iVar6 == 0) {
      local_1c = 0.0;
    }
    else {
      pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
      iVar6 = FUN_00485660(pvVar3,0);
      local_1c = (float)*(int *)(iVar6 + 0x100);
    }
    fVar10 = (float10)local_1c;
    break;
  case 0xffffd92a:
    pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00485660(pvVar3,0);
    if (iVar6 == 0) {
      local_20 = 0.0;
    }
    else {
      pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
      iVar6 = FUN_00485660(pvVar3,0);
      local_20 = (float)*(int *)(iVar6 + 0x104);
    }
    fVar10 = (float10)local_20;
    break;
  case 0xffffd92b:
    pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00485660(pvVar3,0);
    if (iVar6 == 0) {
      local_24 = 0.0;
    }
    else {
      pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
      iVar6 = FUN_00485660(pvVar3,0);
      local_24 = (float)*(int *)(iVar6 + 0x108);
    }
    fVar10 = (float10)local_24;
    break;
  case 0xffffd92c:
    pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00485660(pvVar3,0);
    if (iVar6 == 0) {
      local_28 = 0.0;
    }
    else {
      pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
      iVar6 = FUN_00485660(pvVar3,0);
      local_28 = (float)*(int *)(iVar6 + 0x10c);
    }
    fVar10 = (float10)local_28;
    break;
  case 0xffffd92d:
    pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00485660(pvVar3,0);
    if (iVar6 == 0) {
      local_2c = 0.0;
    }
    else {
      pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
      iVar6 = FUN_00485660(pvVar3,0);
      local_2c = *(float *)(iVar6 + 0x110);
    }
    fVar10 = (float10)local_2c;
    break;
  case 0xffffd92e:
    pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00485660(pvVar3,0);
    if (iVar6 == 0) {
      local_30 = 0.0;
    }
    else {
      pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
      iVar6 = FUN_00485660(pvVar3,0);
      local_30 = *(float *)(iVar6 + 0x114);
    }
    fVar10 = (float10)local_30;
    break;
  case 0xffffd92f:
    pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00485660(pvVar3,0);
    if (iVar6 == 0) {
      local_34 = 0.0;
    }
    else {
      pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
      iVar6 = FUN_00485660(pvVar3,0);
      local_34 = *(float *)(iVar6 + 0x118);
    }
    fVar10 = (float10)local_34;
    break;
  case 0xffffd930:
    pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00485660(pvVar3,0);
    if (iVar6 == 0) {
      local_38 = 0.0;
    }
    else {
      pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
      iVar6 = FUN_00485660(pvVar3,0);
      local_38 = *(float *)(iVar6 + 0x11c);
    }
    fVar10 = (float10)local_38;
    break;
  case 0xffffd931:
    fVar10 = (float10)*(float *)(param_1 + 0x120);
    break;
  case 0xffffd932:
    fVar10 = (float10)*(float *)(param_1 + 0x124);
    break;
  case 0xffffd933:
    fVar10 = (float10)*(float *)(param_1 + 0x128);
    break;
  case 0xffffd934:
    fVar10 = (float10)*(float *)(param_1 + 300);
    break;
  case 0xffffd935:
    FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_004990e0();
    fVar10 = (float10)(float)((double)iVar6 + *(double *)(&DAT_0056cdd0 + (iVar6 >> 0x1f) * -8));
    break;
  case 0xffffd936:
    iVar6 = FUN_0041caf0(*(int *)(param_1 + 0x424));
    iVar6 = FUN_004993b0(iVar6);
    fVar10 = (float10)iVar6;
    break;
  case 0xffffd939:
    iVar6 = FUN_00488830(DAT_005ba828);
    local_64 = (uint)(iVar6 == 0);
    if (((float)local_64 == _DAT_0056e0e8) || (iVar6 = FUN_0049c000(), iVar6 == 0)) {
      local_68 = 0;
    }
    else {
      local_68 = 1;
    }
    fVar10 = (float10)local_68;
    break;
  case 0xffffd93a:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    fVar10 = (float10)*(int *)(iVar6 + 0x10);
    break;
  case 0xffffd93b:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    fVar10 = (float10)*(int *)(iVar6 + 0x14);
    break;
  case 0xffffd93c:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    fVar10 = (float10)*(int *)(iVar6 + 0x18);
    break;
  case 0xffffd93d:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    fVar10 = (float10)*(int *)(iVar6 + 0x1c);
    break;
  case 0xffffd93e:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    fVar10 = (float10)*(float *)(iVar6 + 0x20);
    break;
  case 0xffffd93f:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    fVar10 = (float10)*(float *)(iVar6 + 0x24);
    break;
  case 0xffffd940:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    fVar10 = (float10)*(float *)(iVar6 + 0x28);
    break;
  case 0xffffd941:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    fVar10 = (float10)*(float *)(iVar6 + 0x2c);
    break;
  case 0xffffd942:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    fVar10 = (float10)*(float *)(iVar6 + 0x30);
    break;
  case 0xffffd943:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    fVar10 = (float10)*(float *)(iVar6 + 0x34);
    break;
  case 0xffffd944:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    fVar10 = (float10)*(float *)(iVar6 + 0x38);
    break;
  case 0xffffd945:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    fVar10 = (float10)*(float *)(iVar6 + 0x3c);
    break;
  case 0xffffd946:
    iVar6 = FUN_0040c300((undefined4 *)(param_1 + 0x88));
    fVar10 = (float10)(float)((double)iVar6 + *(double *)(&DAT_0056cdd0 + (iVar6 >> 0x1f) * -8));
    break;
  case 0xffffd949:
    pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00485660(pvVar3,0);
    if (iVar6 == 0) {
      local_3c = 0.0;
    }
    else {
      pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
      iVar6 = FUN_00485660(pvVar3,0);
      fVar10 = (float10)FUN_00497540(iVar6 + 0x198);
      local_3c = (float)fVar10;
    }
    fVar10 = (float10)local_3c;
    break;
  case 0xffffd94a:
    pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00485660(pvVar3,0);
    if (iVar6 == 0) {
      local_40 = 0.0;
    }
    else {
      pvVar3 = (void *)FUN_00412730(*(int *)(param_1 + 0x424));
      iVar6 = FUN_00485660(pvVar3,0);
      iVar6 = FUN_0048bd10((void *)(iVar6 + 0x1e0),0);
      fVar10 = FUN_0047a320(iVar6);
      local_40 = (float)fVar10;
    }
    fVar10 = (float10)local_40;
    break;
  case 0xffffd94b:
    iVar6 = FUN_0040c300((undefined4 *)(param_1 + 0x8c));
    fVar10 = (float10)(float)((double)iVar6 + *(double *)(&DAT_0056cdd0 + (iVar6 >> 0x1f) * -8));
    break;
  case 0xffffd94c:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    iVar6 = FUN_00499130(iVar6);
    fVar10 = (float10)iVar6;
    break;
  case 0xffffd94d:
    iVar6 = FUN_00499460(0x5ba568);
    fVar10 = (float10)iVar6;
    break;
  case 0xffffd94e:
    fVar10 = (float10)(float)((double)(*(uint *)(param_1 + 0x354) >> 3 & 1) + _DAT_0056cdd0);
    break;
  case 0xffffd94f:
    iVar6 = FUN_00499330(0x5ba568);
    fVar10 = (float10)iVar6;
    break;
  case 0xffffd955:
    fVar10 = (float10)DAT_005afcfc;
    break;
  case 0xffffd956:
    iVar6 = FUN_0040c300(*(undefined4 **)(param_1 + 0x424));
    iVar6 = FUN_004992f0(iVar6);
    fVar10 = (float10)iVar6;
    break;
  case 0xffffd961:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    fVar10 = (float10)*(int *)(iVar6 + 0x40);
    break;
  case 0xffffd962:
    iVar6 = FUN_00412730(*(int *)(param_1 + 0x424));
    fVar10 = (float10)*(int *)(iVar6 + 0x44);
    break;
  case 0xffffd963:
    iVar6 = FUN_004640a0(0x5ba568);
    fVar10 = (float10)iVar6;
    break;
  case 0xffffd964:
    iVar6 = FUN_00460830(1 - *(int *)(param_1 + 0x420));
    pfVar7 = (float *)FUN_00460850(iVar6);
    fVar10 = (float10)*pfVar7;
    break;
  case 0xffffd965:
    iVar6 = FUN_00460830(1 - *(int *)(param_1 + 0x420));
    iVar6 = FUN_00460850(iVar6);
    fVar10 = (float10)*(float *)(iVar6 + 4);
    break;
  case 0xffffd966:
    fVar10 = (float10)*(int *)(param_1 + 0x420);
    break;
  case 0xffffd967:
    iVar6 = FUN_00464080(0);
    iVar6 = FUN_00499520(iVar6);
    fVar10 = (float10)iVar6;
    break;
  case 0xffffd968:
    iVar6 = FUN_00464080(0);
    iVar6 = FUN_0041cab0(iVar6);
    fVar10 = (float10)((float)iVar6 / DAT_0056c8d0);
    break;
  case 0xffffd969:
    fVar10 = (float10)(float)((double)(*(uint *)(param_1 + 0x358) >> 6 & 1) + _DAT_0056cdd0);
    break;
  case 0xffffd96a:
    uVar2 = FUN_00423ee0(0x5ba4a8);
    if (uVar2 % 2 == 0) {
      local_14 = DAT_0056c8cc;
    }
    else {
      local_14 = DAT_0056e0f4;
    }
    fVar10 = (float10)local_14;
    break;
  case 0xffffd96b:
    iVar6 = FUN_00478e80(0);
    iVar6 = FUN_004992f0(iVar6);
    fVar10 = (float10)iVar6;
    break;
  case 0xffffd96c:
    iVar6 = FUN_00478ea0(0);
    bVar1 = FUN_004887a0(iVar6);
    fVar10 = (float10)bVar1;
    break;
  case 0xffffd96d:
    iVar6 = FUN_00478060(0);
    bVar1 = FUN_0049be20(iVar6);
    fVar10 = (float10)bVar1;
    break;
  case 0xffffd96e:
    iVar6 = FUN_00478060(0);
    bVar1 = FUN_0049be20(iVar6);
    if (bVar1) {
      iVar6 = FUN_00478060(0);
      iVar6 = FUN_00498970(iVar6);
      pfVar7 = (float *)FUN_0047a2c0(iVar6);
      pfVar8 = (float *)FUN_0047a2c0(param_1);
      fVar10 = (float10)FUN_00456030(pfVar8,pfVar7);
      local_5c = (float)fVar10;
    }
    else {
      local_5c = DAT_0056e0f4;
    }
    fVar10 = (float10)local_5c;
  }
  return fVar10;
}

