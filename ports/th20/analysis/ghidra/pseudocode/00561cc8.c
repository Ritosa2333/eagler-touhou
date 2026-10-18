/* Entry: 0x00561cc8; symbol: FUN_00561cc8; body bytes: 496 */

void __cdecl FUN_00561cc8(int param_1,UINT *param_2,LPWSTR param_3)

{
  int *piVar1;
  short *psVar2;
  int iVar3;
  int *piVar4;
  int iVar5;
  uint uVar6;
  UINT _Val;
  BOOL BVar7;
  void *this;
  void *this_00;
  uint local_14;
  LCID local_10;
  LCID local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  iVar3 = FUN_005579c2();
  piVar4 = (int *)(iVar3 + 0x50);
  local_14 = 0;
  local_10 = 0;
  local_c = 0;
  iVar5 = FUN_005579c2();
  *(uint **)(iVar5 + 0x34c) = &local_14;
  psVar2 = (short *)(param_1 + 0x80);
  *piVar4 = param_1;
  piVar1 = (int *)(iVar3 + 0x54);
  *piVar1 = (int)psVar2;
  if ((psVar2 != (short *)0x0) && (*psVar2 != 0)) {
    FUN_00561c61(0x59ad40,DAT_0059ae54 + -1,piVar1);
  }
  local_14 = 0;
  if (((short *)*piVar4 == (short *)0x0) || (*(short *)*piVar4 == 0)) {
    if (((short *)*piVar1 == (short *)0x0) || (*(short *)*piVar1 == 0)) {
      local_14 = 0x104;
      local_10 = GetUserDefaultLCID();
      local_c = local_10;
    }
    else {
      _GetLcidFromCountry((byte *)&local_14);
    }
  }
  else {
    if (((short *)*piVar1 == (short *)0x0) || (*(short *)*piVar1 == 0)) {
      _GetLcidFromLanguage((byte *)&local_14);
    }
    else {
      _GetLcidFromLangCountry(&local_14);
    }
    if ((local_14 == 0) &&
       (uVar6 = FUN_00561c61(0x59aa30,DAT_0059ad3c + -1,piVar4), (char)uVar6 != '\0')) {
      if (((short *)*piVar1 == (short *)0x0) || (*(short *)*piVar1 == 0)) {
        _GetLcidFromLanguage((byte *)&local_14);
      }
      else {
        _GetLcidFromLangCountry(&local_14);
      }
    }
  }
  if ((((local_14 != 0) &&
       (_Val = _ProcessCodePage((wchar_t *)(-(uint)(param_1 != 0) & param_1 + 0x100U),(int)&local_14
                               ), _Val != 0)) &&
      (BVar7 = IsValidCodePage(_Val & 0xffff), BVar7 != 0)) &&
     (BVar7 = IsValidLocale(local_10,1), BVar7 != 0)) {
    if (param_2 != (UINT *)0x0) {
      *param_2 = _Val;
    }
    ___acrt_LCIDToLocaleName_16(this,local_10,(short *)(iVar3 + 0x2a0),0x55,0);
    if (param_3 != (LPWSTR)0x0) {
      ___acrt_LCIDToLocaleName_16(this_00,local_10,param_3 + 0x90,0x55,0);
      iVar3 = GetLocaleInfoW(local_10,0x1001,param_3,0x40);
      if ((iVar3 != 0) && (iVar3 = GetLocaleInfoW(local_c,0x1002,param_3 + 0x40,0x40), iVar3 != 0))
      {
        FID_conflict___itow_s(_Val,param_3 + 0x80,0x10,10);
      }
    }
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

