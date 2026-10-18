/* Entry: 0x00549935; symbol: FUN_00549935; body bytes: 345 */

uint __fastcall FUN_00549935(__uint64 *param_1)

{
  char cVar1;
  char *pcVar2;
  __uint64 _Var3;
  undefined4 in_EAX;
  undefined3 uVar6;
  char *pcVar4;
  uint uVar5;
  
  cVar1 = *(char *)((int)param_1 + 0x2d);
  uVar6 = (undefined3)((uint)in_EAX >> 8);
  pcVar4 = (char *)CONCAT31(uVar6,cVar1);
  if (cVar1 == 'F') {
    pcVar4 = (char *)((uint)*param_1 & 8);
    if (pcVar4 == (char *)0x0) {
      *(undefined1 *)(param_1 + 3) = 7;
      uVar5 = FUN_00549a8e(param_1);
      return uVar5;
    }
  }
  else if (cVar1 == 'N') {
    pcVar4 = (char *)((uint)*param_1 & 8);
    if (pcVar4 == (char *)0x0) {
      *(undefined1 *)(param_1 + 3) = 8;
      goto LAB_0054996a;
    }
  }
  else {
    if ((int)param_1[5] != 0) {
LAB_0054996a:
      _Var3 = param_1[1];
      *(undefined1 *)((int)_Var3 + 0x1c) = 1;
      *(undefined4 *)((int)_Var3 + 0x18) = 0x16;
      uVar5 = FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,*(int **)(param_1 + 1));
      return uVar5 & 0xffffff00;
    }
    if (cVar1 < 'k') {
      if (cVar1 == 'j') {
        *(undefined4 *)(param_1 + 5) = 5;
        goto LAB_00549a8b;
      }
      if (cVar1 == 'I') {
        pcVar2 = *(char **)((int)param_1 + 0xc);
        cVar1 = *pcVar2;
        pcVar4 = (char *)CONCAT31(uVar6,cVar1);
        if ((cVar1 == '3') && (pcVar2[1] == '2')) {
          pcVar4 = pcVar2 + 2;
          *(undefined4 *)(param_1 + 5) = 10;
          *(char **)((int)param_1 + 0xc) = pcVar4;
        }
        else if (cVar1 == '6') {
          if (pcVar2[1] == '4') {
            pcVar4 = pcVar2 + 2;
            *(undefined4 *)(param_1 + 5) = 0xb;
            *(char **)((int)param_1 + 0xc) = pcVar4;
          }
        }
        else if ((((cVar1 == 'd') || (cVar1 == 'i')) || (cVar1 == 'o')) ||
                (((cVar1 == 'u' || (cVar1 == 'x')) || (cVar1 == 'X')))) {
          *(undefined4 *)(param_1 + 5) = 9;
        }
        goto LAB_00549a8b;
      }
      if (cVar1 == 'L') {
        *(undefined4 *)(param_1 + 5) = 8;
        goto LAB_00549a8b;
      }
      if (cVar1 == 'T') {
        *(undefined4 *)(param_1 + 5) = 0xd;
        goto LAB_00549a8b;
      }
      if (cVar1 != 'h') goto LAB_00549a8b;
      if (**(char **)((int)param_1 + 0xc) == 'h') {
        *(char **)((int)param_1 + 0xc) = *(char **)((int)param_1 + 0xc) + 1;
        pcVar4 = (char *)0x1;
      }
      else {
        pcVar4 = (char *)0x2;
      }
    }
    else {
      if (cVar1 != 'l') {
        if (cVar1 == 't') {
          *(undefined4 *)(param_1 + 5) = 7;
        }
        else if (cVar1 == 'w') {
          *(undefined4 *)(param_1 + 5) = 0xc;
        }
        else if (cVar1 == 'z') {
          *(undefined4 *)(param_1 + 5) = 6;
        }
        goto LAB_00549a8b;
      }
      if (**(char **)((int)param_1 + 0xc) == 'l') {
        *(char **)((int)param_1 + 0xc) = *(char **)((int)param_1 + 0xc) + 1;
        pcVar4 = (char *)0x4;
      }
      else {
        pcVar4 = (char *)0x3;
      }
    }
    *(char **)(param_1 + 5) = pcVar4;
  }
LAB_00549a8b:
  return CONCAT31((int3)((uint)pcVar4 >> 8),1);
}

