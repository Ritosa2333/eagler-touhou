/* Entry: 0x00500380; symbol: FUN_00500380; body bytes: 590 */

void __fastcall FUN_00500380(int param_1)

{
  bool bVar1;
  int iVar2;
  void *this;
  int *piVar3;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Var4;
  int *piVar5;
  uint uVar6;
  default_delete<class_std::_Facet_base> *pdVar7;
  float *pfVar8;
  int *unaff_FS_OFFSET;
  float10 fVar9;
  float local_2c;
  int local_1c [2];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_005683cd;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_2c = DAT_0056fd50;
  iVar2 = FUN_00412730(*(int *)(param_1 + 0x120));
  this = (void *)FUN_004fce40(iVar2);
  FUN_0040c080(local_1c,8);
  FUN_00412280(this,local_1c);
  local_8 = 0;
  piVar3 = (int *)FUN_00412540();
  while( true ) {
    bVar1 = FUN_00411bb0(local_1c,piVar3);
    if (!bVar1) break;
    p_Var4 = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              *)FUN_0040c300(local_1c);
    piVar5 = (int *)FUN_0040f160(p_Var4);
    iVar2 = FUN_004989f0(*piVar5);
    uVar6 = FUN_00506910(iVar2);
    if ((uVar6 & 0xff) == 0) {
      piVar5 = (int *)FUN_0040f160(p_Var4);
      iVar2 = FUN_004989f0(*piVar5);
      uVar6 = FUN_005068f0(iVar2);
      if ((uVar6 & 0xff) == 0) {
        piVar5 = (int *)FUN_0040f160(p_Var4);
        iVar2 = FUN_004989f0(*piVar5);
        uVar6 = FUN_00506930(iVar2);
        if ((uVar6 & 0xff) == 0) {
          piVar5 = (int *)FUN_0040f160(p_Var4);
          iVar2 = FUN_004989f0(*piVar5);
          uVar6 = FUN_005069e0(iVar2);
          if ((uVar6 & 0xff) == 0) {
            piVar5 = (int *)FUN_0040f160(p_Var4);
            iVar2 = FUN_0047a2c0(*piVar5);
            pdVar7 = std::
                     _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                     ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                   *)(param_1 + 0x50));
            if (*(float *)(pdVar7 + 4) - DAT_0056f7b4 <= *(float *)(iVar2 + 4)) {
              piVar5 = (int *)FUN_0040f160(p_Var4);
              pfVar8 = (float *)FUN_0047a2c0(*piVar5);
              pdVar7 = std::
                       _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                       ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                     *)(param_1 + 0x50));
              fVar9 = (float10)FUN_004fe850(*pfVar8 - *(float *)pdVar7);
              if ((float)fVar9 < local_2c) {
                piVar5 = (int *)FUN_0040f160(p_Var4);
                pfVar8 = (float *)FUN_0047a2c0(*piVar5);
                pdVar7 = std::
                         _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                       *)(param_1 + 0x50));
                fVar9 = (float10)FUN_004fe850(*pfVar8 - *(float *)pdVar7);
                local_2c = (float)fVar9;
                piVar5 = (int *)FUN_0040f160(p_Var4);
                pfVar8 = (float *)FUN_0047a2c0(*piVar5);
                pdVar7 = std::
                         _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                         ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                       *)(param_1 + 0x50));
                fVar9 = (float10)FUN_0047a2e0((float *)pdVar7,pfVar8);
                FUN_0047a560((void *)(param_1 + 0x50),(float)fVar9);
              }
            }
          }
        }
      }
    }
    FUN_00411c30(local_1c);
  }
  local_8 = 0xffffffff;
  FUN_00411b00(local_1c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

