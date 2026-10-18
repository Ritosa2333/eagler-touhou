/* Entry: 0x005590fc; symbol: FUN_005590fc; body bytes: 356 */

int __thiscall
FUN_005590fc(void *this,int *param_1,byte *param_2,uint param_3,WCHAR param_4,int *param_5)

{
  uint uVar1;
  int *piVar2;
  DWORD DVar3;
  int iVar4;
  undefined8 local_c;
  
  piVar2 = param_5;
  local_c = CONCAT44(this,this);
  if ((param_2 == (byte *)0x0) && (param_3 != 0)) {
    if (param_1 == (int *)0x0) {
      return 0;
    }
    *param_1 = 0;
    return 0;
  }
  if (param_1 != (int *)0x0) {
    *param_1 = -1;
  }
  if (param_3 < 0x80000000) {
    if ((char)param_5[5] == '\0') {
      FUN_0054a360(param_5);
    }
    uVar1 = *(uint *)(piVar2[3] + 8);
    if (uVar1 == 0xfde9) {
      local_c = 0;
      iVar4 = FUN_00563ceb(param_2,(uint)(ushort)param_4,(undefined4 *)&local_c,(int)piVar2);
      if (param_1 != (int *)0x0) {
        *param_1 = iVar4;
      }
      if (iVar4 < 5) {
        return 0;
      }
      if ((char)piVar2[7] == '\0') {
        return 0;
      }
      return piVar2[6];
    }
    if (*(int *)(piVar2[3] + 0xa8) == 0) {
      if (0xff < (ushort)param_4) {
        if ((param_2 != (byte *)0x0) && (param_3 != 0)) {
          _memset(param_2,0,param_3);
        }
        goto LAB_005591ba;
      }
      if (param_2 == (byte *)0x0) {
LAB_005591d3:
        if (param_1 != (int *)0x0) {
          *param_1 = 1;
        }
        return 0;
      }
      if (param_3 != 0) {
        *param_2 = (byte)param_4;
        goto LAB_005591d3;
      }
    }
    else {
      local_c = local_c & 0xffffffff;
      iVar4 = FUN_0055de4b(uVar1,0,&param_4,1,(LPSTR)param_2,param_3,(LPCSTR)0x0,
                           (LPBOOL)((int)&local_c + 4));
      if (iVar4 != 0) {
        if (local_c._4_4_ == 0) {
          if (param_1 == (int *)0x0) {
            return 0;
          }
          *param_1 = iVar4;
          return 0;
        }
LAB_005591ba:
        piVar2[6] = 0x2a;
        *(undefined1 *)(piVar2 + 7) = 1;
        return 0x2a;
      }
      DVar3 = GetLastError();
      if (DVar3 != 0x7a) goto LAB_005591ba;
      if ((param_2 != (byte *)0x0) && (param_3 != 0)) {
        _memset(param_2,0,param_3);
      }
    }
    iVar4 = 0x22;
  }
  else {
    iVar4 = 0x16;
  }
  piVar2[6] = iVar4;
  *(undefined1 *)(piVar2 + 7) = 1;
  FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,piVar2);
  return iVar4;
}

