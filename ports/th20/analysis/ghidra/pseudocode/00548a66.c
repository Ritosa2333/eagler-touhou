/* Entry: 0x00548a66; symbol: FUN_00548a66; body bytes: 531 */

uint __cdecl FUN_00548a66(int *param_1,char *param_2,undefined4 *param_3,uint param_4,byte param_5)

{
  char cVar1;
  char *pcVar2;
  bool bVar3;
  undefined4 uVar4;
  int iVar5;
  uint uVar6;
  byte bVar7;
  char *pcVar8;
  uint uVar9;
  uint local_10;
  char local_8;
  
  uVar6 = param_4;
  uVar4 = FUN_0054a3bb((int *)&param_2);
  pcVar2 = param_2;
  if ((char)uVar4 == '\0') {
LAB_00548aab:
    if (param_3 != (undefined4 *)0x0) {
      *param_3 = param_2;
    }
    return 0;
  }
  if ((uVar6 != 0) && (((int)uVar6 < 2 || (0x24 < (int)uVar6)))) {
    *(undefined1 *)(param_1 + 7) = 1;
    param_1[6] = 0x16;
    FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,param_1);
    goto LAB_00548aab;
  }
  local_10 = 0;
  local_8 = *param_2;
  pcVar8 = param_2 + 1;
  bVar7 = param_5 | 2;
  if ((local_8 == '-') || (bVar7 = param_5, local_8 == '+')) {
    local_8 = *pcVar8;
    pcVar8 = param_2 + 2;
  }
  param_2 = pcVar8;
  if ((uVar6 != 0) && (uVar9 = uVar6, uVar6 != 0x10)) goto LAB_00548b73;
  if ((byte)(local_8 - 0x30U) < 10) {
    iVar5 = local_8 + -0x30;
LAB_00548b31:
    if (iVar5 != 0) goto LAB_00548b69;
    cVar1 = *pcVar8;
    param_2 = pcVar8 + 1;
    if ((cVar1 == 'x') || (cVar1 == 'X')) {
      local_8 = *param_2;
      param_2 = pcVar8 + 2;
      uVar9 = 0x10;
      if (uVar6 != 0) {
        uVar9 = uVar6;
      }
      goto LAB_00548b73;
    }
    __crt_strtox::c_string_character_source<char>::unget
              ((c_string_character_source<char> *)&param_2,cVar1);
    uVar9 = 8;
  }
  else {
    if ((byte)(local_8 + 0x9fU) < 0x1a) {
      iVar5 = local_8 + -0x57;
      goto LAB_00548b31;
    }
    if ((byte)(local_8 + 0xbfU) < 0x1a) {
      iVar5 = local_8 + -0x37;
      goto LAB_00548b31;
    }
LAB_00548b69:
    uVar9 = 10;
  }
  if (uVar6 != 0) {
    uVar9 = uVar6;
  }
LAB_00548b73:
  while( true ) {
    if ((byte)(local_8 - 0x30U) < 10) {
      uVar6 = (int)local_8 - 0x30;
    }
    else if ((byte)(local_8 + 0x9fU) < 0x1a) {
      uVar6 = (int)local_8 - 0x57;
    }
    else if ((byte)(local_8 + 0xbfU) < 0x1a) {
      uVar6 = (int)local_8 - 0x37;
    }
    else {
      uVar6 = 0xffffffff;
    }
    if (uVar9 <= uVar6) break;
    uVar6 = local_10 * uVar9 + uVar6;
    bVar7 = bVar7 | (uVar6 < local_10 * uVar9 || (uint)(0xffffffff / (ulonglong)uVar9) < local_10)
                    << 2 | 8U;
    local_8 = *param_2;
    param_2 = param_2 + 1;
    local_10 = uVar6;
  }
  __crt_strtox::c_string_character_source<char>::unget
            ((c_string_character_source<char> *)&param_2,local_8);
  if ((bVar7 & 8) == 0) {
    if (param_3 == (undefined4 *)0x0) {
      return 0;
    }
    *param_3 = pcVar2;
    return 0;
  }
  bVar3 = FUN_005489e6(bVar7,local_10);
  if (bVar3) {
    *(undefined1 *)(param_1 + 7) = 1;
    param_1[6] = 0x22;
    if ((bVar7 & 1) != 0) {
      if ((bVar7 & 2) == 0) {
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
  else if ((bVar7 & 2) != 0) {
    local_10 = -local_10;
  }
  if (param_3 != (undefined4 *)0x0) {
    *param_3 = param_2;
    return local_10;
  }
  return local_10;
}

