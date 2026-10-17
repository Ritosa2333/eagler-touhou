/* Entry: 0x005612dd; symbol: _TranslateName; body bytes: 109 */

/* Library Function - Single Match
    _TranslateName
   
   Library: Visual Studio 2019 Release */

bool __cdecl _TranslateName(int param_1,int param_2,int *param_3)

{
  int iVar1;
  int iVar2;
  int iVar3;
  
  iVar1 = 1;
  iVar3 = 0;
  if (-1 < param_2) {
    do {
      if (iVar1 == 0) break;
      iVar2 = (iVar3 + param_2) / 2;
      iVar1 = FUN_0055df10((ushort *)*param_3,*(ushort **)(iVar2 * 0xc + param_1));
      if (iVar1 == 0) {
        *param_3 = iVar2 * 0xc + param_1 + 4;
      }
      else if (iVar1 < 0) {
        param_2 = iVar2 + -1;
      }
      else {
        iVar3 = iVar2 + 1;
      }
    } while (iVar3 <= param_2);
  }
  return iVar1 == 0;
}

