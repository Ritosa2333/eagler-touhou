/* Entry: 0x00561741; symbol: _GetPrimaryLen; body bytes: 44 */

/* Library Function - Single Match
    _GetPrimaryLen
   
   Libraries: Visual Studio 2017 Release, Visual Studio 2019 Release */

int __cdecl _GetPrimaryLen(ushort *param_1)

{
  ushort uVar1;
  int iVar2;
  
  iVar2 = 0;
  while( true ) {
    uVar1 = *param_1;
    param_1 = param_1 + 1;
    if (((uVar1 < 0x41) || (0x5a < uVar1)) && (0x19 < (ushort)(uVar1 - 0x61))) break;
    iVar2 = iVar2 + 1;
  }
  return iVar2;
}

