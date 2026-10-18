/* Entry: 0x0041d350; symbol: FUN_0041d350; body bytes: 1156 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void FUN_0041d350(HWND param_1,uint param_2,uint param_3,LPARAM param_4)

{
  HDC hDC;
  HBRUSH hbr;
  int iVar1;
  undefined4 uVar2;
  HCURSOR pHVar3;
  tagPAINTSTRUCT local_48;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  if (0x1c < param_2) {
    switch(param_2) {
    case 0x20:
      iVar1 = FUN_00415800(0x5c4d40);
      if (((iVar1 == 0) || (iVar1 = FUN_0041cc70(0x5b6758), iVar1 == 8)) ||
         (iVar1 = FUN_0041cc70(0x5b6758), iVar1 == 9)) {
        if (DAT_005b67c1 == '\0') {
          do {
            iVar1 = ShowCursor(0);
          } while (-1 < iVar1);
          SetCursor((HCURSOR)0x0);
        }
        else {
          pHVar3 = LoadCursorW((HINSTANCE)0x0,(LPCWSTR)0x7f00);
          SetCursor(pHVar3);
          do {
            iVar1 = ShowCursor(1);
          } while (iVar1 < 0);
        }
      }
      else {
        pHVar3 = LoadCursorW((HINSTANCE)0x0,(LPCWSTR)0x7f00);
        SetCursor(pHVar3);
        ShowCursor(1);
      }
      goto LAB_0041d7d0;
    case 0x7e:
      iVar1 = FUN_00415800(0x5c4d40);
      if (iVar1 == 0) {
        Sleep(3000);
      }
      _DAT_005b67c4 = 300;
      break;
    case 0x104:
      if (param_3 == 0xd) {
        FUN_0041de00(&DAT_005b6758,1);
        uVar2 = FUN_0041cc70(0x5b6758);
        switch(uVar2) {
        case 0:
          FUN_0041e030(&DAT_005b6758,3);
          break;
        case 1:
          FUN_0041e030(&DAT_005b6758,4);
          break;
        case 2:
          if (DAT_005c4f8a == '\x04') {
            FUN_0041e030(&DAT_005b6758,7);
          }
          else if (DAT_005c4f8a == '\x03') {
            FUN_0041e030(&DAT_005b6758,6);
          }
          else {
            FUN_0041e030(&DAT_005b6758,5);
          }
          break;
        case 3:
          FUN_0041e030(&DAT_005b6758,0);
          break;
        case 4:
          FUN_0041e030(&DAT_005b6758,1);
          break;
        case 5:
          FUN_0041e030(&DAT_005b6758,2);
          break;
        case 6:
          FUN_0041e030(&DAT_005b6758,2);
          break;
        case 7:
          FUN_0041e030(&DAT_005b6758,2);
          break;
        case 8:
        case 9:
          switch(DAT_005c4f8a) {
          case '\0':
            FUN_0041e030(&DAT_005b6758,3);
            break;
          case '\x01':
            FUN_0041e030(&DAT_005b6758,4);
            break;
          case '\x02':
            FUN_0041e030(&DAT_005b6758,5);
            break;
          case '\x03':
            FUN_0041e030(&DAT_005b6758,6);
            break;
          case '\x04':
            FUN_0041e030(&DAT_005b6758,7);
            break;
          case '\x05':
            FUN_0041e030(&DAT_005b6758,5);
          }
        }
      }
      break;
    case 0x112:
      if (((param_3 & 0xfff0) == 0xf090) || ((param_3 & 0xfff0) == 0xf100)) goto LAB_0041d7d0;
    }
    goto switchD_0041d393_caseD_6;
  }
  if (param_2 == 0x1c) {
    DAT_005b67c0 = param_3 != 0;
    if ((bool)DAT_005b67c0) {
      DAT_005b67c1 = '\0';
      SetWindowPos(param_1,(HWND)0xffffffff,0,0,0,0,3);
    }
    else {
      DAT_005b67c1 = '\x01';
      SetWindowPos(param_1,(HWND)0xfffffffe,0,0,0,0,3);
    }
    goto switchD_0041d393_caseD_6;
  }
  switch(param_2) {
  case 5:
    if (((DAT_005b87e8 & 1) != 0) && (param_3 == 2)) {
      FUN_0041de00(&DAT_005b6758,1);
      uVar2 = FUN_0041cc70(0x5b6758);
      switch(uVar2) {
      case 3:
        FUN_0041e030(&DAT_005b6758,0);
        break;
      case 4:
        FUN_0041e030(&DAT_005b6758,1);
        break;
      default:
        FUN_0041e030(&DAT_005b6758,2);
        break;
      case 6:
        FUN_0041e030(&DAT_005b6758,2);
      }
    }
    break;
  case 0xf:
    if ((DAT_005b87e8 >> 7 & 1) == 0) {
      hDC = BeginPaint(param_1,&local_48);
      hbr = CreateSolidBrush(0);
      FillRect(hDC,&local_48.rcPaint,hbr);
      DeleteObject(hbr);
      EndPaint(param_1,&local_48);
      DAT_005b87e8 = DAT_005b87e8 | 0x80;
    }
    break;
  case 0x10:
    DAT_005c5888 = DAT_005c5888 & 0xffffff9f | 0x20;
    goto LAB_0041d7d0;
  case 0x14:
    goto LAB_0041d7d0;
  }
switchD_0041d393_caseD_6:
  DefWindowProcW(param_1,param_2,param_3,param_4);
LAB_0041d7d0:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

