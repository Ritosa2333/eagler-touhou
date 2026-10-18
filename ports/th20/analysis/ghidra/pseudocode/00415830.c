/* Entry: 0x00415830; symbol: FUN_00415830; body bytes: 2227 */

/* WARNING: Removing unreachable block (ram,0x00415e89) */
/* WARNING: Removing unreachable block (ram,0x00415da1) */
/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __cdecl
FUN_00415830(int *param_1,int param_2,undefined4 param_3,undefined4 param_4,LPCSTR param_5,
            undefined4 param_6,int param_7,int param_8,int param_9)

{
  undefined4 uVar1;
  undefined4 *puVar2;
  int *unaff_FS_OFFSET;
  format_string_parser<wchar_t> local_320 [88];
  undefined1 local_2c8 [40];
  LONG local_2a0;
  LONG local_29c;
  undefined4 *local_298;
  undefined4 *local_294;
  HGDIOBJ local_290;
  HGDIOBJ local_28c;
  undefined4 local_288;
  undefined4 local_284;
  uint local_280;
  int local_27c;
  int local_278;
  int local_274;
  int local_270;
  int local_26c;
  uint local_268;
  int local_264;
  undefined1 local_25f;
  undefined1 local_25e;
  undefined1 local_25d;
  HDC local_25c;
  int local_258;
  float local_254;
  void *local_250;
  tagSIZE local_24c;
  undefined4 local_244;
  undefined4 local_240;
  int local_23c;
  int local_238;
  int local_234;
  int local_230;
  undefined4 local_22c;
  undefined4 local_228;
  int local_224;
  int local_220;
  WCHAR local_21c [2];
  WCHAR local_218 [258];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  int local_8;
  
  puStack_c = &LAB_00567dd9;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  local_8 = 0;
  uVar1 = FUN_0040c310(&DAT_005c0240,0x12);
  FUN_0040b730(&local_244,uVar1);
  local_8 = CONCAT31(local_8._1_3_,1);
  puVar2 = (undefined4 *)FUN_00414580(&DAT_005b66f0,param_7);
  local_290 = (HGDIOBJ)*puVar2;
  local_254 = DAT_0056c8d0;
  local_258 = 2;
  local_250 = FUN_00413c50();
  local_27c = param_7;
  switch(param_7) {
  default:
    local_254 = DAT_0056c8d0;
    local_258 = 3;
    break;
  case 1:
    local_254 = DAT_0056c8d0;
    local_258 = 3;
    break;
  case 2:
    local_254 = DAT_0056c8d0;
    local_258 = 3;
    break;
  case 3:
    local_254 = DAT_0056c8d0;
    local_258 = 3;
    break;
  case 4:
    local_254 = DAT_0056c8d4;
    local_258 = 3;
    break;
  case 5:
    local_254 = DAT_0056c8d4;
    local_258 = 3;
    break;
  case 6:
  case 8:
    local_254 = DAT_0056c8d8;
    local_258 = 3;
    break;
  case 9:
    local_254 = DAT_0056c8dc;
    local_258 = 3;
    break;
  case 10:
    local_254 = DAT_0056c8e0;
    local_258 = 4;
    break;
  case 0xc:
  case 0xd:
    local_254 = DAT_0056c8d0;
    local_258 = 6;
    break;
  case 0xe:
    local_254 = DAT_0056c8d8;
    local_258 = 6;
    break;
  case 0xf:
    local_254 = DAT_0056c8d8;
    local_258 = 6;
    break;
  case 0x10:
    local_254 = DAT_0056c8e0;
    local_258 = 6;
    break;
  case 0x11:
    local_254 = DAT_0056c8e0;
    local_258 = 6;
    break;
  case 0x12:
  case 0x13:
    local_254 = DAT_0056c8d0;
    local_258 = 2;
    break;
  case 0x14:
    local_254 = DAT_0056c8d8;
    local_258 = 7;
    break;
  case 0x15:
    local_254 = DAT_0056c8d8;
    local_258 = 7;
  }
  local_254 = local_254 * _DAT_005ae120;
  _DAT_005ae120 = (float)DAT_0056c8cc;
  _memset(local_218,0,0x202);
  MultiByteToWideChar(0x3a4,0,param_5,-1,local_218,0x100);
  local_274 = (param_1[2] - *param_1) + 3 + (int)local_254;
  local_278 = (param_1[3] - param_1[1]) + 3 + (int)local_254;
  FUN_004153d0(local_250,local_274,local_278 + 6,0x15);
  local_25c = (HDC)FUN_00415620((int)local_250);
  local_28c = SelectObject(local_25c,local_290);
  local_25d = (undefined1)((uint)param_3 >> 0x10);
  local_25e = (undefined1)((uint)param_3 >> 8);
  local_25f = (undefined1)param_3;
  local_280 = (uint)CONCAT12(local_25f,CONCAT11(local_25e,local_25d));
  SetBkMode(local_25c,1);
  local_268 = FUN_00548300((undefined1 (*) [32])local_218);
  GetTextExtentPoint32W(local_25c,local_218,local_268,&local_24c);
  if (param_8 == 0) {
    SetTextColor(local_25c,local_280);
    if (param_9 != 0) {
      TextOutW(local_25c,param_2 + 2,local_258,local_218,local_268);
      FUN_00416420(local_250,local_278 + 6,0,local_274,param_4,local_254);
    }
    TextOutW(local_25c,param_2 + 2,local_258,local_218,local_268);
  }
  else {
    local_264 = param_2;
    local_21c[0] = L'\0';
    local_21c[1] = L'\0';
    SetTextColor(local_25c,local_280);
    if (param_9 != 0) {
      for (local_26c = 0; local_26c < (int)local_268; local_26c = local_26c + 1) {
        local_21c[0] = local_218[local_26c];
        local_284 = 2;
                    /* WARNING: Ignoring partial resolution of indirect */
        local_21c[1] = 0;
        TextOutW(local_25c,local_264,local_258,local_21c,1);
        local_264 = local_264 + param_8;
      }
      FUN_00416420(local_250,local_278 + 6,0,local_274,param_4,local_254);
    }
    local_264 = param_2;
    for (local_270 = 0; local_270 < (int)local_268; local_270 = local_270 + 1) {
      local_21c[0] = local_218[local_270];
      local_288 = 2;
                    /* WARNING: Ignoring partial resolution of indirect */
      local_21c[1] = 0;
      TextOutW(local_25c,local_264,local_258,local_21c,1);
      local_264 = local_264 + param_8;
    }
  }
  SelectObject(local_25c,local_28c);
  FUN_00417ab0(local_250,local_278 + 6,0,local_274);
  SelectObject(local_25c,local_28c);
  uVar1 = FUN_0040c310(&DAT_005c0240,0x13);
  FUN_0040b730(&local_240,uVar1);
  local_8._0_1_ = 2;
  local_23c = *param_1;
  local_238 = param_1[1];
  local_234 = param_1[2];
  local_230 = param_1[3];
  local_22c = 0;
  local_228 = 0;
  local_224 = local_234 - local_23c;
  local_220 = local_230 - local_238;
  if (0x400 < local_224) {
    local_224 = 0x400;
  }
  if (((DAT_005b66ec != '\0') && (param_7 != 0x13)) && (param_7 != 0x12)) {
    local_228 = 6;
    local_220 = local_220 + 6;
  }
  puVar2 = (undefined4 *)move<>(&local_250);
  local_298 = FUN_00413ec0(local_320,*puVar2,&param_6,&local_23c,&local_22c,
                           (undefined4 *)&stack0x00000028,(int)&stack0x0000002c);
  local_8._0_1_ = 3;
  local_294 = local_298;
  FUN_00412f20(local_2c8,local_298);
  local_8._0_1_ = 4;
  FUN_004162f0(&DAT_005b66e0,local_2c8);
  local_8._0_1_ = 3;
  FUN_00414400(local_2c8);
  local_8._0_1_ = 2;
  __crt_stdio_input::format_string_parser<wchar_t>::~format_string_parser<wchar_t>(local_320);
  local_8._0_1_ = 1;
  FUN_0040b900(&local_240);
  local_2a0 = local_24c.cx;
  local_29c = local_24c.cy;
  local_8 = (uint)local_8._1_3_ << 8;
  FUN_0040b900(&local_244);
  local_8 = 0xffffffff;
  FUN_00414400(&stack0x0000002c);
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

