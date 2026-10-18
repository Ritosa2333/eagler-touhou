/* Entry: 0x004527d0; symbol: FUN_004527d0; body bytes: 1023 */

void __cdecl FUN_004527d0(char *param_1,char *param_2,int param_3,int param_4,uint param_5)

{
  bool bVar1;
  bool bVar2;
  uint uVar3;
  int *unaff_FS_OFFSET;
  undefined8 uVar4;
  longlong lVar5;
  ulonglong uVar6;
  uint local_78;
  int local_74;
  uint local_6c;
  char *local_64;
  undefined8 local_60;
  char local_18 [4];
  uint local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 uStack_8;
  
  uStack_8 = 0xffffffff;
  puStack_c = &LAB_00568790;
  local_10 = *unaff_FS_OFFSET;
  local_14 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  *unaff_FS_OFFSET = (int)&local_10;
  std::_Adl_verify_range<char*,char_const*>(&param_1,&param_2);
  local_60 = CONCAT44(param_4,param_3);
  if ((param_4 < 1) && (param_4 < 0)) {
    if (param_1 == param_2) goto LAB_00452bb6;
    *param_1 = '-';
    param_1 = param_1 + 1;
    local_60 = CONCAT44(-(uint)(param_3 != 0) - param_4,-param_3);
  }
  local_64 = local_18;
  switch(param_5) {
  case 2:
    do {
      local_64 = local_64 + -1;
      *local_64 = ((byte)local_60 & 1) + 0x30;
      uVar6 = __aullshr(1,local_60._4_4_);
      local_60._0_4_ = (uint)uVar6;
      local_60._4_4_ = (uint)(uVar6 >> 0x20);
      bVar1 = (uint)local_60 != 0;
      bVar2 = local_60._4_4_ != 0;
      local_60 = uVar6;
    } while (bVar1 || bVar2);
    break;
  case 3:
  case 5:
  case 6:
  case 7:
  case 9:
    do {
      local_64 = local_64 + -1;
      uVar4 = __aullrem((uint)local_60,local_60._4_4_,param_5,(int)param_5 >> 0x1f);
      *local_64 = (char)uVar4 + '0';
      uVar6 = __aulldiv((uint)local_60,local_60._4_4_,param_5,(int)param_5 >> 0x1f);
      local_60._0_4_ = (uint)uVar6;
      local_60._4_4_ = (uint)(uVar6 >> 0x20);
      bVar1 = (uint)local_60 != 0;
      bVar2 = local_60._4_4_ != 0;
      local_60 = uVar6;
    } while (bVar1 || bVar2);
    break;
  case 4:
    do {
      local_64 = local_64 + -1;
      *local_64 = ((byte)local_60 & 3) + 0x30;
      uVar6 = __aullshr(2,local_60._4_4_);
      local_60._0_4_ = (uint)uVar6;
      local_60._4_4_ = (uint)(uVar6 >> 0x20);
      bVar1 = (uint)local_60 != 0;
      bVar2 = local_60._4_4_ != 0;
      local_60 = uVar6;
    } while (bVar1 || bVar2);
    break;
  case 8:
    do {
      local_64 = local_64 + -1;
      *local_64 = ((byte)local_60 & 7) + 0x30;
      uVar6 = __aullshr(3,local_60._4_4_);
      local_60._0_4_ = (uint)uVar6;
      local_60._4_4_ = (uint)(uVar6 >> 0x20);
      bVar1 = (uint)local_60 != 0;
      bVar2 = local_60._4_4_ != 0;
      local_60 = uVar6;
    } while (bVar1 || bVar2);
    break;
  case 10:
    while( true ) {
      if (local_60._4_4_ == 0) break;
      uVar4 = __aullrem((uint)local_60,local_60._4_4_,1000000000,0);
      local_78 = (uint)uVar4;
      local_60 = __aulldiv((uint)local_60,local_60._4_4_,1000000000,0);
      for (local_74 = 0; local_74 != 9; local_74 = local_74 + 1) {
        local_64 = local_64 + -1;
        *local_64 = (char)((ulonglong)local_78 % 10) + '0';
        local_78 = local_78 / 10;
      }
    }
    local_6c = (uint)local_60;
    do {
      local_64 = local_64 + -1;
      *local_64 = (char)((ulonglong)local_6c % 10) + '0';
      local_6c = local_6c / 10;
    } while (local_6c != 0);
    break;
  default:
    do {
      local_64 = local_64 + -1;
      uVar4 = __aullrem((uint)local_60,local_60._4_4_,param_5,(int)param_5 >> 0x1f);
      lVar5 = __allmul((uint)uVar4,(int)((ulonglong)uVar4 >> 0x20),1,0);
      *local_64 = "0123456789abcdefghijklmnopqrstuvwxyz---------------------------------------------------------- \r\n"
                  [(int)lVar5];
      uVar6 = __aulldiv((uint)local_60,local_60._4_4_,param_5,(int)param_5 >> 0x1f);
      local_60._0_4_ = (uint)uVar6;
      local_60._4_4_ = (uint)(uVar6 >> 0x20);
      bVar1 = (uint)local_60 != 0;
      bVar2 = local_60._4_4_ != 0;
      local_60 = uVar6;
    } while (bVar1 || bVar2);
    break;
  case 0x10:
    do {
      local_64 = local_64 + -1;
      lVar5 = __allmul((uint)local_60 & 0xf,0,1,0);
      *local_64 = "0123456789abcdefghijklmnopqrstuvwxyz---------------------------------------------------------- \r\n"
                  [(int)lVar5];
      uVar6 = __aullshr(4,local_60._4_4_);
      local_60._0_4_ = (uint)uVar6;
      local_60._4_4_ = (uint)(uVar6 >> 0x20);
      bVar1 = (uint)local_60 != 0;
      bVar2 = local_60._4_4_ != 0;
      local_60 = uVar6;
    } while (bVar1 || bVar2);
    break;
  case 0x20:
    do {
      local_64 = local_64 + -1;
      lVar5 = __allmul((uint)local_60 & 0x1f,0,1,0);
      *local_64 = "0123456789abcdefghijklmnopqrstuvwxyz---------------------------------------------------------- \r\n"
                  [(int)lVar5];
      uVar6 = __aullshr(5,local_60._4_4_);
      local_60._0_4_ = (uint)uVar6;
      local_60._4_4_ = (uint)(uVar6 >> 0x20);
      bVar1 = (uint)local_60 != 0;
      bVar2 = local_60._4_4_ != 0;
      local_60 = uVar6;
    } while (bVar1 || bVar2);
  }
  uVar3 = (int)local_18 - (int)local_64;
  if ((int)uVar3 <= (int)param_2 - (int)param_1) {
    FUN_00452490(local_64,uVar3,param_1);
  }
LAB_00452bb6:
  *unaff_FS_OFFSET = local_10;
  FUN_005429ee(local_14 ^ (uint)&stack0xfffffffc);
  return;
}

