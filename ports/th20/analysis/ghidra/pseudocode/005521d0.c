/* Entry: 0x005521d0; symbol: _islower; body bytes: 132 */

/* Library Function - Single Match
    _islower
   
   Library: Visual Studio 2019 Release */

int __cdecl _islower(int _C)

{
  int iVar1;
  int *local_8;
  
  if (DAT_005e55dc == 0) {
    if (_C + 1U < 0x101) {
      return *(ushort *)(PTR_DAT_005b2788 + _C * 2) & 2;
    }
  }
  else {
    iVar1 = FUN_005579c2();
    local_8 = *(int **)(iVar1 + 0x4c);
    ___acrt_update_locale_info(iVar1,(int *)&local_8);
    if (_C + 1U < 0x101) {
      return *(ushort *)(*local_8 + _C * 2) & 2;
    }
    if (1 < local_8[1]) {
      iVar1 = __isctype_l(_C,2,(_locale_t)0x0);
      return iVar1;
    }
  }
  return 0;
}

