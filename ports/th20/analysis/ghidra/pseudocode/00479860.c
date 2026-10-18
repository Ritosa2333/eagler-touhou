/* Entry: 0x00479860; symbol: FUN_00479860; body bytes: 1456 */

void __fastcall FUN_00479860(int param_1)

{
  bool bVar1;
  bool bVar2;
  void *pvVar3;
  undefined4 *puVar4;
  int iVar5;
  undefined4 *puVar6;
  undefined3 extraout_var;
  float *pfVar7;
  default_delete<class_std::_Facet_base> *pdVar8;
  uint uVar9;
  uint uVar10;
  float10 fVar11;
  undefined4 uVar12;
  int local_34;
  uint local_2c;
  int local_28;
  int local_24;
  undefined4 *local_20;
  undefined4 *local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  pvVar3 = (void *)FUN_00460830(0);
  FUN_00479060(pvVar3,0x28);
  puVar4 = (undefined4 *)(param_1 + 0xb8);
  iVar5 = FUN_00460830(0);
  puVar6 = (undefined4 *)FUN_00460850(iVar5);
  FUN_004645e0((void *)(param_1 + 0x74),puVar6);
  bVar1 = FUN_004235c0((void *)(param_1 + 8),0x78);
  if (bVar1) {
    bVar1 = false;
    for (local_20 = puVar4; local_20 != (undefined4 *)(param_1 + 0x14f8); local_20 = local_20 + 0x36
        ) {
      bVar2 = FUN_0047a340(local_20);
      if (bVar2) {
        bVar1 = true;
        break;
      }
    }
    if (!bVar1) {
      FUN_00479040((void *)(param_1 + 0x74));
      goto LAB_00479e02;
    }
  }
  bVar1 = FUN_004785e0((void *)(param_1 + 8),0xf0);
  if (bVar1) {
    FUN_00479f90((int)puVar4);
    FUN_00479040((void *)(param_1 + 0x74));
  }
  else {
    bVar1 = FUN_0045d030((int *)(param_1 + 8));
    if (CONCAT31(extraout_var,bVar1) != 0) {
      bVar1 = FUN_004785e0((void *)(param_1 + 8),0);
      if (bVar1) {
        FUN_00429210(&local_c,0.0);
        local_14 = puVar4;
        for (local_24 = 0; local_24 < 8; local_24 = local_24 + 1) {
          uVar12 = 0xf;
          iVar5 = FUN_00460830(0);
          puVar6 = (undefined4 *)FUN_00460850(iVar5);
          FUN_0047a090(local_14,local_24,puVar6,uVar12);
          FUN_0047a4c0((int)(local_14 + 1));
          FUN_0047a540(local_14 + 1,0);
          fVar11 = FUN_004292e0(&local_c);
          FUN_0047a560(local_14 + 1,(float)fVar11);
          FUN_0047a520(local_14 + 1,DAT_0056e0f0 / DAT_0056fa28);
          local_14[0x35] = DAT_0056e0f0 / DAT_0056f280;
          FUN_004530c0(&local_c,(DAT_0056e0f0 * DAT_0056c8d0) / DAT_0056d7bc);
          local_14 = local_14 + 0x36;
        }
      }
      else {
        bVar1 = FUN_004785e0((void *)(param_1 + 8),0x28);
        if (bVar1) {
          FUN_00429210(&local_10,0.0);
          local_14 = (undefined4 *)(param_1 + 0x778);
          iVar5 = FUN_00460830(0);
          pfVar7 = (float *)FUN_00460850(iVar5);
          FUN_00426eb0(&DAT_005ba830,0x2c,*pfVar7);
          for (local_28 = 0; local_28 < 8; local_28 = local_28 + 1) {
            uVar12 = 7;
            iVar5 = FUN_00460830(0);
            puVar6 = (undefined4 *)FUN_00460850(iVar5);
            FUN_0047a090(local_14,local_28,puVar6,uVar12);
            FUN_0047a4c0((int)local_14 + 4);
            FUN_0047a540((void *)((int)local_14 + 4),0);
            fVar11 = FUN_004292e0(&local_10);
            FUN_0047a560((void *)((int)local_14 + 4),(float)fVar11);
            FUN_0047a520((void *)((int)local_14 + 4),DAT_0056e0f8 / DAT_0056fa28);
            *(float *)((int)local_14 + 0xd4) = DAT_0056e0f8 / DAT_0056f280;
            FUN_004530c0(&local_10,(DAT_0056e0f0 * DAT_0056c8d0) / DAT_0056d7bc);
            local_14 = (undefined4 *)((int)local_14 + 0xd8);
          }
        }
      }
    }
    local_14 = puVar4;
    for (local_34 = 0; local_34 < 0x18; local_34 = local_34 + 1) {
      if (local_14[0x28] != 0) {
        FUN_00479360();
        iVar5 = FUN_004c0e10(local_14 + 0x33);
        if (iVar5 != 0) {
          iVar5 = FUN_004c0e10(local_14 + 0x33);
          iVar5 = FUN_0045bcf0(iVar5);
          if (iVar5 < 300) {
            pdVar8 = std::
                     _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                     ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                   *)(local_14 + 1));
            FUN_004c1ff0(local_14 + 0x33,(undefined4 *)pdVar8);
          }
          else {
            FUN_00479e10(local_14);
            pdVar8 = std::
                     _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                     ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                   *)(local_14 + 1));
            FUN_00426eb0(&DAT_005ba830,0x1b,*(float *)pdVar8);
            __aligned_offset_recalloc((void *)0x1,4,6,6,0);
          }
        }
      }
      local_14 = local_14 + 0x36;
    }
    local_14 = (undefined4 *)(param_1 + 0xb8);
    for (local_2c = 0; (int)local_2c < 0x18; local_2c = local_2c + 1) {
      if (*(int *)((int)local_14 + 0xa0) != 0) {
        uVar9 = FUN_00472040((void *)(param_1 + 8),8);
        uVar10 = local_2c & 0x80000007;
        if ((int)uVar10 < 0) {
          uVar10 = (uVar10 - 1 | 0xfffffff8) + 1;
        }
        if (uVar9 == uVar10) {
          pvVar3 = (void *)FUN_00478e80(0);
          pdVar8 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)local_14 + 4));
          FUN_0047cf60(pvVar3,(float *)pdVar8,DAT_0056fa28,0,99999,0);
          pvVar3 = (void *)FUN_00478ee0(0);
          pdVar8 = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)((int)local_14 + 4));
          FUN_004caad0(pvVar3,(undefined4 *)pdVar8,DAT_0056fa28,0,1);
        }
      }
      local_14 = (undefined4 *)((int)local_14 + 0xd8);
    }
  }
  FUN_00423540((undefined4 *)(param_1 + 8));
LAB_00479e02:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

