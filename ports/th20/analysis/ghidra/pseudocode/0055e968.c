/* Entry: 0x0055e968; symbol: getSystemCP; body bytes: 113 */

/* Library Function - Single Match
    int __cdecl getSystemCP(int)
   
   Library: Visual Studio 2019 Release */

int __cdecl getSystemCP(int param_1)

{
  int local_14;
  int local_10;
  char local_8;
  
  FUN_0054c44c(&local_14,(int *)0x0);
  DAT_005e5970 = 0;
  if (param_1 == -2) {
    DAT_005e5970 = 1;
    param_1 = GetOEMCP();
  }
  else if (param_1 == -3) {
    DAT_005e5970 = 1;
    param_1 = GetACP();
  }
  else if (param_1 == -4) {
    DAT_005e5970 = 1;
    param_1 = *(UINT *)(local_10 + 8);
  }
  if (local_8 != '\0') {
    *(uint *)(local_14 + 0x350) = *(uint *)(local_14 + 0x350) & 0xfffffffd;
  }
  return param_1;
}

