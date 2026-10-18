
void effect_467430(void)

{
  bool bVar1;
  undefined3 extraout_var;
  undefined4 *puVar2;
  float *pfVar3;
  int iVar4;
  int in_ECX;
  void *extraout_ECX;
  float10 fVar5;
  undefined1 auStack_bc [8];
  float afStack_b4 [2];
  undefined1 auStack_ac [8];
  undefined4 uStack_a4;
  undefined1 auStack_a0 [4];
  undefined1 auStack_9c [4];
  void *pvStack_98;
  void *pvStack_94;
  undefined4 uStack_90;
  undefined4 uStack_8c;
  undefined4 *puStack_88;
  default_delete<class_std::_Facet_base> *pdStack_84;
  void *pvStack_80;
  void *pvStack_7c;
  void *pvStack_78;
  float fStack_74;
  float fStack_70;
  float fStack_6c;
  float fStack_68;
  void *pvStack_64;
  void *pvStack_60;
  int iStack_5c;
  void *pvStack_58;
  void *pvStack_54;
  void *pvStack_50;
  char *pcStack_4c;
  char *pcStack_48;
  void *pvStack_44;
  default_delete<class_std::_Facet_base> *pdStack_40;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  *p_Stack_3c;
  float fStack_38;
  float fStack_34;
  float fStack_30;
  default_delete<class_std::_Facet_base> *pdStack_2c;
  int iStack_28;
  float afStack_24 [2];
  float afStack_1c [2];
  uint uStack_14;
  
  uStack_14 = DAT_005b25c0 ^ (uint)&stack0xfffffff0;
  pvStack_94 = (void *)(in_ECX + 0x178);
  iStack_28 = in_ECX;
  bVar1 = FUN_00423560(pvStack_94,0x3c);
  if (bVar1) {
    bVar1 = FUN_0045d030((int *)(iStack_28 + 0x178));
    if (CONCAT31(extraout_var,bVar1) != 0) {
      pvStack_98 = (void *)(iStack_28 + 8);
      puVar2 = (undefined4 *)FUN_00467c50(pvStack_98,auStack_9c);
      pcStack_48 = (char *)*puVar2;
      pvStack_44 = (void *)(iStack_28 + 8);
      puVar2 = (undefined4 *)begin(pvStack_44,auStack_a0);
      pcStack_4c = (char *)*puVar2;
      FUN_00467230(&uStack_a4,pcStack_4c,pcStack_48,1);
      pvStack_50 = (void *)(iStack_28 + 0x178);
      bVar1 = FUN_00423590(pvStack_50,0x1e);
      if (bVar1) {
        pvStack_54 = (void *)(iStack_28 + 0x18c);
        pfVar3 = (float *)FUN_004673b0(pvStack_54,auStack_ac,30.0);
        FUN_00439960((void *)(iStack_28 + 0x194),afStack_b4,pfVar3);
      }
      FUN_00429250(afStack_24,*(undefined4 *)(iStack_28 + 400),*(undefined4 *)(iStack_28 + 0x18c));
      FUN_004398a0(afStack_1c);
      pvStack_58 = (void *)(iStack_28 + 0x178);
      bVar1 = FUN_00423560(pvStack_58,6);
      if (bVar1) {
        iStack_5c = iStack_28 + 0x178;
        fVar5 = FUN_00467380(iStack_5c,6.0);
        fStack_38 = (float)fVar5;
      }
      else {
        pvStack_60 = (void *)(iStack_28 + 0x178);
        bVar1 = FUN_00461070(pvStack_60,0x1e);
        if (bVar1) {
          fStack_34 = 0.0;
        }
        else {
          pvStack_64 = (void *)(iStack_28 + 0x178);
          bVar1 = FUN_004235c0(pvStack_64,0x18);
          if (bVar1) {
            iVar4 = FUN_0040ff90(iStack_28 + 0x178);
            fStack_30 = (float)(0x1e - iVar4) / 6.0;
          }
          else {
            fStack_30 = 1.0;
          }
          fStack_34 = fStack_30;
        }
        fStack_38 = fStack_34;
      }
      fStack_6c = fStack_38;
      fVar5 = FUN_004292e0((float *)(iStack_28 + 0x170));
      fVar5 = FUN_00439820(extraout_ECX,(float)fVar5);
      fStack_68 = (float)fVar5;
      fStack_74 = fStack_68 * *(float *)(iStack_28 + 0x174) * fStack_6c;
      fVar5 = FUN_00438290(0x5b6758);
      fStack_70 = (float)fVar5;
      FUN_004678c0(afStack_1c,afStack_24,fStack_74 * fStack_70 * 0.5);
      pvStack_78 = (void *)(iStack_28 + 0x178);
      bVar1 = FUN_00423590(pvStack_78,0x1e);
      if (bVar1) {
        FUN_004530c0((void *)(iStack_28 + 0x170),0.20943952);
      }
      pvStack_7c = (void *)(iStack_28 + 0x194);
      puVar2 = (undefined4 *)FUN_00429640(pvStack_7c,auStack_bc,afStack_1c);
      uStack_90 = *puVar2;
      uStack_8c = puVar2[1];
      pvStack_80 = (void *)(iStack_28 + 8);
      puVar2 = (undefined4 *)FUN_0045d670(pvStack_80,0);
      *puVar2 = uStack_90;
      puVar2[1] = uStack_8c;
    }
    iVar4 = FUN_00460830(0);
    pfVar3 = (float *)FUN_00460850(iVar4);
    if (*pfVar3 <= -168.0 && *pfVar3 != -168.0) {
      iVar4 = FUN_00460830(0);
      iVar4 = FUN_00460850(iVar4);
      if (320.0 < *(float *)(iVar4 + 4)) {
        p_Stack_3c = (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                      *)(iStack_28 + 0xf8);
        pdStack_2c = std::
                     _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                     ::_Get_first(p_Stack_3c);
        pdStack_84 = (default_delete<class_std::_Facet_base> *)FUN_00466fb0((int)p_Stack_3c);
        for (; pdStack_2c != pdStack_84; pdStack_2c = pdStack_2c + 4) {
          pdStack_40 = pdStack_2c;
          if (0x1f < (byte)pdStack_2c[3]) {
            pdStack_2c[3] = (default_delete<class_std::_Facet_base>)0x20;
          }
        }
      }
    }
    puStack_88 = (undefined4 *)(iStack_28 + 0x178);
    FUN_00423540(puStack_88);
  }
  FUN_005429ee(uStack_14 ^ (uint)&stack0xfffffff0);
  return;
}

