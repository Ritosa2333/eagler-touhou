/* Entry: 0x0054fda0; symbol: _strncmp; body bytes: 115 */

/* Library Function - Single Match
    _strncmp
   
   Libraries: Visual Studio 2015, Visual Studio 2017, Visual Studio 2019 */

int __cdecl _strncmp(char *_Str1,char *_Str2,size_t _MaxCount)

{
  byte bVar1;
  uint uVar2;
  int iVar3;
  bool bVar4;
  
  if (_MaxCount != 0) {
    iVar3 = (int)_Str1 - (int)_Str2;
    uVar2 = (uint)_Str2 & 3;
    while( true ) {
      if (uVar2 == 0) {
        while ((((uint)(iVar3 + (int)_Str2) & 0xfff) < 0xffd &&
               (uVar2 = *(uint *)(iVar3 + (int)_Str2), uVar2 == *(uint *)_Str2))) {
          bVar4 = _MaxCount < 4;
          _MaxCount = _MaxCount - 4;
          if (bVar4 || _MaxCount == 0) {
            return 0;
          }
          _Str2 = (char *)((int)_Str2 + 4);
          if ((~uVar2 & uVar2 + 0xfefefeff & 0x80808080) != 0) {
            return 0;
          }
        }
      }
      bVar1 = *(byte *)(iVar3 + (int)_Str2);
      if (bVar1 != (byte)*(uint *)_Str2) {
        return -(uint)(bVar1 < (byte)*(uint *)_Str2) | 1;
      }
      if (bVar1 == 0) break;
      _Str2 = (char *)((int)_Str2 + 1);
      bVar4 = _MaxCount == 0;
      _MaxCount = _MaxCount - 1;
      if (bVar4 || _MaxCount == 0) {
        return 0;
      }
      uVar2 = (uint)_Str2 & 3;
    }
  }
  return 0;
}

