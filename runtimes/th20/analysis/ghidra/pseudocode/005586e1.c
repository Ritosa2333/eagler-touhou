/* Entry: 0x005586e1; symbol: FUN_005586e1; body bytes: 828 */

int __cdecl
FUN_005586e1(uint *param_1,undefined1 (*param_2) [16],uint param_3,char *param_4,int param_5,
            size_t param_6,byte param_7,int param_8,int param_9,int *param_10)

{
  char *pcVar1;
  char cVar2;
  short sVar3;
  ushort uVar4;
  int iVar5;
  undefined1 (*pauVar6) [16];
  char *pcVar7;
  uint extraout_ECX;
  short sVar8;
  uint uVar9;
  uint uVar10;
  size_t _Size;
  uint uVar11;
  char *_Dst;
  char *pcVar12;
  bool bVar13;
  ulonglong uVar14;
  undefined8 uVar15;
  longlong lVar16;
  char local_28;
  uint local_20;
  uint local_1c;
  uint local_14;
  
  _Size = 0;
  if (-1 < (int)param_6) {
    _Size = param_6;
  }
  (*param_2)[0] = 0;
  if (param_3 <= _Size + 0xb) {
    *(undefined1 *)(param_10 + 7) = 1;
    param_10[6] = 0x22;
    FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,param_10);
    return 0x22;
  }
  if ((param_1[1] >> 0x14 & 0x7ff) == 0x7ff) {
    iVar5 = FUN_00558a1d(param_1,(char *)param_2,param_3,param_4,param_5,_Size,'\0',param_8,param_9,
                         param_10);
    if (iVar5 != 0) {
      (*param_2)[0] = 0;
      return iVar5;
    }
    pauVar6 = FUN_005448c0(param_2,0x65);
    if (pauVar6 == (undefined1 (*) [16])0x0) {
      return 0;
    }
    (*pauVar6)[0] = (param_7 ^ 1) * ' ' + 'P';
    (*pauVar6)[3] = 0;
    return 0;
  }
  if (((int)param_1[1] < 1) && ((int)param_1[1] < 0)) {
    (*param_2)[0] = 0x2d;
    param_2 = (undefined1 (*) [16])(*param_2 + 1);
  }
  sVar3 = (ushort)(param_7 ^ 1) * 0x20 + 7;
  pcVar1 = *param_2 + 1;
  local_14 = 0x3ff;
  if ((param_1[1] & 0x7ff00000) == 0) {
    (*param_2)[0] = 0x30;
    if (*param_1 == 0 && (param_1[1] & 0xfffff) == 0) {
      local_14 = 0;
    }
    else {
      local_14 = 0x3fe;
    }
  }
  else {
    (*param_2)[0] = 0x31;
  }
  _Dst = *param_2 + 2;
  if (_Size == 0) {
    cVar2 = '\0';
  }
  else {
    if ((char)param_10[5] == '\0') {
      FUN_0054a360(param_10);
    }
    cVar2 = *(char *)**(undefined4 **)(param_10[3] + 0x88);
  }
  *pcVar1 = cVar2;
  if (((param_1[1] & 0xfffff) != 0) || (*param_1 != 0)) {
    sVar8 = 0x30;
    local_20 = 0xf0000;
    local_1c = 0;
    do {
      if ((int)_Size < 1) {
        uVar9 = FUN_00558ee9(param_1,local_1c,local_20,sVar8,param_9);
        pcVar12 = _Dst;
        if ((char)uVar9 != '\0') goto LAB_005588e6;
        goto LAB_00558924;
      }
      uVar14 = __aullshr((byte)sVar8,param_1[1] & local_20);
      uVar4 = (short)uVar14 + 0x30;
      if (0x39 < uVar4) {
        uVar4 = uVar4 + sVar3;
      }
      *_Dst = (char)uVar4;
      sVar8 = sVar8 + -4;
      _Dst = _Dst + 1;
      local_1c = local_1c >> 4 | local_20 << 0x1c;
      local_20 = local_20 >> 4;
      _Size = _Size - 1;
    } while (-1 < sVar8);
  }
LAB_00558910:
  if (0 < (int)_Size) {
    _memset(_Dst,0x30,_Size);
    _Dst = _Dst + _Size;
  }
LAB_00558924:
  if (*pcVar1 == '\0') {
    _Dst = pcVar1;
  }
  *_Dst = (param_7 ^ 1) * ' ' + 'P';
  uVar14 = __aullshr(0x34,param_1[1]);
  pcVar1 = _Dst + 2;
  uVar9 = (uint)uVar14 & 0x7ff;
  uVar10 = uVar9 - local_14;
  uVar9 = (uint)(uVar9 < local_14);
  uVar11 = -uVar9;
  if (uVar9 == 0) {
    cVar2 = '+';
  }
  else {
    bVar13 = uVar10 != 0;
    uVar10 = -uVar10;
    uVar11 = -(uVar11 + bVar13);
    cVar2 = '-';
  }
  _Dst[1] = cVar2;
  *pcVar1 = '0';
  pcVar12 = pcVar1;
  if ((int)uVar11 < 0) goto LAB_00558a0b;
  if (((int)uVar11 < 1) && (uVar9 = uVar10, uVar10 < 1000)) {
LAB_005589ae:
    uVar10 = uVar9;
    lVar16 = CONCAT44(uVar11,uVar10);
    if ((int)uVar11 < 0) goto LAB_00558a0b;
    uVar9 = uVar10;
    if ((0 < (int)uVar11) || (99 < uVar10)) goto LAB_005589b9;
LAB_005589e0:
    uVar10 = (uint)lVar16;
    if ((lVar16 < 0) || (((int)((ulonglong)lVar16 >> 0x20) == 0 || lVar16 < 0 && (uVar10 < 10))))
    goto LAB_00558a0b;
  }
  else {
    uVar15 = __alldvrm(uVar10,uVar11,1000,0);
    pcVar12 = _Dst + 3;
    *pcVar1 = (char)uVar15 + '0';
    uVar9 = extraout_ECX;
    uVar11 = uVar10;
    if (pcVar12 == pcVar1) goto LAB_005589ae;
LAB_005589b9:
    uVar15 = __alldiv(uVar9,uVar11,100,0);
    *pcVar12 = (char)uVar15 + '0';
    pcVar12 = pcVar12 + 1;
    lVar16 = __allrem(uVar9,uVar11,100,0);
    if (pcVar12 == pcVar1) goto LAB_005589e0;
  }
  uVar9 = (uint)((ulonglong)lVar16 >> 0x20);
  uVar15 = __alldiv((uint)lVar16,uVar9,10,0);
  *pcVar12 = (char)uVar15 + '0';
  pcVar12 = pcVar12 + 1;
  uVar15 = __allrem((uint)lVar16,uVar9,10,0);
  uVar10 = (uint)uVar15;
LAB_00558a0b:
  *pcVar12 = (char)uVar10 + '0';
  pcVar12[1] = '\0';
  return 0;
LAB_005588e6:
  while( true ) {
    pcVar7 = pcVar12 + -1;
    local_28 = *pcVar7;
    if ((local_28 != 'f') && (local_28 != 'F')) break;
    *pcVar7 = '0';
    pcVar12 = pcVar7;
  }
  if (pcVar7 == pcVar1) {
    pcVar12[-2] = pcVar12[-2] + '\x01';
  }
  else {
    if (local_28 == '9') {
      local_28 = (char)sVar3;
      local_28 = local_28 + '9';
    }
    *pcVar7 = local_28 + '\x01';
  }
  goto LAB_00558910;
}

