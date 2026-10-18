/* Entry: 0x0055a089; symbol: _iswctype; body bytes: 105 */

/* Library Function - Single Match
    _iswctype
   
   Library: Visual Studio 2019 Release */

int __cdecl _iswctype(wint_t _C,wctype_t _Type)

{
  int iVar1;
  uint in_ECX;
  WCHAR local_c;
  undefined2 uStack_a;
  uint local_8;
  
  if (_C != 0xffff) {
    if (_C < 0x100) {
      return (uint)(*(ushort *)(PTR_DAT_005b2774 + (uint)_C * 2) & _Type);
    }
    _local_c = CONCAT22((short)(in_ECX >> 0x10),_C);
    local_8 = in_ECX & 0xffff0000;
    iVar1 = ___acrt_GetStringTypeW(1,&local_c,1,(LPWORD)&local_8);
    if (iVar1 != 0) {
      return (uint)((ushort)local_8 & _Type);
    }
  }
  return 0;
}

