/* Entry: 0x00564667; symbol: FUN_00564667; body bytes: 966 */

void __cdecl FUN_00564667(DWORD *param_1,uint param_2,uint *param_3,int param_4,int *param_5)

{
  byte bVar1;
  ushort uVar2;
  byte *pbVar3;
  byte *pbVar4;
  uint *puVar5;
  BOOL BVar6;
  DWORD DVar7;
  int iVar8;
  void *pvVar9;
  int iVar10;
  void *pvVar11;
  int *unaff_FS_OFFSET;
  uint uVar12;
  undefined8 local_80;
  int local_78;
  UINT local_74;
  uint *local_70;
  HANDLE local_6c;
  uint *local_68;
  uint *local_64;
  undefined2 local_60 [2];
  undefined8 local_5c;
  uint *local_54;
  int local_50;
  uint local_4c;
  int local_48;
  undefined4 local_44;
  int *local_40;
  uint *local_3c;
  void *local_38;
  char local_31;
  uint *local_30;
  CHAR local_2c [8];
  undefined1 local_24;
  undefined1 local_23;
  uint local_1c [4];
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_0056b32a;
  local_1c[3] = *unaff_FS_OFFSET;
  local_1c[2] = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)(local_1c + 3);
  local_50 = (param_2 & 0x3f) * 0x38;
  local_48 = (int)param_2 >> 6;
  local_68 = param_3;
  local_40 = param_5;
  local_6c = *(HANDLE *)(local_50 + 0x18 + (&DAT_005e5690)[local_48]);
  local_54 = (uint *)(param_4 + (int)param_3);
  local_74 = GetConsoleOutputCP();
  if ((char)param_5[5] == '\0') {
    FUN_0054a360(param_5);
  }
  local_78 = *(int *)(param_5[3] + 8);
  *param_1 = 0;
  param_1[1] = 0;
  param_1[2] = 0;
  local_30 = local_68;
  if (local_54 <= local_68) {
LAB_00564a11:
    *unaff_FS_OFFSET = local_1c[3];
    FUN_005429ee(local_1c[2] ^ (uint)&stack0xfffffffc);
    return;
  }
  local_5c = local_5c & 0xffffffff;
  iVar10 = local_50;
LAB_0056470c:
  pvVar11 = (void *)0x0;
  local_31 = (char)*local_30;
  local_44 = 0;
  local_38 = (void *)0x1;
  if (local_78 == 0xfde9) {
    pbVar3 = (byte *)((&DAT_005e5690)[local_48] + 0x2e + iVar10);
    local_5c = CONCAT44(pbVar3,(uint)local_5c);
    pbVar4 = pbVar3;
    local_38 = pvVar11;
    do {
      if (*pbVar4 == 0) break;
      local_38 = (void *)((int)local_38 + 1);
      pbVar4 = pbVar4 + 1;
    } while ((int)local_38 < 5);
    iVar10 = (int)local_54 - (int)local_30;
    if ((int)local_38 < 1) {
      local_4c = (int)(char)(&DAT_005b2f20)[(byte)*local_30] + 1;
      if (iVar10 < (int)local_4c) {
        if (0 < iVar10) {
          do {
            *(undefined1 *)((&DAT_005e5690)[local_48] + local_50 + 0x2e + (int)pvVar11) =
                 *(undefined1 *)((int)pvVar11 + (int)local_30);
            pvVar11 = (void *)((int)pvVar11 + 1);
          } while ((int)pvVar11 < iVar10);
        }
        goto LAB_005649c2;
      }
      local_5c = 0;
      local_38 = (void *)((local_4c == 4) + 1);
      local_3c = local_30;
      iVar8 = FUN_005596de(local_38,(ushort *)&local_44,&local_3c,(uint)local_38,(uint *)&local_5c,
                           (int)local_40);
      iVar10 = local_50;
    }
    else {
      local_3c = (uint *)((int)(char)(&DAT_005b2f20)[*pbVar3] + 1);
      local_4c = (int)local_3c - (int)local_38;
      if (iVar10 < (int)local_4c) {
        if (0 < iVar10) {
          do {
            pvVar9 = (void *)((int)pvVar11 + 1);
            *(undefined1 *)
             ((int)pvVar11 + (&DAT_005e5690)[local_48] + local_50 + 0x2e + (int)local_38) =
                 *(undefined1 *)((int)pvVar11 + (int)local_30);
            pvVar11 = pvVar9;
          } while ((int)pvVar9 < iVar10);
        }
LAB_005649c2:
        param_1[1] = param_1[1] + iVar10;
        goto LAB_00564a11;
      }
      do {
        *(byte *)((int)local_1c + (int)pvVar11) = *pbVar3;
        pvVar11 = (void *)((int)pvVar11 + 1);
        pbVar3 = pbVar3 + 1;
      } while ((int)pvVar11 < (int)local_38);
      if (0 < (int)local_4c) {
        FUN_00543e20((uint *)((int)local_1c + (int)local_38),local_30,local_4c);
      }
      iVar10 = local_50;
      iVar8 = 0;
      do {
        *(undefined1 *)((&DAT_005e5690)[local_48] + local_50 + 0x2e + iVar8) = 0;
        iVar8 = iVar8 + 1;
      } while (iVar8 < (int)local_38);
      local_70 = local_1c;
      local_80 = 0;
      local_38 = (void *)((local_3c == (uint *)0x4) + 1);
      iVar8 = FUN_005596de(&local_80,(ushort *)&local_44,&local_70,(uint)local_38,(uint *)&local_80,
                           (int)local_40);
    }
    if (iVar8 == -1) goto LAB_00564a11;
    puVar5 = (uint *)((int)local_30 + (local_4c - 1));
  }
  else {
    local_4c = (&DAT_005e5690)[local_48];
    bVar1 = *(byte *)(local_4c + 0x2d + iVar10);
    if ((bVar1 & 4) == 0) {
      if (*(short *)(*(int *)local_40[3] + (uint)(byte)*local_30 * 2) < 0) {
        local_3c = (uint *)((int)local_30 + 1);
        if (local_3c < local_54) {
          uVar12 = FUN_0055929a((LPWSTR)&local_44,(byte *)local_30,2,local_40);
          puVar5 = local_3c;
          if (uVar12 != 0xffffffff) goto LAB_005648e9;
        }
        else {
          *(char *)(local_4c + 0x2e + iVar10) = (char)*local_30;
          pbVar4 = (byte *)((&DAT_005e5690)[local_48] + 0x2d + iVar10);
          *pbVar4 = *pbVar4 | 4;
          param_1[1] = local_5c._4_4_ + 1;
        }
        goto LAB_00564a11;
      }
      uVar12 = 1;
      puVar5 = local_30;
    }
    else {
      uVar2 = CONCAT11(bVar1,*(undefined1 *)(local_4c + 0x2e + iVar10)) & 0xfbff;
      local_24 = (undefined1)uVar2;
      local_23 = (undefined1)*local_30;
      *(char *)(local_4c + 0x2d + iVar10) = (char)(uVar2 >> 8);
      uVar12 = 2;
      puVar5 = (uint *)&local_24;
    }
    uVar12 = FUN_0055929a((LPWSTR)&local_44,(byte *)puVar5,uVar12,local_40);
    puVar5 = local_30;
    if (uVar12 == 0xffffffff) goto LAB_00564a11;
  }
LAB_005648e9:
  local_30 = (uint *)((int)puVar5 + 1);
  local_3c = (uint *)FUN_0055de4b(local_74,0,(LPCWSTR)&local_44,(int)local_38,local_2c,5,(LPCSTR)0x0
                                  ,(LPBOOL)0x0);
  if (local_3c == (uint *)0x0) goto LAB_00564a11;
  BVar6 = WriteFile(local_6c,local_2c,(DWORD)local_3c,(LPDWORD)&local_64,(LPOVERLAPPED)0x0);
  if (BVar6 == 0) {
LAB_00564a09:
    DVar7 = GetLastError();
    *param_1 = DVar7;
    goto LAB_00564a11;
  }
  DVar7 = (param_1[2] - (int)local_68) + (int)local_30;
  local_5c = CONCAT44(DVar7,(uint)local_5c);
  param_1[1] = DVar7;
  if (local_64 < local_3c) goto LAB_00564a11;
  if (local_31 == '\n') {
    local_60[0] = 0xd;
    BVar6 = WriteFile(local_6c,local_60,1,(LPDWORD)&local_64,(LPOVERLAPPED)0x0);
    if (BVar6 == 0) goto LAB_00564a09;
    if (local_64 == (uint *)0x0) goto LAB_00564a11;
    param_1[2] = param_1[2] + 1;
    param_1[1] = param_1[1] + 1;
    local_5c = CONCAT44(param_1[1],(uint)local_5c);
  }
  if (local_54 <= local_30) goto LAB_00564a11;
  goto LAB_0056470c;
}

