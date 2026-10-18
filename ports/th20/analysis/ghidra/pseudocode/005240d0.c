/* Entry: 0x005240d0; symbol: FUN_005240d0; body bytes: 2877 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __fastcall FUN_005240d0(int param_1)

{
  bool bVar1;
  undefined3 extraout_var;
  int *piVar2;
  int iVar3;
  int iVar4;
  undefined3 extraout_var_00;
  undefined4 *puVar5;
  float10 fVar6;
  int local_84;
  int local_7c;
  int local_78;
  undefined4 local_68;
  float local_64;
  tm local_5c;
  tm local_38;
  undefined4 local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (*(int *)(param_1 + 0x20) == 2) {
    FUN_00422dd0(&local_68,DAT_0056cd94,DAT_0056fa30,0);
    FUN_004e6a20(DAT_005c0698,1);
    local_7c = FUN_0040c300((undefined4 *)(param_1 + 0x108));
    for (local_7c = local_7c * 0x19; iVar3 = FUN_0040c300((undefined4 *)(param_1 + 0x108)),
        local_7c < iVar3 * 0x19 + 0x19; local_7c = local_7c + 1) {
      bVar1 = FUN_0046b1b0((void *)(param_1 + 0x24),local_7c % 0x19);
      if (CONCAT31(extraout_var,bVar1) == 0) {
        FUN_00470ad0(DAT_005c0698,0xff808080);
      }
      else {
        FUN_00470ad0(DAT_005c0698,0xffffff00);
      }
      piVar2 = (int *)FUN_00414580((void *)(param_1 + 0x5740),local_7c);
      if (*piVar2 == 0) {
        iVar3 = FUN_0040c300((undefined4 *)(param_1 + 0x108));
        if (iVar3 == 0) {
          local_84 = 1;
        }
        else {
          local_84 = 3;
        }
        FUN_0046c990(DAT_005c0698,&local_68,
                     (wchar_t *)(&PTR_s_No___2d__s___2d___2d___2d___2d___005b0a74)[local_84]);
      }
      else {
        piVar2 = (int *)FUN_00414580((void *)(param_1 + 0x5740),local_7c);
        iVar3 = FUN_00421810(*piVar2);
        local_5c.tm_sec = 0;
        local_5c.tm_min = 0;
        local_5c.tm_hour = 0;
        local_5c.tm_mday = 0;
        local_5c.tm_mon = 0;
        local_5c.tm_year = 0;
        local_5c.tm_wday = 0;
        local_5c.tm_yday = 0;
        local_5c.tm_isdst = 0;
        FUN_004ac270(&local_5c,(uint *)(iVar3 + 0x10));
        iVar4 = FUN_0040c300((undefined4 *)(param_1 + 0x108));
        if (iVar4 == 0) {
          if ((*(byte *)(iVar3 + 10) >> 1 & 1) == 0) {
            FUN_0046c990(DAT_005c0698,&local_68,
                         (wchar_t *)PTR_s_No___2d__s___2d___2d___2d___2d___005b0a74);
          }
          else {
            FUN_0046c990(DAT_005c0698,&local_68,
                         (wchar_t *)PTR_s_No___2d__s___2d___2d___2d___2d___005b0a84);
          }
        }
        else {
          piVar2 = (int *)FUN_00414580((void *)(param_1 + 0x5740),local_7c);
          FUN_0050a220(*piVar2);
          FUN_0046c990(DAT_005c0698,&local_68,
                       (wchar_t *)PTR_s__s__s___2d___2d___2d___2d___2d___005b0a7c);
        }
      }
      local_64 = local_64 + _DAT_0056f2fc;
    }
    FUN_00470ad0(DAT_005c0698,0xffffffff);
    FUN_004e6a20(DAT_005c0698,0);
  }
  else if (*(int *)(param_1 + 0x20) == 4) {
    FUN_00422dd0(&local_14,DAT_0056cd94,DAT_0056fa30,0);
    piVar2 = (int *)FUN_00414580((void *)(param_1 + 0x5740),*(int *)(param_1 + 0x5738));
    iVar3 = FUN_00421810(*piVar2);
    bVar1 = FUN_00423560((void *)(param_1 + 0x154),10);
    if (bVar1) {
      iVar4 = *(int *)(param_1 + 0x5738);
      fVar6 = FUN_00423bd0(param_1 + 0x154);
      local_10 = ((float)((iVar4 % 0x19) * 0xf) * (DAT_0056e734 - (float)fVar6)) / DAT_0056e734 +
                 DAT_0056fa30;
    }
    FUN_004e6a20(DAT_005c0698,1);
    local_38.tm_sec = 0;
    local_38.tm_min = 0;
    local_38.tm_hour = 0;
    local_38.tm_mday = 0;
    local_38.tm_mon = 0;
    local_38.tm_year = 0;
    local_38.tm_wday = 0;
    local_38.tm_yday = 0;
    local_38.tm_isdst = 0;
    FUN_004ac270(&local_38,(uint *)(iVar3 + 0x10));
    iVar4 = FUN_0040c300((undefined4 *)(param_1 + 0x108));
    if (iVar4 == 0) {
      if ((*(byte *)(iVar3 + 10) >> 1 & 1) == 0) {
        FUN_0046c990(DAT_005c0698,&local_14,
                     (wchar_t *)PTR_s_No___2d__s___2d___2d___2d___2d___005b0a74);
      }
      else {
        FUN_0046c990(DAT_005c0698,&local_14,
                     (wchar_t *)PTR_s_No___2d__s___2d___2d___2d___2d___005b0a84);
      }
    }
    else {
      piVar2 = (int *)FUN_00414580((void *)(param_1 + 0x5740),*(int *)(param_1 + 0x5738));
      FUN_0050a220(*piVar2);
      FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)PTR_s__s__s___2d___2d___2d___2d___2d___005b0a7c
                  );
    }
    bVar1 = FUN_004235c0((void *)(param_1 + 0x154),10);
    if (bVar1) {
      local_14 = DAT_00575678;
      local_10 = DAT_0056cd98;
      for (local_78 = 1; local_78 < 8; local_78 = local_78 + 1) {
        bVar1 = FUN_0046b1b0((void *)(param_1 + 0x24),local_78 + -1);
        if (CONCAT31(extraout_var_00,bVar1) == 0) {
          FUN_00470ad0(DAT_005c0698,0xff808080);
        }
        else {
          FUN_00470ad0(DAT_005c0698,0xffffff00);
        }
        puVar5 = (undefined4 *)FUN_00414580((void *)(param_1 + 0x5740),*(int *)(param_1 + 0x5738));
        iVar3 = FUN_00488770((void *)*puVar5,local_78);
        if (iVar3 == 0) {
          FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"%s  ---------");
        }
        else {
          if (local_78 < 7) {
            puVar5 = (undefined4 *)
                     FUN_00414580((void *)(param_1 + 0x5740),*(int *)(param_1 + 0x5738));
            iVar3 = FUN_00488770((void *)*puVar5,local_78 + 1);
            if (iVar3 != 0) {
              puVar5 = (undefined4 *)
                       FUN_00414580((void *)(param_1 + 0x5740),*(int *)(param_1 + 0x5738));
              iVar3 = FUN_00488770((void *)*puVar5,local_78 + 1);
              FUN_004b7fc0(iVar3 + 0x70);
              FUN_0052c400((undefined8 *)(iVar3 + 0x70));
              FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"%s  %8llu%d");
              goto LAB_0052498c;
            }
          }
          puVar5 = (undefined4 *)FUN_00414580((void *)(param_1 + 0x5740),*(int *)(param_1 + 0x5738))
          ;
          iVar3 = FUN_00488770((void *)*puVar5,local_78);
          if (iVar3 != 0) {
            FUN_0046c990(DAT_005c0698,&local_14,(wchar_t *)"%s  %8llu%d");
          }
        }
LAB_0052498c:
        local_10 = local_10 + _DAT_00572644;
      }
    }
    FUN_00470ad0(DAT_005c0698,0xffffffff);
    FUN_004e6a20(DAT_005c0698,0);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

