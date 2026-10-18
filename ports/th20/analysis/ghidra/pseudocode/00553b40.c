/* Entry: 0x00553b40; symbol: FUN_00553b40; body bytes: 809 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

bool __cdecl FUN_00553b40(uint param_1,double *param_2,uint param_3)

{
  char cVar1;
  byte bVar2;
  int iVar3;
  uint uVar4;
  uint uVar5;
  float10 fVar6;
  double dVar7;
  uint local_2c;
  byte bStack_28;
  undefined1 uStack_27;
  ushort uStack_26;
  int local_20;
  undefined8 local_1c;
  uint local_14;
  undefined4 local_10;
  uint local_c;
  char local_5;
  
  bVar2 = 0;
  uVar5 = param_1 & 0x1f;
  local_c = uVar5;
  if (((param_1 & 8) != 0) && ((param_3 & 1) != 0)) {
    bVar2 = 1;
    uVar5 = param_1 & 0x17;
    goto LAB_00553e3f;
  }
  if ((param_1 & param_3 & 4) != 0) {
    bVar2 = 4;
    uVar5 = param_1 & 0x1b;
    goto LAB_00553e3f;
  }
  if (((param_1 & 1) == 0) || ((param_3 & 8) == 0)) {
    if (((param_1 & 2) == 0) || ((param_3 & 0x10) == 0)) goto LAB_00553e3f;
    uVar4 = param_1 >> 4 & 1;
    dVar7 = *param_2;
    if (dVar7 == _DAT_0056cd50) {
      uVar4 = 1;
    }
    else {
      fVar6 = FUN_005523a0(SUB84(dVar7,0),(uint)((ulonglong)dVar7 >> 0x20),&local_20);
      dVar7 = (double)fVar6;
      local_20 = local_20 + -0x600;
      local_2c = SUB84(dVar7,0);
      bStack_28 = (byte)((ulonglong)dVar7 >> 0x20);
      uStack_27 = (undefined1)((ulonglong)dVar7 >> 0x28);
      uStack_26 = (ushort)((ulonglong)dVar7 >> 0x30);
      local_1c = (double)CONCAT44(local_20,local_2c);
      if (local_20 < -0x432) {
        uVar4 = 1;
        *param_2 = dVar7 * 0.0;
      }
      else {
        local_14 = (uint)(dVar7 < 0.0);
        local_5 = '\0';
        uStack_26 = uStack_26 & 0xf | 0x10;
        cVar1 = '\0';
        local_10 = 0;
        if (local_20 < -0x3fd) {
          local_20 = -0x3fd - local_20;
          do {
            cVar1 = (char)(local_2c & 1);
            local_1c._4_4_ = 1;
            if (((local_2c & 1) != 0) && (uVar4 == 0)) {
              uVar4 = local_1c._4_4_;
            }
            if (local_5 != '\0') {
              local_10 = 1;
            }
            local_2c = local_2c >> 1;
            if ((bStack_28 & 1) != 0) {
              local_2c = local_2c | 0x80000000;
            }
            uVar5 = CONCAT22(uStack_26,CONCAT11(uStack_27,bStack_28)) >> 1;
            bStack_28 = (byte)uVar5;
            uStack_27 = (undefined1)(uVar5 >> 8);
            uStack_26 = uStack_26 >> 1;
            local_20 = local_20 + -1;
            local_5 = cVar1;
          } while (local_20 != 0);
          cVar1 = (char)local_10;
        }
        local_1c = (double)CONCAT26(uStack_26,CONCAT15(uStack_27,CONCAT14(bStack_28,local_2c)));
        if (local_14 != 0) {
          local_2c = local_2c ^ _DAT_00597070;
          uVar5 = (uint)((ulonglong)local_1c >> 0x20) ^ _UNK_00597074;
          local_1c = (double)CONCAT44(uVar5,local_2c);
          bStack_28 = (byte)uVar5;
          uStack_27 = (undefined1)(uVar5 >> 8);
          uStack_26 = (ushort)(uVar5 >> 0x10);
        }
        if ((local_5 != '\0') || (dVar7 = local_1c, cVar1 != '\0')) {
          iVar3 = FUN_00559a20();
          dVar7 = local_1c;
          if (iVar3 < 0x201) {
            bVar2 = (byte)local_14;
            if (iVar3 == 0x200) {
              bVar2 = bVar2 ^ 1;
LAB_00553e06:
              if (bVar2 != 0) {
LAB_00553e08:
                iVar3 = CONCAT22(uStack_26,CONCAT11(uStack_27,bStack_28)) +
                        (uint)(0xfffffffe < local_2c);
                bStack_28 = (byte)iVar3;
                uStack_27 = (undefined1)((uint)iVar3 >> 8);
                uStack_26 = (ushort)((uint)iVar3 >> 0x10);
                dVar7 = (double)CONCAT26(uStack_26,
                                         CONCAT15(uStack_27,CONCAT14(bStack_28,local_2c + 1)));
              }
            }
            else if (iVar3 == 0) {
              if (local_5 != '\0') {
                if ((char)local_10 == '\0') {
                  bVar2 = (byte)local_2c & 1;
                  goto LAB_00553e06;
                }
                goto LAB_00553e08;
              }
            }
            else if (iVar3 == 0x100) goto LAB_00553e06;
          }
        }
        *param_2 = dVar7;
        uVar5 = local_c;
      }
    }
    uVar5 = uVar5 & 0xfffffffd;
    bVar2 = -(uVar4 != 0) & 0x10;
    goto LAB_00553e3f;
  }
  bVar2 = 8;
  uVar5 = param_3 & 0xc00;
  if (uVar5 < 0x801) {
    if (uVar5 == 0x800) {
      if (_DAT_0056cd50 < *param_2) {
        *param_2 = DAT_005989c0;
        uVar5 = param_1 & 0x1e;
        goto LAB_00553e3f;
      }
LAB_00553c7d:
      dVar7 = (double)CONCAT44((uint)((ulonglong)DAT_005989c8 >> 0x20) ^ _UNK_00597074,
                               SUB84(DAT_005989c8,0) ^ _DAT_00597070);
    }
    else {
      uVar4 = (uint)((ulonglong)DAT_005989c0 >> 0x20);
      if (uVar5 != 0) {
        if (uVar5 == 0x400) {
          if (*param_2 <= _DAT_0056cd50) {
            *param_2 = (double)CONCAT44(uVar4 ^ _UNK_00597074,SUB84(DAT_005989c0,0) ^ _DAT_00597070)
            ;
            uVar5 = param_1 & 0x1e;
          }
          else {
            *param_2 = DAT_005989c8;
            uVar5 = param_1 & 0x1e;
          }
          goto LAB_00553e3f;
        }
        goto LAB_00553c90;
      }
      dVar7 = DAT_005989c0;
      if (*param_2 <= _DAT_0056cd50) {
        *param_2 = (double)CONCAT44(uVar4 ^ _UNK_00597074,SUB84(DAT_005989c0,0) ^ _DAT_00597070);
        uVar5 = param_1 & 0x1e;
        goto LAB_00553e3f;
      }
    }
    *param_2 = dVar7;
  }
  else if (uVar5 == 0xc00) {
    if (_DAT_0056cd50 < *param_2) {
      *param_2 = DAT_005989c8;
      uVar5 = param_1 & 0x1e;
      goto LAB_00553e3f;
    }
    goto LAB_00553c7d;
  }
LAB_00553c90:
  uVar5 = param_1 & 0x1e;
LAB_00553e3f:
  if (((param_1 & 0x10) != 0) && ((param_3 & 0x20) != 0)) {
    bVar2 = bVar2 | 0x20;
    uVar5 = uVar5 & 0xffffffef;
  }
  if (bVar2 != 0) {
    FUN_0055dc10();
  }
  return uVar5 == 0;
}

