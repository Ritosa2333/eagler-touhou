/* Entry: 0x00474880; symbol: FUN_00474880; body bytes: 1109 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_00474880(void *this,int param_1)

{
  int iVar1;
  int iVar2;
  void *this_00;
  float10 fVar3;
  float10 fVar4;
  short *local_78;
  short *local_74;
  _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
  local_6c [64];
  float local_2c [3];
  float local_20;
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  float local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_74 = *(short **)((int)this + 0x332c);
  FUN_00474780((void *)((int)this + 0x10),param_1);
  FUN_004455c0((int)DAT_005c0028);
  FUN_004ddac0(0x5c4d40);
  FUN_00477520(&DAT_005c4d40,3);
  FUN_00477770(DAT_005c0028,1);
  for (; -1 < *local_74; local_74 = local_74 + 8) {
    iVar1 = *(int *)(*(int *)((int)this + 0x3328) + *local_74 * 4);
    if (*(char *)(iVar1 + 2) == param_1) {
      FUN_00422dd0(local_2c,*(undefined4 *)(local_74 + 2),*(undefined4 *)(local_74 + 4),
                   *(undefined4 *)(local_74 + 6));
      iVar2 = FUN_00473b20(local_2c,*(float *)((int)this + 0x32c4),&DAT_005c53fc);
      if (iVar2 == 0) {
        *(byte *)(iVar1 + 3) = *(byte *)(iVar1 + 3) | 2;
        for (local_78 = (short *)(iVar1 + 0x30); -1 < *local_78;
            local_78 = (short *)FUN_00471160((int)local_78,(int)local_78[1])) {
          this_00 = (void *)(local_78[3] * 0x5e4 + *(int *)((int)this + 0x3320));
          if (*local_78 == 0) {
            fVar3 = FUN_00474d40((int)this_00);
            if (3 < *(byte *)((int)this_00 + 0x498)) {
              std::
              _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                            *)&local_14);
              std::
              _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
              ::_Get_first(local_6c);
              local_14 = *(float *)(local_78 + 4);
              local_10 = *(float *)(local_78 + 6);
              local_c = *(float *)(local_78 + 8);
              if (*(float *)(iVar1 + 0xc) != _DAT_0056e0e8) {
                std::
                _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                              *)&local_20);
                FUN_00472840((undefined4 *)local_6c);
                D3DXMatrixRotationY(local_6c,*(undefined4 *)(iVar1 + 0xc));
                D3DXVec3TransformCoord(&local_20,&local_14,local_6c);
                local_14 = local_20;
                local_10 = local_1c;
                local_c = local_18;
                fVar4 = FUN_00438540((float)fVar3 + *(float *)(iVar1 + 0xc));
                FUN_004504c0(this_00,(float)fVar4);
              }
              *(float *)((int)this_00 + 0x5bc) = local_14 + *(float *)(local_74 + 2);
              *(float *)((int)this_00 + 0x5c0) = local_10 + *(float *)(local_74 + 4);
              *(float *)((int)this_00 + 0x5c4) = local_c + *(float *)(local_74 + 6);
              if (*(float *)(local_78 + 10) != _DAT_0056e0e8) {
                iVar2 = FUN_00437cd0((int)this_00);
                FUN_004777f0(this_00,*(float *)(local_78 + 10) / *(float *)(iVar2 + 0x4c));
              }
              if (*(float *)(local_78 + 0xc) != _DAT_0056e0e8) {
                iVar2 = FUN_00437cd0((int)this_00);
                FUN_004777c0(this_00,*(float *)(local_78 + 0xc) / *(float *)(iVar2 + 0x48));
              }
            }
            if ((*(char *)((int)this_00 + 0x498) == '\b') ||
               (*(char *)((int)this_00 + 0x498) == '\x18')) {
              FUN_004ddac0(0x5c4d40);
            }
            else {
              FUN_004dda60(0x5c4d40);
            }
            if ((*(uint *)((int)this_00 + 0x49c) >> 4 & 1) == 0) {
              FUN_004ddfe0(0x5c4d40);
            }
            else {
              FUN_004ddf80(0x5c4d40);
            }
            FUN_00443880(DAT_005c0028,this_00);
            FUN_004504c0(this_00,(float)fVar3);
            *(int *)((int)this + 0x3340) = *(int *)((int)this + 0x3340) + 1;
          }
        }
        local_74[1] = local_74[1] | 1;
        *(int *)((int)this + 0x3338) = *(int *)((int)this + 0x3338) + 1;
      }
      else {
        *(int *)((int)this + 0x333c) = *(int *)((int)this + 0x333c) + 1;
        local_74[1] = local_74[1] & 0xfffe;
      }
    }
  }
  FUN_004ddf80(0x5c4d40);
  FUN_004750a0(DAT_005c0028);
  FUN_004455c0((int)DAT_005c0028);
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

