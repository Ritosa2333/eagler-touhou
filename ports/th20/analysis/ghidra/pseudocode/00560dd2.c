/* Entry: 0x00560dd2; symbol: _GetPrimaryLen; body bytes: 52 */

/* Library Function - Single Match
    _GetPrimaryLen
   
   Library: Visual Studio 2019 Release */

int __cdecl _GetPrimaryLen(ushort *param_1)

{
  ushort uVar1;
  int iVar2;
  
  iVar2 = 0;
  if (param_1 == (ushort *)0x0) {
    return 0;
  }
  while( true ) {
    uVar1 = *param_1;
    param_1 = param_1 + 1;
    if (((uVar1 < 0x41) || (0x5a < uVar1)) && (0x19 < (ushort)(uVar1 - 0x61))) break;
    iVar2 = iVar2 + 1;
  }
  return iVar2;
}

