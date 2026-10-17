/* Entry: 0x00509280; symbol: FUN_00509280; body bytes: 2844 */

void __thiscall
FUN_00509280(void *this,undefined4 param_1,char *param_2,undefined4 param_3,int param_4)

{
  bool bVar1;
  void *pvVar2;
  undefined3 extraout_var;
  int iVar3;
  int *piVar4;
  uint *puVar5;
  undefined4 *puVar6;
  uint uVar7;
  uint *puVar8;
  char **ppcVar9;
  int iVar10;
  int *unaff_FS_OFFSET;
  float10 fVar11;
  undefined8 uVar12;
  undefined4 local_fc;
  undefined4 local_f8;
  int local_ac;
  int local_a8;
  size_t local_a4;
  undefined4 *local_9c;
  int local_98;
  int local_90;
  undefined4 *local_84;
  int local_80;
  SIZE_T local_78;
  wchar_t *local_70;
  tm local_6c;
  basic_string<char,struct_std::char_traits<char>,class_std::allocator<char>_> local_48 [24];
  function<void___cdecl(void)> local_30 [24];
  uint local_18;
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  uint local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056a7e5;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_98 = 0;
  local_ac = 0;
  FUN_00548610(*(char **)((int)this + 0x1c),9,(int)param_2);
  for (local_a4 = _strlen(param_2); (int)local_a4 < 8; local_a4 = local_a4 + 1) {
    *(undefined1 *)(*(int *)((int)this + 0x1c) + local_a4) = 0x20;
  }
  if (((*(uint *)((int)this + 0x25c) & 1) == 0) && (param_4 != 0)) {
    pvVar2 = (void *)FUN_0040c300(*(undefined4 **)((int)this + 0xe0));
    bVar1 = FUN_00509e40(pvVar2,0xffff,0xffff,0xffff);
    if (CONCAT31(extraout_var,bVar1) != 0) {
      iVar3 = FUN_0050a2e0(this,*(int *)((int)this + 600));
      *(int *)((int)this + 0xe0) = iVar3;
    }
  }
  local_78 = 0x100;
  local_a8 = 0;
  for (local_80 = 0; local_80 < 8; local_80 = local_80 + 1) {
    piVar4 = (int *)FUN_00414580((void *)((int)this + 0x20),local_80);
    if (*piVar4 != 0) {
      if (local_98 == 0) {
        local_98 = local_80;
      }
      local_ac = local_80;
      if ((*(uint *)((int)this + 0x25c) & 1) == 0) {
        piVar4 = (int *)FUN_00414580((void *)((int)this + 0x20),local_80);
        *(undefined4 *)(*piVar4 + 0xc) = 0;
      }
      local_78 = local_78 + 0x2a0;
      for (local_9c = (undefined4 *)FUN_0040ff90((int)this + local_80 * 0x14 + 0x40);
          local_9c != (undefined4 *)0x0; local_9c = (undefined4 *)FUN_0040ff90((int)local_9c)) {
        pvVar2 = (void *)FUN_0040c300(local_9c);
        iVar3 = FUN_0050a110(pvVar2);
        local_78 = iVar3 + local_78;
        if ((*(uint *)((int)this + 0x25c) & 1) == 0) {
          pvVar2 = (void *)FUN_0040c300(local_9c);
          iVar3 = FUN_0050a110(pvVar2);
          piVar4 = (int *)FUN_00414580((void *)((int)this + 0x20),local_80);
          *(int *)(*piVar4 + 0xc) = *(int *)(*piVar4 + 0xc) + iVar3;
          pvVar2 = (void *)FUN_0040c300(local_9c);
          iVar3 = FUN_0050a0a0(pvVar2);
          piVar4 = (int *)FUN_00414580((void *)((int)this + 0x20),local_80);
          *(int *)(*piVar4 + 8) = *(int *)(*piVar4 + 8) + iVar3;
        }
      }
      local_a8 = local_a8 + 1;
    }
  }
  *(int *)(*(int *)((int)this + 0x1c) + 0xd4) = local_a8;
  uVar12 = FUN_004b8290(0x5ba568);
  iVar3 = *(int *)((int)this + 0x1c);
  local_fc = (undefined4)uVar12;
  local_f8 = (undefined4)((ulonglong)uVar12 >> 0x20);
  *(undefined4 *)(iVar3 + 0x18) = local_fc;
  *(undefined4 *)(iVar3 + 0x1c) = local_f8;
  fVar11 = FUN_0050a1e0(DAT_005c4a00);
  *(float *)(*(int *)((int)this + 0x1c) + 0xd0) = (float)fVar11;
  puVar5 = (uint *)FUN_0041f610(local_78);
  FUN_00543e20(puVar5,*(uint **)((int)this + 0x1c),0x100);
  local_78 = 0x100;
  for (local_90 = 0; local_90 < 8; local_90 = local_90 + 1) {
    piVar4 = (int *)FUN_00414580((void *)((int)this + 0x20),local_90);
    if (*piVar4 != 0) {
      uVar7 = 0x2a0;
      puVar6 = (undefined4 *)FUN_00414580((void *)((int)this + 0x20),local_90);
      FUN_00543e20((uint *)((int)puVar5 + local_78),(uint *)*puVar6,uVar7);
      local_78 = local_78 + 0x2a0;
      for (local_84 = (undefined4 *)FUN_0040ff90((int)this + local_90 * 0x14 + 0x40);
          local_84 != (undefined4 *)0x0; local_84 = (undefined4 *)FUN_0040ff90((int)local_84)) {
        pvVar2 = (void *)FUN_0040c300(local_84);
        uVar7 = FUN_0050a1a0(pvVar2);
        puVar8 = (uint *)FUN_00507ad0(pvVar2,0);
        FUN_00543e20((uint *)((int)puVar5 + local_78),puVar8,uVar7);
        iVar3 = FUN_0050a1a0(pvVar2);
        local_78 = iVar3 + local_78;
      }
      for (local_84 = (undefined4 *)FUN_0040ff90((int)this + local_90 * 0x14 + 0x40);
          local_84 != (undefined4 *)0x0; local_84 = (undefined4 *)FUN_0040ff90((int)local_84)) {
        iVar3 = FUN_0040c300(local_84);
        uVar7 = FUN_0050a170(iVar3);
        puVar8 = (uint *)FUN_00411c10((void *)(iVar3 + 0x34bc0),0);
        FUN_00543e20((uint *)((int)puVar5 + local_78),puVar8,uVar7);
        iVar3 = FUN_0050a170(iVar3);
        local_78 = iVar3 + local_78;
      }
    }
  }
  puVar8 = (uint *)FUN_00539550((byte *)puVar5,local_78,(int *)&local_18);
  if (puVar5 != (uint *)0x0) {
    FUN_0041f670(puVar5);
  }
  FUN_004103c0(puVar8,local_18,0x7d,':',0x100,local_18);
  FUN_004103c0(puVar8,local_18,0x5c,-0x1f,0x400,local_18);
  *(SIZE_T *)(*(int *)((int)this + 0x18) + 0x2c) = local_78;
  *(uint *)(*(int *)((int)this + 0x18) + 0x28) = local_18;
  *(int *)(*(int *)((int)this + 0x18) + 0xc) = *(int *)(*(int *)((int)this + 0x18) + 0x28) + 0x30;
  FUN_00417cd0(local_30,&DAT_005b67e1);
  local_8 = 0;
  FUN_00417e40("replay");
  FUN_004d8620(&param_1);
  FUN_0041e790(local_30,local_48);
  local_8 = CONCAT31(local_8._1_3_,1);
  ppcVar9 = (char **)FUN_0040ff30((undefined4 *)local_48);
  FUN_00410840(ppcVar9);
  FUN_00411060(*(LPCVOID *)((int)this + 0x18),0x30);
  FUN_00411060(puVar8,local_18);
  if (puVar8 != (uint *)0x0) {
    FUN_0041f670(puVar8);
  }
  local_8 = local_8 & 0xffffff00;
  FUN_0040e5a0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)local_48);
  local_8 = 0xffffffff;
  FUN_0040e6c0(local_30);
  puVar6 = (undefined4 *)FUN_0041f610(0xffff);
  _memset(puVar6,0,0xffff);
  FUN_00508b00(puVar6);
  *(undefined1 *)(puVar6 + 2) = 0;
  iVar3 = FID_conflict__swprintf((wchar_t *)(puVar6 + 3),(wchar_t *)0xea60,&DAT_00573530);
  local_70 = (wchar_t *)(iVar3 + (int)(puVar6 + 3));
  iVar3 = FID_conflict__swprintf(local_70,(wchar_t *)0xea60,"Version %s\r\n");
  local_70 = (wchar_t *)(iVar3 + (int)local_70);
  iVar3 = FID_conflict__swprintf(local_70,(wchar_t *)0xea60,"Name %s\r\n");
  local_70 = (wchar_t *)(iVar3 + (int)local_70);
  local_6c.tm_sec = 0;
  local_6c.tm_min = 0;
  local_6c.tm_hour = 0;
  local_6c.tm_mday = 0;
  local_6c.tm_mon = 0;
  local_6c.tm_year = 0;
  local_6c.tm_wday = 0;
  local_6c.tm_yday = 0;
  local_6c.tm_isdst = 0;
  FUN_004ac270(&local_6c,(uint *)(*(int *)((int)this + 0x1c) + 0x10));
  iVar3 = FID_conflict__swprintf
                    (local_70,(wchar_t *)0xea60,"Date %.2d/%.2d/%.2d %.2d:%.2d\r\n",
                     local_6c.tm_year % 100,local_6c.tm_mon + 1,local_6c.tm_mday,local_6c.tm_hour);
  local_70 = (wchar_t *)(iVar3 + (int)local_70);
  iVar3 = FID_conflict__swprintf(local_70,(wchar_t *)0xea60,"Chara %s\r\n");
  local_70 = (wchar_t *)(iVar3 + (int)local_70);
  iVar3 = FID_conflict__swprintf(local_70,(wchar_t *)0xea60,"Rank %s\r\n");
  local_70 = (wchar_t *)(iVar3 + (int)local_70);
  if (*(int *)(*(int *)((int)this + 0x1c) + 0xf4) < 8) {
    if (local_98 == local_ac) {
      if (local_98 == 7) {
        iVar3 = FID_conflict__swprintf(local_70,(wchar_t *)0xea60);
      }
      else {
        iVar3 = FID_conflict__swprintf(local_70,(wchar_t *)0xea60,"Stage %d\r\n",local_98);
      }
      local_70 = (wchar_t *)(iVar3 + (int)local_70);
    }
    else {
      iVar3 = FID_conflict__swprintf(local_70,(wchar_t *)0xea60,&DAT_005735ec,local_98,local_ac);
      local_70 = (wchar_t *)(iVar3 + (int)local_70);
    }
  }
  else {
    if (local_98 == 7) {
      iVar3 = FID_conflict__swprintf(local_70,(wchar_t *)0xea60);
    }
    else {
      iVar3 = FID_conflict__swprintf(local_70,(wchar_t *)0xea60);
    }
    local_70 = (wchar_t *)(iVar3 + (int)local_70);
  }
  iVar3 = FID_conflict__swprintf
                    (local_70,(wchar_t *)0xea60,"Score %llu\r\n",
                     *(undefined4 *)(*(int *)((int)this + 0x1c) + 0x18),
                     *(undefined4 *)(*(int *)((int)this + 0x1c) + 0x1c));
  iVar10 = FID_conflict__swprintf
                     ((wchar_t *)(iVar3 + (int)local_70),(wchar_t *)0xea60,"Slow Rate %2.2f\r\n",
                      (double)*(float *)(*(int *)((int)this + 0x1c) + 0xd0));
  local_70 = (wchar_t *)(iVar3 + (int)local_70 + iVar10 + 1);
  uVar7 = (int)local_70 - (int)puVar6 & 0x80000003;
  if ((int)uVar7 < 0) {
    uVar7 = (uVar7 - 1 | 0xfffffffc) + 1;
  }
  if (uVar7 != 0) {
    uVar7 = (int)local_70 - (int)puVar6 & 0x80000003;
    if ((int)uVar7 < 0) {
      uVar7 = (uVar7 - 1 | 0xfffffffc) + 1;
    }
    local_70 = (wchar_t *)((4 - uVar7) + (int)local_70);
  }
  puVar6[1] = (int)local_70 - (int)puVar6;
  FUN_00411060(puVar6,(int)local_70 - (int)puVar6);
  _memset(puVar6,0,0xffff);
  FUN_00508b00(puVar6);
  *(undefined1 *)(puVar6 + 2) = 1;
  iVar3 = FID_conflict__swprintf((wchar_t *)(puVar6 + 3),(wchar_t *)0xea60);
  local_70 = (wchar_t *)((int)(puVar6 + 3) + iVar3 + 1);
  uVar7 = (int)local_70 - (int)puVar6 & 0x80000003;
  if ((int)uVar7 < 0) {
    uVar7 = (uVar7 - 1 | 0xfffffffc) + 1;
  }
  if (uVar7 != 0) {
    uVar7 = (int)local_70 - (int)puVar6 & 0x80000003;
    if ((int)uVar7 < 0) {
      uVar7 = (uVar7 - 1 | 0xfffffffc) + 1;
    }
    local_70 = (wchar_t *)((4 - uVar7) + (int)local_70);
  }
  puVar6[1] = (int)local_70 - (int)puVar6;
  FUN_00411060(puVar6,(int)local_70 - (int)puVar6);
  if (puVar6 != (undefined4 *)0x0) {
    FUN_0041f670(puVar6);
  }
  FUN_004106e0();
  *(uint *)((int)this + 0x25c) = *(uint *)((int)this + 0x25c) | 1;
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

