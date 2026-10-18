/* Entry: 0x0055a0f2; symbol: __isctype_l; body bytes: 203 */

/* Library Function - Single Match
    __isctype_l
   
   Library: Visual Studio 2019 Release */

int __cdecl __isctype_l(int _C,int _Type,_locale_t _Locale)

{
  int iVar1;
  int local_24;
  int *local_20 [2];
  char local_18;
  CHAR local_14;
  CHAR local_13;
  undefined1 local_12;
  WORD local_10 [4];
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_0054c44c(&local_24,(int *)_Locale);
  if ((_C < -1) || (0xff < _C)) {
    if (*(short *)(*local_20[0] + (_C >> 8 & 0xffU) * 2) < 0) {
      local_12 = 0;
      iVar1 = 2;
      local_14 = (CHAR)((uint)_C >> 8);
      local_13 = (CHAR)_C;
    }
    else {
      local_13 = '\0';
      iVar1 = 1;
      local_14 = (CHAR)_C;
    }
    local_10[0] = 0;
    local_10[1] = 0;
    local_10[2] = 0;
    iVar1 = FUN_0055d108((int *)local_20,1,&local_14,iVar1,local_10,local_20[0][2],1);
    if (iVar1 == 0) {
      if (local_18 != '\0') {
        *(uint *)(local_24 + 0x350) = *(uint *)(local_24 + 0x350) & 0xfffffffd;
      }
      goto LAB_0055a1b1;
    }
  }
  if (local_18 != '\0') {
    *(uint *)(local_24 + 0x350) = *(uint *)(local_24 + 0x350) & 0xfffffffd;
  }
LAB_0055a1b1:
  iVar1 = FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return iVar1;
}

