/* Entry: 0x004263e0; symbol: FUN_004263e0; body bytes: 890 */

void __thiscall FUN_004263e0(void *this,HWND param_1)

{
  undefined4 *puVar1;
  int iVar2;
  undefined4 uVar3;
  undefined4 local_2a0;
  undefined4 local_29c;
  undefined1 local_298 [512];
  undefined4 *local_98;
  uint local_94;
  default_delete<class_std::_Facet_base> *local_90;
  default_delete<class_std::_Facet_base> *local_8c;
  code *local_88;
  undefined4 local_84;
  code *local_80;
  undefined4 local_7c;
  code *local_78;
  undefined4 local_74;
  code *local_70;
  undefined4 local_6c;
  void *local_68;
  void *local_64;
  undefined *local_60;
  default_delete<class_std::_Facet_base> *local_5c;
  int local_58;
  undefined4 *local_54;
  undefined4 local_50;
  undefined4 local_4c;
  undefined4 local_48;
  undefined2 *local_40;
  undefined4 local_2c;
  void *local_28;
  undefined2 local_24;
  undefined2 local_22;
  undefined4 local_20;
  undefined4 local_1c;
  undefined2 local_18;
  undefined2 local_16;
  undefined2 local_14;
  undefined4 local_10;
  size_t local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  local_64 = (void *)((int)this + 0x1c);
  local_2a0 = 0xffffffff;
  local_29c = 0;
  local_54 = (undefined4 *)this;
  _memset(local_298,0,0x200);
  FUN_00428f80(local_64,&local_2a0);
  puVar1 = FUN_004259a0();
  local_54[3] = puVar1;
  local_68 = (void *)local_54[3];
  iVar2 = FUN_0045aaa0(local_68,param_1,2,2,0xac44,0x10);
  if (iVar2 < 0) {
    FUN_00454150(&DAT_005c0678,
                 L"楄敲瑣潓湵⁤䦃疃垃䚃亃枃첂規慠뮉ꪂ뢎玔떂붂\xe682਍");
    if (local_54[3] != 0) {
      FUN_00425930((void *)local_54[3]);
      local_54[3] = 0;
    }
  }
  else {
    uVar3 = FUN_0040c300((undefined4 *)local_54[3]);
    *local_54 = uVar3;
    local_54[5] = 0;
    _memset(&local_50,0,0x24);
    local_50 = 0x24;
    local_4c = 0x8008;
    local_48 = 0x8000;
    _memset(&local_24,0,0x12);
    local_14 = 0;
    local_24 = 1;
    local_22 = 2;
    local_20 = 0xac44;
    local_1c = 0x2b110;
    local_18 = 4;
    local_16 = 0x10;
    local_40 = &local_24;
    local_6c = *local_54;
    local_70 = *(code **)(*(int *)*local_54 + 0xc);
    iVar2 = (*local_70)(local_6c,&local_50,local_54 + 1,0);
    if (-1 < iVar2) {
      local_74 = local_54[1];
      local_78 = *(code **)(*(int *)local_54[1] + 0x2c);
      iVar2 = (*local_78)(local_74,0,0x8000,&local_28,&local_c,&local_2c,&local_10,0);
      if (-1 < iVar2) {
        _memset(local_28,0,local_c);
        local_7c = local_54[1];
        local_80 = *(code **)(*(int *)local_54[1] + 0x4c);
        (*local_80)(local_7c,local_28,local_c,local_2c,local_10);
        local_84 = local_54[1];
        local_88 = *(code **)(*(int *)local_54[1] + 0x30);
        (*local_88)(local_84,0,0,1);
        local_54[0x15f7] = 100;
        local_54[0x15f8] = 100;
        SetTimer(param_1,0,0xfa,(TIMERPROC)0x0);
        local_54[2] = param_1;
        for (local_58 = 0; local_58 < 0x48; local_58 = local_58 + 1) {
          iVar2 = FUN_00427050(&DAT_005ba830,local_58,
                               (undefined1 (*) [16])(&PTR_s_se_plst00_wav_005aedf0)[local_58]);
          if (iVar2 != 0) {
            FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056dcbc);
            goto LAB_0042674a;
          }
        }
        local_60 = &DAT_005bc1c4;
        local_5c = std::
                   _Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                   ::_Get_first((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                                 *)&DAT_005bc1c4);
        local_8c = (default_delete<class_std::_Facet_base> *)FUN_00428c60((int)local_60);
        for (; local_5c != local_8c; local_5c = local_5c + 0x18) {
          local_90 = local_5c;
          iVar2 = FUN_00426760((int *)local_5c);
          if (iVar2 != 0) goto LAB_0042674a;
        }
        local_98 = local_54 + 0x881;
        local_94 = 0;
        FUN_00428f60(local_98,&local_94);
        FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056dcf4);
      }
    }
  }
LAB_0042674a:
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

