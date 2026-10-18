/* Entry: 0x00550250; symbol: FUN_00550250; body bytes: 587 */

uint __cdecl FUN_00550250(int *param_1,byte *param_2,undefined4 *param_3,uint param_4,byte param_5)

{
  byte bVar1;
  byte *pbVar2;
  bool bVar3;
  undefined4 uVar4;
  uint uVar5;
  int iVar6;
  uint uVar7;
  byte bVar8;
  uint local_10;
  byte local_8;
  
  uVar7 = param_4;
  uVar4 = FUN_0054a3bb((int *)&param_2);
  pbVar2 = param_2;
  if ((char)uVar4 == '\0') {
LAB_00550295:
    if (param_3 != (undefined4 *)0x0) {
      *param_3 = param_2;
    }
    return 0;
  }
  if ((uVar7 != 0) && (((int)uVar7 < 2 || (0x24 < (int)uVar7)))) {
    *(undefined1 *)(param_1 + 7) = 1;
    param_1[6] = 0x16;
    FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,param_1);
    goto LAB_00550295;
  }
  local_10 = 0;
  local_8 = *param_2;
  param_2 = param_2 + 1;
  if ((char)param_1[5] == '\0') {
    FUN_0054a360(param_1);
  }
  while (uVar5 = FUN_0054fc37((uint)local_8,8,(_locale_t)(param_1 + 3)), uVar5 != 0) {
    local_8 = *param_2;
    param_2 = param_2 + 1;
  }
  bVar8 = param_5 | 2;
  if ((local_8 == 0x2d) || (bVar8 = param_5, local_8 == 0x2b)) {
    local_8 = *param_2;
    param_2 = param_2 + 1;
  }
  if ((uVar7 != 0) && (uVar5 = uVar7, uVar7 != 0x10)) goto LAB_00550395;
  if ((byte)(local_8 - 0x30) < 10) {
    iVar6 = (char)local_8 + -0x30;
LAB_00550355:
    if (iVar6 != 0) goto LAB_0055038b;
    bVar1 = *param_2;
    if ((bVar1 == 0x78) || (bVar1 == 0x58)) {
      local_8 = param_2[1];
      param_2 = param_2 + 2;
      uVar5 = 0x10;
      if (uVar7 != 0) {
        uVar5 = uVar7;
      }
      goto LAB_00550395;
    }
    param_2 = param_2 + 1;
    __crt_strtox::c_string_character_source<char>::unget
              ((c_string_character_source<char> *)&param_2,bVar1);
    uVar5 = 8;
  }
  else {
    if ((byte)(local_8 + 0x9f) < 0x1a) {
      iVar6 = (char)local_8 + -0x57;
      goto LAB_00550355;
    }
    if ((byte)(local_8 + 0xbf) < 0x1a) {
      iVar6 = (char)local_8 + -0x37;
      goto LAB_00550355;
    }
LAB_0055038b:
    uVar5 = 10;
  }
  if (uVar7 != 0) {
    uVar5 = uVar7;
  }
LAB_00550395:
  while( true ) {
    if ((byte)(local_8 - 0x30) < 10) {
      uVar7 = (int)(char)local_8 - 0x30;
    }
    else if ((byte)(local_8 + 0x9f) < 0x1a) {
      uVar7 = (int)(char)local_8 - 0x57;
    }
    else if ((byte)(local_8 + 0xbf) < 0x1a) {
      uVar7 = (int)(char)local_8 - 0x37;
    }
    else {
      uVar7 = 0xffffffff;
    }
    if (uVar5 <= uVar7) break;
    uVar7 = local_10 * uVar5 + uVar7;
    bVar8 = bVar8 | (uVar7 < local_10 * uVar5 || (uint)(0xffffffff / (ulonglong)uVar5) < local_10)
                    << 2 | 8U;
    local_8 = *param_2;
    param_2 = param_2 + 1;
    local_10 = uVar7;
  }
  __crt_strtox::c_string_character_source<char>::unget
            ((c_string_character_source<char> *)&param_2,local_8);
  if ((bVar8 & 8) == 0) {
    if (param_3 == (undefined4 *)0x0) {
      return 0;
    }
    *param_3 = pbVar2;
    return 0;
  }
  bVar3 = FUN_005489e6(bVar8,local_10);
  if (bVar3) {
    *(undefined1 *)(param_1 + 7) = 1;
    param_1[6] = 0x22;
    if ((bVar8 & 1) != 0) {
      if ((bVar8 & 2) == 0) {
        if (param_3 != (undefined4 *)0x0) {
          *param_3 = param_2;
        }
        return 0x7fffffff;
      }
      if (param_3 != (undefined4 *)0x0) {
        *param_3 = param_2;
      }
      return 0x80000000;
    }
    local_10 = 0xffffffff;
  }
  else if ((bVar8 & 2) != 0) {
    local_10 = -local_10;
  }
  if (param_3 != (undefined4 *)0x0) {
    *param_3 = param_2;
    return local_10;
  }
  return local_10;
}

