/* Entry: 0x004c3c90; symbol: FUN_004c3c90; body bytes: 1571 */

void * __thiscall
FUN_004c3c90(void *this,int param_1,float *param_2,undefined4 param_3,float param_4,float param_5,
            int param_6,undefined4 param_7,undefined4 param_8)

{
  char cVar1;
  int iVar2;
  float *pfVar3;
  uint uVar4;
  undefined4 *puVar5;
  float10 fVar6;
  undefined4 uVar7;
  float fVar8;
  float fVar9;
  undefined4 uVar10;
  undefined4 uVar11;
  undefined1 local_58 [12];
  void *local_4c;
  void *local_48;
  int local_44;
  void *local_40;
  int local_3c;
  void *local_38;
  int local_34;
  int local_30;
  void *local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  void *local_18;
  int local_14;
  void *local_10;
  void *local_c;
  void *local_8;
  
  local_c = this;
  if (param_1 == 1) {
    iVar2 = FUN_00464080(0);
    cVar1 = FUN_004c4f50(iVar2);
    if (cVar1 != '\0') {
      *(int *)((int)local_c + 0x49c878) = *(int *)((int)local_c + 0x49c878) + 1;
      local_14 = 10;
      iVar2 = FUN_00464080(0);
      cVar1 = FUN_00464400(iVar2);
      if (cVar1 != '\0') {
        local_14 = 0xe;
      }
      if (*(int *)((int)local_c + 0x49c878) % local_14 == local_14 + -1) {
        iVar2 = FUN_00464230(0);
        local_18 = (void *)FUN_004859d0(iVar2);
        FUN_004865b0(local_18,1);
        fVar6 = FUN_004298e0(0x5ba4a8);
        local_1c = (float)fVar6;
        local_24 = local_1c * DAT_0056cd90 + param_2[1];
        fVar6 = FUN_004298e0(0x5ba4a8);
        local_20 = (float)fVar6;
        local_28 = local_20 * DAT_0056cd90 + *param_2;
        iVar2 = param_6 + 0x10;
        uVar7 = param_3;
        fVar8 = param_4;
        fVar9 = param_5;
        uVar10 = param_7;
        uVar11 = param_8;
        pfVar3 = (float *)FUN_00422dd0(local_58,local_28,local_24,0);
        FUN_004c3c90(local_c,1,pfVar3,uVar7,fVar8,fVar9,iVar2,uVar10,uVar11);
      }
    }
  }
  if (param_1 < 0x10) {
    *(int *)((int)local_c + 0x49c868) = *(int *)((int)local_c + 0x49c868) + 1;
    if ((((param_1 == 9) || (param_1 == 10)) || (param_1 == 0xb)) ||
       ((param_1 == 0xc || (param_1 == 0xd)))) {
      local_8 = (void *)FUN_004859f0((int)local_c + 0x49c848);
      if (local_8 != (void *)0x0) {
        *(undefined4 *)((int)local_8 + 0xc38) = *(undefined4 *)((int)local_c + 0x49c874);
        local_10 = local_8;
        FUN_00411ce0((int)local_8);
        local_2c = (void *)((int)local_c + 0x49c818);
        FUN_0044a6b0(local_2c,local_10);
        if (*(int *)((int)local_c + 0x49c870) < 0x400) {
          if (*(int *)((int)local_c + 0x49c870) < 0x200) {
            if (*(int *)((int)local_c + 0x49c870) < 0x100) {
              uVar4 = *(uint *)((int)local_c + 0x49c868) & 0x80000003;
              if ((int)uVar4 < 0) {
                uVar4 = (uVar4 - 1 | 0xfffffffc) + 1;
              }
              *(uint *)((int)local_10 + 0xc34) = uVar4;
            }
            else {
              uVar4 = *(uint *)((int)local_c + 0x49c868) & 0x80000007;
              if ((int)uVar4 < 0) {
                uVar4 = (uVar4 - 1 | 0xfffffff8) + 1;
              }
              *(uint *)((int)local_10 + 0xc34) = uVar4 + 4;
            }
          }
          else {
            uVar4 = *(uint *)((int)local_c + 0x49c868) & 0x8000000f;
            if ((int)uVar4 < 0) {
              uVar4 = (uVar4 - 1 | 0xfffffff0) + 1;
            }
            *(uint *)((int)local_10 + 0xc34) = uVar4 + 8;
          }
        }
        else {
          uVar4 = *(uint *)((int)local_c + 0x49c868) & 0x8000001f;
          if ((int)uVar4 < 0) {
            uVar4 = (uVar4 - 1 | 0xffffffe0) + 1;
          }
          *(uint *)((int)local_10 + 0xc34) = uVar4 + 0x10;
        }
        *(undefined4 *)((int)local_10 + 0xc24) = 5;
        *(int *)((int)local_10 + 0xc28) = param_1;
        *(undefined4 *)((int)local_10 + 0xc2c) = 0;
        *(float *)((int)local_10 + 0xbe4) = *param_2;
        *(float *)((int)local_10 + 0xbe8) = param_2[1];
        *(float *)((int)local_10 + 0xbec) = param_2[2];
        *(undefined4 *)((int)local_10 + 0xbec) = 0;
        FUN_00439330(local_10,(float *)((int)local_10 + 0xbf0),param_4,param_5);
        *(undefined4 *)((int)local_10 + 0xbf8) = 0;
        FUN_00423520((void *)((int)local_10 + 0xc04),0);
        *(float *)((int)local_10 + 0xc00) = param_4;
        *(float *)((int)local_10 + 0xbfc) = param_5;
        *(undefined4 *)((int)local_10 + 0xc3c) = param_7;
        *(undefined4 *)((int)local_10 + 0xc40) = param_8;
        FUN_004c4f90(local_10,*(undefined4 *)((int)local_c + 0x49c894));
        local_8 = local_10;
      }
    }
    else {
      if (param_1 == 0xf) {
        iVar2 = FUN_00498f40();
        local_34 = FUN_004640c0(iVar2);
        local_34 = local_34 + 1;
        local_30 = *(int *)((int)local_c + 0x49c86c);
        *(int *)((int)local_c + 0x49c86c) = local_30 + local_34;
        if (*(int *)((int)local_c + 0x49c86c) < 10) {
          return (void *)0x0;
        }
        *(undefined4 *)((int)local_c + 0x49c86c) = 0;
        param_1 = 2;
      }
      puVar5 = (undefined4 *)FUN_004859f0((int)(FUN_0049c830 + (int)local_c));
      local_8 = (void *)FUN_0040c300(puVar5);
      if (local_8 != (void *)0x0) {
        FUN_00411ce0((int)local_8);
        local_38 = (void *)((int)local_c + 0x49c818);
        FUN_0044a6b0(local_38,local_8);
        *(undefined4 *)((int)local_8 + 0xc24) = 1;
        *(float *)((int)local_8 + 0xbe4) = *param_2;
        *(float *)((int)local_8 + 0xbe8) = param_2[1];
        *(float *)((int)local_8 + 0xbec) = param_2[2];
        *(undefined4 *)((int)local_8 + 0xbec) = 0;
        if (DAT_0056fa6c < *(float *)((int)local_8 + 0xbe4)) {
          if (DAT_0056e050 <= *(float *)((int)local_8 + 0xbe4)) {
            *(float *)((int)local_8 + 0xbe4) = DAT_0056e050;
          }
        }
        else {
          *(float *)((int)local_8 + 0xbe4) = DAT_0056fa6c;
        }
        if (param_1 == 0xe) {
          param_1 = 6;
        }
        FUN_00439330((void *)((int)local_8 + 0xbf0),(float *)((int)local_8 + 0xbf0),param_4,param_5)
        ;
        *(undefined4 *)((int)local_8 + 0xbf8) = 0;
        FUN_00423520((void *)((int)local_8 + 0xc04),0);
        *(undefined4 *)((int)local_8 + 0xbfc) = 0;
        *(undefined4 *)((int)local_8 + 0xc30) = 0;
        *(int *)((int)local_8 + 0xc34) = param_6;
        *(int *)((int)local_8 + 0xc28) = param_1;
        *(undefined4 *)((int)local_8 + 0xc40) = param_8;
        FUN_004c4f90(local_8,*(undefined4 *)((int)local_c + 0x49c894));
        if (param_6 == 0) {
          FUN_004c42c0((int)local_8);
        }
        *(undefined4 *)((int)local_8 + 0xc2c) = 0;
        iVar2 = FUN_0040c300(*(undefined4 **)((int)local_c + 0x49c898));
        local_40 = *(void **)(iVar2 + 0x286d8c);
        local_3c = *(int *)(param_1 * 8 + 0x5afd50);
        FUN_00438380(local_40,(void *)((int)local_8 + 0x18),local_3c);
        iVar2 = FUN_0040c300(*(undefined4 **)((int)local_c + 0x49c898));
        local_48 = *(void **)(iVar2 + 0x286d8c);
        local_44 = *(int *)(param_1 * 8 + 0x5afd54);
        FUN_00438380(local_48,(void *)((int)local_8 + 0x5fc),local_44);
        FUN_004117a0((void *)((int)local_8 + 0xbe0),0);
        local_4c = (void *)((int)local_8 + 0x18);
        FUN_0044fa10(local_4c,param_3);
        *(undefined4 *)((int)local_8 + 0xc3c) = param_7;
      }
    }
  }
  else {
    local_8 = (void *)0x0;
  }
  return local_8;
}

