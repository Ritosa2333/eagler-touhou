/* Entry: 0x00562647; symbol: ___acrt_DownlevelLocaleNameToLCID; body bytes: 44 */

/* Library Function - Single Match
    ___acrt_DownlevelLocaleNameToLCID
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

undefined4 __cdecl ___acrt_DownlevelLocaleNameToLCID(ushort *param_1)

{
  uint uVar1;
  
  if (param_1 != (ushort *)0x0) {
    uVar1 = FUN_00562575(param_1);
    if ((-1 < (int)uVar1) && (uVar1 < 0xe4)) {
      return *(undefined4 *)(&DAT_0059b800 + uVar1 * 8);
    }
  }
  return 0;
}

