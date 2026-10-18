/* Entry: 0x0051ac30; symbol: FUN_0051ac30; body bytes: 482 */

bool __thiscall FUN_0051ac30(void *this,char param_1)

{
  bool bVar1;
  undefined1 uVar2;
  byte bVar3;
  uint uVar4;
  int *piVar5;
  undefined3 extraout_var;
  basic_streambuf<char,struct_std::char_traits<char>_> *pbVar6;
  int iVar7;
  int *unaff_FS_OFFSET;
  uint uStack_60;
  undefined1 local_50 [8];
  basic_ios<char,struct_std::char_traits<char>_> *local_48;
  void *local_40;
  uint local_3c;
  uint local_38;
  locale *local_34;
  locale *local_30;
  void *local_2c;
  basic_ios<char,struct_std::char_traits<char>_> *local_28;
  int *local_24;
  uint local_20;
  int *local_1c;
  undefined1 local_16;
  char local_15;
  undefined1 *local_14;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_0056acfd;
  local_10 = *unaff_FS_OFFSET;
  uStack_60 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_14 = (undefined1 *)&uStack_60;
  *unaff_FS_OFFSET = (int)&local_10;
                    /* WARNING: Load size is inaccurate */
  local_1c = (int *)this;
  bVar1 = FUN_0051ba80((int)this + *(int *)(*this + 4));
  if (bVar1) {
    local_24 = (int *)FUN_0051ccd0((int)local_1c + *(int *)(*local_1c + 4));
    if (local_24 != (int *)0x0) {
      FUN_0051b730(local_24);
    }
    local_15 = '\0';
    if ((param_1 == '\0') &&
       (uVar4 = FUN_0041ca90((int)local_1c + *(int *)(*local_1c + 4)), (uVar4 & 1) != 0)) {
      local_2c = (void *)((int)local_1c + *(int *)(*local_1c + 4));
      local_34 = (locale *)FUN_0051ba60(local_2c,local_50);
      local_8 = 0;
      local_30 = local_34;
      local_40 = (void *)FUN_005156d0(local_34);
      local_8 = 0xffffffff;
      FUN_004f5600((int)local_50);
      local_8 = 1;
      piVar5 = (int *)FUN_00438520((int)local_1c + *(int *)(*local_1c + 4));
      uVar2 = FUN_0051cb20(piVar5);
      local_38 = CONCAT31(extraout_var,uVar2);
      local_20 = local_38;
      while( true ) {
        uVar4 = local_20;
        iVar7 = eof(local_20);
        bVar1 = FUN_0051b5b0(iVar7,uVar4);
        if (bVar1) break;
        bVar3 = FUN_004ed5a0((char)local_20);
        local_16 = FUN_0051bb50(local_40,0x48,bVar3);
        if (!(bool)local_16) goto LAB_0051adc2;
        pbVar6 = (basic_streambuf<char,struct_std::char_traits<char>_> *)
                 FUN_00438520((int)local_1c + *(int *)(*local_1c + 4));
        local_3c = FUN_0051cb90(pbVar6);
        local_20 = local_3c;
      }
      local_15 = '\x01';
LAB_0051adc2:
      local_8 = 0xffffffff;
    }
    if (local_15 != '\0') {
      local_48 = (basic_ios<char,struct_std::char_traits<char>_> *)
                 ((int)local_1c + *(int *)(*local_1c + 4));
      std::basic_ios<char,struct_std::char_traits<char>_>::setstate(local_48,3,false);
    }
    bVar1 = FUN_0051ba80((int)local_1c + *(int *)(*local_1c + 4));
  }
  else {
    local_28 = (basic_ios<char,struct_std::char_traits<char>_> *)
               ((int)local_1c + *(int *)(*local_1c + 4));
    std::basic_ios<char,struct_std::char_traits<char>_>::setstate(local_28,2,false);
    bVar1 = false;
  }
  *unaff_FS_OFFSET = local_10;
  return bVar1;
}

