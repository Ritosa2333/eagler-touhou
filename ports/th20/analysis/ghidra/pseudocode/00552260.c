/* Entry: 0x00552260; symbol: _isupper; body bytes: 132 */

/* Library Function - Single Match
    _isupper
   
   Library: Visual Studio 2019 Release */

int __cdecl _isupper(int _C)

{
  int iVar1;
  int *local_8;
  
  if (DAT_005e55dc == 0) {
    if (_C + 1U < 0x101) {
      return *(ushort *)(PTR_DAT_005b2788 + _C * 2) & 1;
    }
  }
  else {
    iVar1 = FUN_005579c2();
    local_8 = *(int **)(iVar1 + 0x4c);
    ___acrt_update_locale_info(iVar1,(int *)&local_8);
    if (_C + 1U < 0x101) {
      return *(ushort *)(*local_8 + _C * 2) & 1;
    }
    if (1 < local_8[1]) {
      iVar1 = __isctype_l(_C,1,(_locale_t)0x0);
      return iVar1;
    }
  }
  return 0;
}

