/* Entry: 0x004524f0; symbol: FUN_004524f0; body bytes: 659 */

void __cdecl FUN_004524f0(char *param_1,char *param_2,uint param_3,uint param_4)

{
  char *pcVar1;
  int *unaff_FS_OFFSET;
  uint local_48;
  char *local_40;
  uint local_3c;
  char local_19 [5];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00568760;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  std::_Adl_verify_range<char*,char_const*>(&param_1,&param_2);
  local_3c = param_3;
  if ((int)param_3 < 0) {
    if (param_1 == param_2) goto LAB_0045276a;
    *param_1 = '-';
    param_1 = param_1 + 1;
    local_3c = -param_3;
  }
  pcVar1 = local_19 + 1;
  local_40 = pcVar1;
  switch(param_4) {
  case 2:
    do {
      local_40 = local_40 + -1;
      *local_40 = ((byte)local_3c & 1) + 0x30;
      local_3c = local_3c >> 1;
    } while (local_3c != 0);
    break;
  case 3:
  case 5:
  case 6:
  case 7:
  case 9:
    do {
      local_40 = local_40 + -1;
      *local_40 = (char)(local_3c % param_4) + '0';
      local_3c = local_3c / param_4;
    } while (local_3c != 0);
    break;
  case 4:
    do {
      local_40 = local_40 + -1;
      *local_40 = ((byte)local_3c & 3) + 0x30;
      local_3c = local_3c >> 2;
    } while (local_3c != 0);
    break;
  case 8:
    do {
      local_40 = local_40 + -1;
      *local_40 = ((byte)local_3c & 7) + 0x30;
      local_3c = local_3c >> 3;
    } while (local_3c != 0);
    break;
  case 10:
    local_48 = local_3c;
    do {
      local_40 = local_40 + -1;
      *local_40 = (char)((ulonglong)local_48 % 10) + '0';
      local_48 = local_48 / 10;
    } while (local_48 != 0);
    break;
  default:
    do {
      local_40 = local_40 + -1;
      *local_40 = "0123456789abcdefghijklmnopqrstuvwxyz---------------------------------------------------------- \r\n"
                  [local_3c % param_4];
      local_3c = local_3c / param_4;
    } while (local_3c != 0);
    break;
  case 0x10:
    do {
      local_40 = local_40 + -1;
      *local_40 = "0123456789abcdefghijklmnopqrstuvwxyz---------------------------------------------------------- \r\n"
                  [local_3c & 0xf];
      local_3c = local_3c >> 4;
    } while (local_3c != 0);
    break;
  case 0x20:
    do {
      local_40 = local_40 + -1;
      *local_40 = "0123456789abcdefghijklmnopqrstuvwxyz---------------------------------------------------------- \r\n"
                  [local_3c & 0x1f];
      local_3c = local_3c >> 5;
    } while (local_3c != 0);
  }
  if ((int)pcVar1 - (int)local_40 <= (int)param_2 - (int)param_1) {
    FUN_00452490(local_40,(int)pcVar1 - (int)local_40,param_1);
  }
LAB_0045276a:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

