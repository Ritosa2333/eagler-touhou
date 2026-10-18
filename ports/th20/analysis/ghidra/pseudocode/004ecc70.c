/* Entry: 0x004ecc70; symbol: FUN_004ecc70; body bytes: 623 */

void __cdecl FUN_004ecc70(char *param_1,char *param_2,byte param_3,int param_4)

{
  char *pcVar1;
  int *unaff_FS_OFFSET;
  byte local_29;
  char *local_28;
  byte local_21;
  char local_19 [5];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_0056a0d0;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  std::_Adl_verify_range<char*,char_const*>(&param_1,&param_2);
  local_21 = param_3;
  pcVar1 = local_19 + 1;
  local_28 = pcVar1;
  switch(param_4) {
  case 2:
    do {
      local_28 = local_28 + -1;
      *local_28 = (local_21 & 1) + 0x30;
      local_21 = local_21 >> 1;
    } while (local_21 != 0);
    break;
  case 3:
  case 5:
  case 6:
  case 7:
  case 9:
    do {
      local_28 = local_28 + -1;
      *local_28 = (char)((longlong)(ulonglong)local_21 % (longlong)param_4) + '0';
      local_21 = (byte)((longlong)(ulonglong)local_21 / (longlong)param_4);
    } while (local_21 != 0);
    break;
  case 4:
    do {
      local_28 = local_28 + -1;
      *local_28 = (local_21 & 3) + 0x30;
      local_21 = local_21 >> 2;
    } while (local_21 != 0);
    break;
  case 8:
    do {
      local_28 = local_28 + -1;
      *local_28 = (local_21 & 7) + 0x30;
      local_21 = local_21 >> 3;
    } while (local_21 != 0);
    break;
  case 10:
    local_29 = param_3;
    do {
      local_28 = local_28 + -1;
      *local_28 = local_29 % 10 + 0x30;
      local_29 = local_29 / 10;
    } while (local_29 != 0);
    break;
  default:
    do {
      local_28 = local_28 + -1;
      *local_28 = "0123456789abcdefghijklmnopqrstuvwxyz---------------------------------------------------------- \r\n"
                  [(int)((longlong)(ulonglong)local_21 % (longlong)param_4)];
      local_21 = (byte)((longlong)(ulonglong)local_21 / (longlong)param_4);
    } while (local_21 != 0);
    break;
  case 0x10:
    do {
      local_28 = local_28 + -1;
      *local_28 = "0123456789abcdefghijklmnopqrstuvwxyz---------------------------------------------------------- \r\n"
                  [local_21 & 0xf];
      local_21 = local_21 >> 4;
    } while (local_21 != 0);
    break;
  case 0x20:
    do {
      local_28 = local_28 + -1;
      *local_28 = "0123456789abcdefghijklmnopqrstuvwxyz---------------------------------------------------------- \r\n"
                  [local_21 & 0x1f];
      local_21 = local_21 >> 5;
    } while (local_21 != 0);
  }
  if ((int)pcVar1 - (int)local_28 <= (int)param_2 - (int)param_1) {
    FUN_00452490(local_28,(int)pcVar1 - (int)local_28,param_1);
  }
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

