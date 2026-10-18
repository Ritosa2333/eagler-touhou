/* Entry: 0x00502db0; symbol: FUN_00502db0; body bytes: 447 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_00502db0(int param_1)

{
  float *pfVar1;
  default_delete<class_std::_Facet_base> *pdVar2;
  float10 fVar3;
  float fVar4;
  undefined4 local_4c;
  undefined4 local_48;
  void *local_44;
  float local_40;
  float local_3c;
  float local_38;
  void *local_34;
  void *local_30;
  float local_2c;
  void *local_28;
  void *local_24;
  int local_20;
  undefined4 local_1c [3];
  undefined4 local_10;
  undefined4 local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_30 = (void *)(param_1 + 0x50);
  local_20 = param_1;
  FUN_00506050(local_30,DAT_0056ef28);
  local_2c = DAT_0056cd90;
  local_34 = (void *)(local_20 + 0x18);
  local_28 = (void *)FUN_0044c670(local_34,9,0);
  if (local_28 != (void *)0x0) {
    fVar3 = FUN_0047a320(local_20 + 0x50);
    local_3c = (float)fVar3;
    local_38 = local_3c;
    fVar3 = FUN_004379c0((int)local_28);
    local_40 = (float)fVar3;
    FUN_004777f0(local_28,local_3c + local_40);
    fVar3 = FUN_004379c0((int)local_28);
    local_2c = (float)fVar3;
  }
  local_24 = (void *)FUN_004c0e10((undefined4 *)(local_20 + 0xdc));
  if (local_24 != (void *)0x0) {
    FUN_00422e10(local_1c);
    FUN_004398a0(&local_10);
    fVar4 = (float)((uint)local_2c ^ _DAT_0056d7d0) / DAT_0056c8d0;
    pfVar1 = (float *)FUN_004562c0(local_20 + 0x50);
    FUN_004532c0(local_1c,pfVar1,fVar4);
    pdVar2 = std::
             _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
             ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                           *)(local_20 + 0x50));
    FUN_004296e0(local_1c,(float *)pdVar2);
    FUN_004c2030(local_24,local_1c);
    pfVar1 = (float *)FUN_00421830(local_20 + 0x50);
    fVar3 = FUN_004292e0(pfVar1);
    FUN_00506a20(local_24,(float)fVar3);
    FUN_00429250(&local_4c,local_2c,DAT_0056fd48);
    local_10 = local_4c;
    local_c = local_48;
    FUN_00506a50(local_24,&local_10);
    FUN_00506a90(local_24,*(undefined4 *)(local_20 + 0xac));
  }
  local_44 = (void *)(local_20 + 0x1c);
  FUN_004235c0(local_44,0x82);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

