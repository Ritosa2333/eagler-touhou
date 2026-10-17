/* Entry: 0x0055d519; symbol: FUN_0055d519; body bytes: 452 */

byte * __cdecl FUN_0055d519(LPWSTR param_1,byte *param_2,byte *param_3,int *param_4)

{
  byte bVar1;
  int *piVar2;
  byte *pbVar3;
  int iVar4;
  DWORD DVar5;
  int *unaff_FS_OFFSET;
  uint local_1c;
  byte *pbStack_18;
  byte *local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  piVar2 = param_4;
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_005682e0;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  if (param_1 != (LPWSTR)0x0) {
    pbVar3 = (byte *)0x0;
    if (param_3 == (byte *)0x0) goto LAB_0055d6cd;
    *param_1 = L'\0';
  }
  if (param_2 == (byte *)0x0) {
    *(undefined1 *)(param_4 + 7) = 1;
    param_4[6] = 0x16;
    FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,param_4);
  }
  else {
    if ((char)param_4[5] == '\0') {
      FUN_0054a360(param_4);
    }
    if (*(int *)(piVar2[3] + 8) == 0xfde9) {
      _local_1c = 0;
      pbVar3 = (byte *)FUN_005596de(param_3,(ushort *)param_1,&param_2,(uint)param_3,&local_1c,
                                    (int)piVar2);
      goto LAB_0055d6cd;
    }
    iVar4 = *(int *)(piVar2[3] + 0xa8);
    if (param_1 == (LPWSTR)0x0) {
      if (iVar4 == 0) {
        pbVar3 = param_2 + 1;
        do {
          bVar1 = *param_2;
          param_2 = param_2 + 1;
        } while (bVar1 != 0);
        pbVar3 = param_2 + -(int)pbVar3;
        goto LAB_0055d6cd;
      }
      iVar4 = FUN_0055dd91(*(uint *)(piVar2[3] + 8),9,(LPCSTR)param_2,-1,(LPWSTR)0x0,0);
      if (iVar4 != 0) goto LAB_0055d6cc;
      *(undefined1 *)(piVar2 + 7) = 1;
      piVar2[6] = 0x2a;
    }
    else {
      if (iVar4 == 0) {
        pbVar3 = (byte *)0x0;
        if (param_3 != (byte *)0x0) {
          do {
            *param_1 = (ushort)pbVar3[(int)param_2];
            if (pbVar3[(int)param_2] == 0) break;
            pbVar3 = pbVar3 + 1;
            param_1 = param_1 + 1;
          } while (pbVar3 < param_3);
        }
        goto LAB_0055d6cd;
      }
      iVar4 = FUN_0055dd91(*(uint *)(piVar2[3] + 8),9,(LPCSTR)param_2,-1,param_1,(int)param_3);
      if (iVar4 != 0) {
LAB_0055d6cc:
        pbVar3 = (byte *)(iVar4 + -1);
        goto LAB_0055d6cd;
      }
      DVar5 = GetLastError();
      local_14 = param_2;
      pbVar3 = param_3;
      if (DVar5 == 0x7a) {
        while (pbVar3 != (byte *)0x0) {
          _local_1c = CONCAT44(pbVar3 + -1,local_1c);
          if (*local_14 == 0) break;
          if ((*(short *)(*(int *)piVar2[3] + (uint)*local_14 * 2) < 0) &&
             (local_14 = local_14 + 1, *local_14 == 0)) goto LAB_0055d623;
          local_14 = local_14 + 1;
          pbVar3 = pbVar3 + -1;
        }
        pbVar3 = (byte *)FUN_0055dd91(*(uint *)(piVar2[3] + 8),1,(LPCSTR)param_2,
                                      (int)local_14 - (int)param_2,param_1,(int)param_3);
        if (pbVar3 != (byte *)0x0) goto LAB_0055d6cd;
      }
LAB_0055d623:
      *(undefined1 *)(piVar2 + 7) = 1;
      piVar2[6] = 0x2a;
      *param_1 = L'\0';
    }
  }
  pbVar3 = (byte *)0xffffffff;
LAB_0055d6cd:
  *unaff_FS_OFFSET = local_10;
  return pbVar3;
}

