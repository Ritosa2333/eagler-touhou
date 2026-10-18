/* Entry: 0x0049abc0; symbol: FUN_0049abc0; body bytes: 4152 */

uint __thiscall FUN_0049abc0(void *this,undefined4 param_1)

{
  bool bVar1;
  uint uVar2;
  undefined4 *puVar3;
  int iVar4;
  float *pfVar5;
  float *pfVar6;
  void *pvVar7;
  float10 fVar8;
  float10 fVar9;
  uint local_50;
  uint local_4c;
  uint local_48;
  uint local_44;
  uint local_40;
  uint local_3c;
  uint local_38;
  uint local_34;
  uint local_30;
  uint local_2c;
  uint local_28;
  uint local_24;
  uint local_20;
  uint local_1c;
  uint local_18;
  uint local_14;
  uint local_10;
  
  switch(param_1) {
  case 0xffffd8f0:
    local_24 = FUN_00423ee0(0x5ba4a8);
    local_24 = local_24 & 0x7fffffff;
    break;
  case 0xffffd8f1:
    fVar9 = FUN_00429830(0x5ba4a8);
    local_24 = (uint)fVar9;
    break;
  default:
    local_24 = 0;
    break;
  case 0xffffd8f3:
    pfVar5 = (float *)FUN_0047a2c0((int)this);
    local_24 = (uint)*pfVar5;
    break;
  case 0xffffd8f4:
    iVar4 = FUN_0047a2c0((int)this);
    local_24 = (uint)*(float *)(iVar4 + 4);
    break;
  case 0xffffd8f5:
    pfVar5 = (float *)FUN_0048bd10((void *)((int)this + 0x1e0),0);
    fVar9 = FUN_004292e0(pfVar5);
    local_24 = (uint)fVar9;
    break;
  case 0xffffd8f6:
    iVar4 = FUN_0048bd10((void *)((int)this + 0x1e0),0);
    fVar9 = FUN_00499390(iVar4);
    local_24 = (uint)fVar9;
    break;
  case 0xffffd8f7:
    pfVar5 = (float *)FUN_0048bd10((void *)((int)this + 0x1e0),1);
    fVar9 = FUN_004292e0(pfVar5);
    local_24 = (uint)fVar9;
    break;
  case 0xffffd8f8:
    iVar4 = FUN_0048bd10((void *)((int)this + 0x1e0),1);
    fVar9 = FUN_00499390(iVar4);
    local_24 = (uint)fVar9;
    break;
  case 0xffffd8f9:
    iVar4 = FUN_0040ff90(*(int *)((int)this + 0x424));
    pfVar5 = (float *)FUN_00460850(iVar4);
    local_24 = (uint)*pfVar5;
    break;
  case 0xffffd8fa:
    iVar4 = FUN_0040ff90(*(int *)((int)this + 0x424));
    iVar4 = FUN_00460850(iVar4);
    local_24 = (uint)*(float *)(iVar4 + 4);
    break;
  case 0xffffd8fc:
    local_24 = FUN_0040ff90((int)this + 0x130);
    break;
  case 0xffffd8fd:
    fVar9 = FUN_004298e0(0x5ba4a8);
    local_24 = (uint)fVar9;
    break;
  case 0xffffd8fe:
    local_24 = *(uint *)((int)this + 0x354) >> 8 & 1;
    break;
  case 0xffffd8ff:
    local_24 = *(uint *)((int)this + 0x100);
    break;
  case 0xffffd900:
    local_24 = *(uint *)((int)this + 0x104);
    break;
  case 0xffffd901:
    local_24 = *(uint *)((int)this + 0x108);
    break;
  case 0xffffd902:
    local_24 = *(uint *)((int)this + 0x10c);
    break;
  case 0xffffd903:
    local_24 = (uint)*(float *)((int)this + 0x110);
    break;
  case 0xffffd904:
    local_24 = (uint)*(float *)((int)this + 0x114);
    break;
  case 0xffffd905:
    local_24 = (uint)*(float *)((int)this + 0x118);
    break;
  case 0xffffd906:
    local_24 = (uint)*(float *)((int)this + 0x11c);
    break;
  case 0xffffd907:
    fVar9 = FUN_004292e0((float *)((int)this + 0x198));
    fVar8 = FUN_004292e0((float *)((int)this + 0x150));
    local_24 = (uint)((float)fVar9 - (float)fVar8);
    break;
  case 0xffffd908:
    fVar9 = FUN_00499390((int)this + 0x198);
    fVar8 = FUN_00499390((int)this + 0x150);
    local_24 = (uint)((float)fVar9 - (float)fVar8);
    break;
  case 0xffffd909:
    iVar4 = FUN_0048bd10((void *)((int)this + 0x1e0),0);
    fVar9 = FUN_0049be00(iVar4);
    local_24 = (uint)fVar9;
    break;
  case 0xffffd90a:
    iVar4 = FUN_0048bd10((void *)((int)this + 0x1e0),0);
    fVar9 = FUN_00474d40(iVar4);
    local_24 = (uint)fVar9;
    break;
  case 0xffffd90b:
    iVar4 = FUN_0048bd10((void *)((int)this + 0x1e0),1);
    fVar9 = FUN_0049be00(iVar4);
    local_24 = (uint)fVar9;
    break;
  case 0xffffd90c:
    iVar4 = FUN_0048bd10((void *)((int)this + 0x1e0),1);
    fVar9 = FUN_00474d40(iVar4);
    local_24 = (uint)fVar9;
    break;
  case 0xffffd90d:
    iVar4 = FUN_0048bd10((void *)((int)this + 0x1e0),0);
    pfVar5 = (float *)FUN_00421830(iVar4);
    fVar9 = FUN_004292e0(pfVar5);
    local_24 = (uint)fVar9;
    break;
  case 0xffffd90e:
    iVar4 = FUN_0048bd10((void *)((int)this + 0x1e0),1);
    pfVar5 = (float *)FUN_00421830(iVar4);
    fVar9 = FUN_004292e0(pfVar5);
    local_24 = (uint)fVar9;
    break;
  case 0xffffd90f:
    iVar4 = FUN_0048bd10((void *)((int)this + 0x1e0),0);
    fVar9 = FUN_0047a320(iVar4);
    local_24 = (uint)fVar9;
    break;
  case 0xffffd910:
    iVar4 = FUN_0048bd10((void *)((int)this + 0x1e0),1);
    fVar9 = FUN_0047a320(iVar4);
    local_24 = (uint)fVar9;
    break;
  case 0xffffd911:
    iVar4 = FUN_0048bd10((void *)((int)this + 0x1e0),0);
    fVar9 = FUN_00499410(iVar4);
    local_24 = (uint)fVar9;
    break;
  case 0xffffd912:
    iVar4 = FUN_0048bd10((void *)((int)this + 0x1e0),1);
    fVar9 = FUN_00499410(iVar4);
    local_24 = (uint)fVar9;
    break;
  case 0xffffd913:
    iVar4 = FUN_0040ff90(*(int *)((int)this + 0x424));
    pfVar5 = (float *)FUN_00460850(iVar4);
    local_24 = (uint)*pfVar5;
    break;
  case 0xffffd914:
    iVar4 = FUN_0040ff90(*(int *)((int)this + 0x424));
    iVar4 = FUN_00460850(iVar4);
    local_24 = (uint)*(float *)(iVar4 + 4);
    break;
  case 0xffffd915:
    pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar4 = FUN_00485660(pvVar7,0);
    pfVar5 = (float *)FUN_0047a2c0(iVar4);
    local_24 = (uint)*pfVar5;
    break;
  case 0xffffd916:
    pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar4 = FUN_00485660(pvVar7,0);
    iVar4 = FUN_0047a2c0(iVar4);
    local_24 = (uint)*(float *)(iVar4 + 4);
    break;
  case 0xffffd917:
    puVar3 = (undefined4 *)FUN_0048bce0((void *)((int)this + 0x94),0);
    iVar4 = FUN_0044ced0(puVar3);
    local_24 = FUN_00498950(iVar4);
    break;
  case 0xffffd919:
    local_24 = *(uint *)((int)this + 0xd8);
    break;
  case 0xffffd91a:
    fVar9 = (float10)FUN_00497540((int)this + 0x198);
    local_24 = (uint)fVar9;
    break;
  case 0xffffd91b:
    local_24 = 1;
    break;
  case 0xffffd91e:
    local_24 = FUN_0040c300((undefined4 *)((int)this + 0x214));
    break;
  case 0xffffd91f:
    iVar4 = FUN_004640a0(0x5ba568);
    local_14 = (uint)(iVar4 == 0);
    local_24 = local_14;
    break;
  case 0xffffd920:
    iVar4 = FUN_004640a0(0x5ba568);
    local_18 = (uint)(iVar4 == 1);
    local_24 = local_18;
    break;
  case 0xffffd921:
    iVar4 = FUN_004640a0(0x5ba568);
    local_1c = (uint)(iVar4 == 2);
    local_24 = local_1c;
    break;
  case 0xffffd922:
    iVar4 = FUN_004640a0(0x5ba568);
    local_20 = (uint)(iVar4 == 3);
    local_24 = local_20;
    break;
  case 0xffffd923:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = FUN_004990f0(iVar4);
    break;
  case 0xffffd924:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = FUN_00438500(iVar4);
    break;
  case 0xffffd925:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = FUN_00498e50(iVar4);
    break;
  case 0xffffd926:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = FUN_00478080(iVar4);
    break;
  case 0xffffd927:
    iVar4 = FUN_0041caf0(*(int *)((int)this + 0x424));
    local_24 = FUN_004989d0(iVar4);
    break;
  case 0xffffd928:
    iVar4 = FUN_0040ff90(*(int *)((int)this + 0x424));
    pfVar5 = (float *)FUN_00460850(iVar4);
    pfVar6 = (float *)FUN_0047a2c0((int)this);
    fVar9 = (float10)FUN_00456030(pfVar6,pfVar5);
    local_24 = (uint)fVar9;
    break;
  case 0xffffd929:
    pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar4 = FUN_00485660(pvVar7,0);
    if (iVar4 == 0) {
      local_2c = 0;
      local_24 = local_2c;
    }
    else {
      pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar4 = FUN_00485660(pvVar7,0);
      local_24 = *(uint *)(iVar4 + 0x100);
    }
    break;
  case 0xffffd92a:
    pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar4 = FUN_00485660(pvVar7,0);
    if (iVar4 == 0) {
      local_30 = 0;
      local_24 = local_30;
    }
    else {
      pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar4 = FUN_00485660(pvVar7,0);
      local_24 = *(uint *)(iVar4 + 0x104);
    }
    break;
  case 0xffffd92b:
    pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar4 = FUN_00485660(pvVar7,0);
    if (iVar4 == 0) {
      local_34 = 0;
      local_24 = local_34;
    }
    else {
      pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar4 = FUN_00485660(pvVar7,0);
      local_24 = *(uint *)(iVar4 + 0x108);
    }
    break;
  case 0xffffd92c:
    pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar4 = FUN_00485660(pvVar7,0);
    if (iVar4 == 0) {
      local_38 = 0;
      local_24 = local_38;
    }
    else {
      pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar4 = FUN_00485660(pvVar7,0);
      local_24 = *(uint *)(iVar4 + 0x10c);
    }
    break;
  case 0xffffd92d:
    pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar4 = FUN_00485660(pvVar7,0);
    if (iVar4 == 0) {
      local_3c = 0;
      local_24 = local_3c;
    }
    else {
      pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar4 = FUN_00485660(pvVar7,0);
      local_24 = (uint)*(float *)(iVar4 + 0x110);
    }
    break;
  case 0xffffd92e:
    pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar4 = FUN_00485660(pvVar7,0);
    if (iVar4 == 0) {
      local_40 = 0;
      local_24 = local_40;
    }
    else {
      pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar4 = FUN_00485660(pvVar7,0);
      local_24 = (uint)*(float *)(iVar4 + 0x114);
    }
    break;
  case 0xffffd92f:
    pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar4 = FUN_00485660(pvVar7,0);
    if (iVar4 == 0) {
      local_44 = 0;
      local_24 = local_44;
    }
    else {
      pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar4 = FUN_00485660(pvVar7,0);
      local_24 = (uint)*(float *)(iVar4 + 0x118);
    }
    break;
  case 0xffffd930:
    pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar4 = FUN_00485660(pvVar7,0);
    if (iVar4 == 0) {
      local_48 = 0;
      local_24 = local_48;
    }
    else {
      pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar4 = FUN_00485660(pvVar7,0);
      local_24 = (uint)*(float *)(iVar4 + 0x11c);
    }
    break;
  case 0xffffd931:
    local_24 = (uint)*(float *)((int)this + 0x120);
    break;
  case 0xffffd932:
    local_24 = (uint)*(float *)((int)this + 0x124);
    break;
  case 0xffffd933:
    local_24 = (uint)*(float *)((int)this + 0x128);
    break;
  case 0xffffd934:
    local_24 = (uint)*(float *)((int)this + 300);
    break;
  case 0xffffd935:
    FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = FUN_004990e0();
    break;
  case 0xffffd936:
    iVar4 = FUN_0041caf0(*(int *)((int)this + 0x424));
    local_24 = FUN_004993b0(iVar4);
    break;
  case 0xffffd939:
    iVar4 = FUN_00488830(DAT_005ba828);
    if ((iVar4 == 0) && (iVar4 = FUN_0049c000(), iVar4 != 0)) {
      local_28 = 1;
      local_24 = local_28;
    }
    else {
      local_28 = 0;
      local_24 = local_28;
    }
    break;
  case 0xffffd93a:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = *(uint *)(iVar4 + 0x10);
    break;
  case 0xffffd93b:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = *(uint *)(iVar4 + 0x14);
    break;
  case 0xffffd93c:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = *(uint *)(iVar4 + 0x18);
    break;
  case 0xffffd93d:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = *(uint *)(iVar4 + 0x1c);
    break;
  case 0xffffd93e:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = (uint)*(float *)(iVar4 + 0x20);
    break;
  case 0xffffd93f:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = (uint)*(float *)(iVar4 + 0x24);
    break;
  case 0xffffd940:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = (uint)*(float *)(iVar4 + 0x28);
    break;
  case 0xffffd941:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = (uint)*(float *)(iVar4 + 0x2c);
    break;
  case 0xffffd942:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = (uint)*(float *)(iVar4 + 0x30);
    break;
  case 0xffffd943:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = (uint)*(float *)(iVar4 + 0x34);
    break;
  case 0xffffd944:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = (uint)*(float *)(iVar4 + 0x38);
    break;
  case 0xffffd945:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = (uint)*(float *)(iVar4 + 0x3c);
    break;
  case 0xffffd946:
    local_24 = FUN_0040c300((undefined4 *)((int)this + 0x88));
    break;
  case 0xffffd949:
    pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar4 = FUN_00485660(pvVar7,0);
    if (iVar4 == 0) {
      local_4c = 0;
      local_24 = local_4c;
    }
    else {
      pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar4 = FUN_00485660(pvVar7,0);
      FUN_00497540(iVar4 + 0x198);
      local_24 = thunk_FUN_00543320();
    }
    break;
  case 0xffffd94a:
    pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
    iVar4 = FUN_00485660(pvVar7,0);
    if (iVar4 == 0) {
      local_50 = 0;
      local_24 = local_50;
    }
    else {
      pvVar7 = (void *)FUN_00412730(*(int *)((int)this + 0x424));
      iVar4 = FUN_00485660(pvVar7,0);
      iVar4 = FUN_0048bd10((void *)(iVar4 + 0x1e0),0);
      FUN_0047a320(iVar4);
      local_24 = thunk_FUN_00543320();
    }
    break;
  case 0xffffd94b:
    local_24 = FUN_0040c300((undefined4 *)((int)this + 0x8c));
    break;
  case 0xffffd94c:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = FUN_00499130(iVar4);
    break;
  case 0xffffd94d:
    local_24 = FUN_00499460(0x5ba568);
    break;
  case 0xffffd94e:
    local_24 = *(uint *)((int)this + 0x354) >> 3 & 1;
    break;
  case 0xffffd94f:
    local_24 = FUN_00499330(0x5ba568);
    break;
  case 0xffffd950:
    iVar4 = FUN_00498f40();
    local_24 = FUN_00499580(iVar4);
    break;
  case 0xffffd955:
    local_24 = DAT_005afcfc;
    break;
  case 0xffffd956:
    iVar4 = FUN_0040c300(*(undefined4 **)((int)this + 0x424));
    local_24 = FUN_004992f0(iVar4);
    break;
  case 0xffffd957:
    local_24 = 0;
    break;
  case 0xffffd958:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = *(uint *)(iVar4 + 0xe0);
    break;
  case 0xffffd959:
    local_24 = DAT_005c49d8;
    break;
  case 0xffffd95a:
    local_24 = DAT_005c49dc;
    break;
  case 0xffffd95b:
    local_24 = DAT_005c49e0;
    break;
  case 0xffffd95c:
    local_24 = DAT_005c49e4;
    break;
  case 0xffffd95f:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = FUN_00499210(iVar4);
    break;
  case 0xffffd961:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = *(uint *)(iVar4 + 0x40);
    break;
  case 0xffffd962:
    iVar4 = FUN_00412730(*(int *)((int)this + 0x424));
    local_24 = *(uint *)(iVar4 + 0x44);
    break;
  case 0xffffd963:
    local_24 = FUN_004640a0(0x5ba568);
    break;
  case 0xffffd964:
    iVar4 = FUN_00460830(1 - *(int *)((int)this + 0x420));
    pfVar5 = (float *)FUN_00460850(iVar4);
    local_24 = (int)*pfVar5;
    break;
  case 0xffffd965:
    iVar4 = FUN_00460830(1 - *(int *)((int)this + 0x420));
    iVar4 = FUN_00460850(iVar4);
    local_24 = (int)*(float *)(iVar4 + 4);
    break;
  case 0xffffd966:
    local_24 = *(uint *)((int)this + 0x420);
    break;
  case 0xffffd967:
    iVar4 = FUN_00464080(0);
    local_24 = FUN_00499520(iVar4);
    break;
  case 0xffffd968:
    iVar4 = FUN_00464080(0);
    iVar4 = FUN_0041cab0(iVar4);
    local_24 = iVar4 / 2;
    break;
  case 0xffffd969:
    local_24 = *(uint *)((int)this + 0x358) >> 6 & 1;
    break;
  case 0xffffd96a:
    uVar2 = FUN_00423ee0(0x5ba4a8);
    if (uVar2 % 2 == 0) {
      local_10 = 1;
      local_24 = local_10;
    }
    else {
      local_10 = 0xffffffff;
      local_24 = local_10;
    }
    break;
  case 0xffffd96b:
    iVar4 = FUN_00478e80(0);
    local_24 = FUN_004992f0(iVar4);
    break;
  case 0xffffd96c:
    iVar4 = FUN_00478ea0(0);
    bVar1 = FUN_004887a0(iVar4);
    local_24 = (uint)bVar1;
    break;
  case 0xffffd96d:
    iVar4 = FUN_00478060(0);
    bVar1 = FUN_0049be20(iVar4);
    local_24 = (uint)bVar1;
    break;
  case 0xffffd96e:
    iVar4 = FUN_00478060(0);
    bVar1 = FUN_0049be20(iVar4);
    if (bVar1) {
      iVar4 = FUN_00478060(0);
      iVar4 = FUN_00498970(iVar4);
      pfVar5 = (float *)FUN_0047a2c0(iVar4);
      pfVar6 = (float *)FUN_0047a2c0((int)this);
      FUN_00456030(pfVar6,pfVar5);
      local_24 = thunk_FUN_00543320();
    }
    else {
      local_24 = 0xffffffff;
    }
  }
  return local_24;
}

