/* Entry: 0x005465e0; symbol: _ValidateLocalCookies; body bytes: 50 */

/* Library Function - Single Match
    _ValidateLocalCookies
   
   Library: Visual Studio 2019 Release */

void __cdecl _ValidateLocalCookies(int *param_1,int param_2)

{
  if (*param_1 != -2) {
    FUN_005429ee(param_1[1] + param_2 ^ *(uint *)(*param_1 + param_2));
  }
  FUN_005429ee(param_1[3] + param_2 ^ *(uint *)(param_1[2] + param_2));
  return;
}

