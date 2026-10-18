/* Entry: 0x00553863; symbol: __initialize_onexit_table; body bytes: 41 */

/* Library Function - Single Match
    __initialize_onexit_table
   
   Library: Visual Studio 2019 Release */

undefined4 __cdecl __initialize_onexit_table(int *param_1)

{
  int iVar1;
  
  iVar1 = DAT_005b25c0;
  if (param_1 == (int *)0x0) {
    return 0xffffffff;
  }
  if (*param_1 == param_1[2]) {
    *param_1 = DAT_005b25c0;
    param_1[1] = iVar1;
    param_1[2] = iVar1;
  }
  return 0;
}

