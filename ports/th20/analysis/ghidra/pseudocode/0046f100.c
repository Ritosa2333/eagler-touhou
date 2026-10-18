/* Entry: 0x0046f100; symbol: FUN_0046f100; body bytes: 2615 */

undefined8 __cdecl
FUN_0046f100(uint *param_1,uint *param_2,undefined4 param_3,undefined4 param_4,size_t param_5)

{
  char cVar1;
  char cVar2;
  uint *puVar3;
  bool bVar4;
  int iVar5;
  int iVar6;
  uint *puVar7;
  uint uVar8;
  uint uVar9;
  undefined4 uVar10;
  bool bVar11;
  longlong lVar12;
  ulonglong uVar13;
  longlong lVar14;
  uint local_80;
  int local_7c;
  uint local_68;
  uint local_60;
  uint local_4c;
  uint local_48;
  uint local_3c;
  uint local_34;
  uint local_2c;
  uint *local_28;
  int local_20;
  undefined1 local_1a;
  uint local_18;
  int local_14;
  uint local_c;
  uint local_8;
  
  puVar3 = param_1;
  lVar12 = FUN_0046fc30();
  uVar8 = (uint)((ulonglong)lVar12 >> 0x20);
  if (lVar12 == 0) {
    local_80 = (uint)(param_5 != 0);
    if ((int)param_2 - (int)param_1 < (int)(local_80 + 5 + param_5)) {
      uVar10 = 0x84;
    }
    else {
      *(undefined1 *)param_1 = 0x30;
      puVar3 = (uint *)((int)param_1 + 1);
      if (param_5 != 0) {
        *(undefined1 *)((int)param_1 + 1) = 0x2e;
        _memset((void *)((int)param_1 + 2),0x30,param_5);
        puVar3 = (uint *)((int)param_1 + 2 + param_5);
      }
      param_1 = puVar3;
      FUN_00543e20(param_1,(uint *)&DAT_0056f570,4);
      param_2 = param_1 + 1;
      uVar10 = 0;
    }
  }
  else {
    local_3c = uVar8 & 0xfffff;
    uVar13 = __aullshr(0x34,uVar8);
    if ((int)uVar13 == 0) {
      local_20 = -0x432;
    }
    else {
      local_20 = (int)uVar13 + -0x433;
      local_3c = local_3c | 0x100000;
    }
    bVar11 = param_5 == 0;
    uVar8 = param_5 + 1;
    local_c = 0;
    local_18 = 0;
    local_8 = 0;
    local_14 = 0;
    if (-0x35 < local_20) {
      if (local_20 < 0) {
        local_60 = 0;
      }
      else {
        local_60 = FUN_0046fc70(local_20);
      }
      iVar5 = FUN_0046ffc0(local_60);
      local_2c = FUN_0046fc80(local_60);
      while( true ) {
        local_2c = local_2c - 1;
        local_8 = 0;
        if ((int)local_2c < 0) break;
        iVar6 = (iVar5 - local_20) + 8;
        puVar7 = (uint *)(&DAT_0057c5e0 +
                         (*(ushort *)(&DAT_0056f418 + local_60 * 2) + local_2c) * 0x18);
        lVar14 = __allshl(8,local_3c);
        local_c = FUN_0046fce0((uint)lVar14,(uint)((ulonglong)lVar14 >> 0x20),puVar7,iVar6);
        if (local_18 == 0) {
          if (local_c != 0) {
            local_8 = FUN_0046fb40(local_c);
            local_14 = local_2c * 9 + -1 + local_8;
            if (uVar8 < local_8) break;
            local_18 = local_8;
            if (bVar11) {
              if (param_1 == param_2) {
                uVar10 = 0x84;
                goto LAB_0046fb33;
              }
              *(char *)param_1 = (char)local_c + '0';
              param_1 = (uint *)((int)param_1 + 1);
            }
            else {
              if ((int)param_2 - (int)param_1 < (int)(local_8 + 1)) {
                uVar10 = 0x84;
                goto LAB_0046fb33;
              }
              FUN_0046ef80(local_8,local_c,(char *)param_1);
              param_1 = (uint *)((int)param_1 + local_8 + 1);
            }
          }
        }
        else {
          if (uVar8 < local_18 + 9) {
            local_8 = 9;
            break;
          }
          if ((int)param_2 - (int)param_1 < 9) {
            uVar10 = 0x84;
            goto LAB_0046fb33;
          }
          FUN_00469640(local_c,(char *)param_1);
          param_1 = (uint *)((int)param_1 + 9);
          local_18 = local_18 + 9;
        }
      }
    }
    if ((local_20 < 0) && (local_8 == 0)) {
      iVar5 = (int)(-local_20 + (-local_20 >> 0x1f & 0xfU)) >> 4;
      for (local_34 = (uint)(byte)(&UNK_0056f528)[iVar5]; (int)local_34 < 200;
          local_34 = local_34 + 1) {
        uVar9 = (*(ushort *)(&UNK_0056f498 + iVar5 * 2) + local_34) -
                (uint)(byte)(&UNK_0056f528)[iVar5];
        if (uVar9 < *(ushort *)(&UNK_0056f49a + iVar5 * 2)) {
          iVar6 = (iVar5 * -0x10 - local_20) + 0x80;
          puVar7 = (uint *)(&DAT_005838a0 + uVar9 * 0x18);
          lVar14 = __allshl(8,local_3c);
          local_68 = FUN_0046fce0((uint)lVar14,(uint)((ulonglong)lVar14 >> 0x20),puVar7,iVar6);
        }
        else {
          local_68 = 0;
        }
        local_c = local_68;
        if (local_18 == 0) {
          if (local_68 != 0) {
            uVar9 = FUN_0046fb40(local_68);
            local_14 = (local_34 + 1) * -9 + -1 + uVar9;
            local_8 = uVar9;
            if (uVar8 < uVar9) break;
            if (bVar11) {
              if (param_1 == param_2) {
                uVar10 = 0x84;
                goto LAB_0046fb33;
              }
              *(char *)param_1 = (char)local_68 + '0';
              param_1 = (uint *)((int)param_1 + 1);
            }
            else {
              if ((int)param_2 - (int)param_1 < (int)(uVar9 + 1)) {
                uVar10 = 0x84;
                goto LAB_0046fb33;
              }
              FUN_0046ef80(uVar9,local_68,(char *)param_1);
              param_1 = (uint *)((int)param_1 + uVar9 + 1);
            }
            local_8 = 0;
            local_18 = uVar9;
          }
        }
        else {
          if (uVar8 < local_18 + 9) {
            local_8 = 9;
            break;
          }
          if ((int)param_2 - (int)param_1 < 9) {
            uVar10 = 0x84;
            goto LAB_0046fb33;
          }
          FUN_00469640(local_68,(char *)param_1);
          param_1 = (uint *)((int)param_1 + 9);
          local_18 = local_18 + 9;
        }
      }
    }
    uVar9 = uVar8 - local_18;
    if (local_8 == 0) {
      local_c = 0;
    }
    local_4c = 0;
    if (uVar9 < local_8) {
      for (local_48 = 0; local_48 < local_8 - uVar9; local_48 = local_48 + 1) {
        local_4c = local_c % 10;
        local_c = local_c / 10;
      }
    }
    if (local_4c == 5) {
      iVar5 = uVar8 - local_14;
      local_20 = -iVar5 - local_20;
      if ((local_20 < 1) ||
         ((local_20 < 0x3c && (bVar4 = FUN_0046ff30((uint)lVar12,local_3c,(byte)local_20), bVar4))))
      {
        bVar4 = true;
      }
      else {
        bVar4 = false;
      }
      if (iVar5 < 0) {
        if ((bVar4) && (bVar4 = FUN_0046ff80((uint)lVar12,local_3c,-iVar5), bVar4)) {
          bVar4 = true;
        }
        else {
          bVar4 = false;
        }
      }
      if (bVar4) {
        cVar2 = '\x02';
      }
      else {
        cVar2 = '\x01';
      }
    }
    else {
      cVar2 = 5 < local_4c;
    }
    if (local_18 == 0) {
      if (bVar11) {
        if (param_1 == param_2) {
          uVar10 = 0x84;
          goto LAB_0046fb33;
        }
        *(char *)param_1 = (char)local_c + '0';
        param_1 = (uint *)((int)param_1 + 1);
      }
      else {
        if ((int)param_2 - (int)param_1 < (int)(uVar9 + 1)) {
          uVar10 = 0x84;
          goto LAB_0046fb33;
        }
        FUN_0046ef80(uVar9,local_c,(char *)param_1);
        param_1 = (uint *)((int)param_1 + uVar9 + 1);
      }
    }
    else {
      if ((int)param_2 - (int)param_1 < (int)uVar9) {
        uVar10 = 0x84;
        goto LAB_0046fb33;
      }
      if (local_c == 0) {
        _memset(param_1,0x30,uVar9);
      }
      else {
        FUN_00469450(uVar9,local_c,(int)param_1);
      }
      param_1 = (uint *)((int)param_1 + uVar9);
    }
    if (cVar2 != '\0') {
      local_28 = param_1;
      while (local_28 != puVar3) {
        local_28 = (uint *)((int)local_28 + -1);
        cVar1 = *(char *)local_28;
        if (cVar1 != '.') {
          if (cVar1 != '9') {
            if (cVar2 != '\x01') {
              uVar8 = (int)cVar1 & 0x80000001;
              if ((int)uVar8 < 0) {
                uVar8 = (uVar8 - 1 | 0xfffffffe) + 1;
              }
              if (uVar8 == 0) goto LAB_0046fa10;
            }
            *(char *)local_28 = cVar1 + '\x01';
            goto LAB_0046fa10;
          }
          *(char *)local_28 = '0';
          cVar2 = '\x01';
        }
      }
      *(char *)local_28 = '1';
      local_14 = local_14 + 1;
    }
LAB_0046fa10:
    if (local_14 < 0) {
      local_1a = 0x2d;
      local_14 = -local_14;
    }
    else {
      local_1a = 0x2b;
    }
    if (local_14 < 100) {
      local_7c = 4;
    }
    else {
      local_7c = 5;
    }
    if ((int)param_2 - (int)param_1 < local_7c) {
      uVar10 = 0x84;
    }
    else {
      *(undefined1 *)param_1 = 0x65;
      *(undefined1 *)((int)param_1 + 1) = local_1a;
      if (local_14 < 100) {
        FUN_00543e20((uint *)((int)param_1 + 2),(uint *)(&DAT_0056f350 + local_14 * 2),2);
        param_1 = param_1 + 1;
      }
      else {
        FUN_00543e20((uint *)((int)param_1 + 2),(uint *)(&DAT_0056f350 + (local_14 / 10) * 2),2);
        *(char *)(param_1 + 1) = (char)(local_14 % 10) + '0';
        param_1 = (uint *)((int)param_1 + 5);
      }
      uVar10 = 0;
      param_2 = param_1;
    }
  }
LAB_0046fb33:
  return CONCAT44(uVar10,param_2);
}

