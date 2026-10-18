/* Entry: 0x0055d3f3; symbol: ___acrt_LCMapStringA; body bytes: 73 */

/* Library Function - Single Match
    ___acrt_LCMapStringA
   
   Library: Visual Studio 2019 Release */

void __cdecl
___acrt_LCMapStringA
          (int *param_1,ushort *param_2,uint param_3,char *param_4,int param_5,LPWSTR param_6,
          int param_7,uint param_8,int param_9)

{
  int local_14;
  int local_10 [2];
  char local_8;
  
  FUN_0054c44c(&local_14,param_1);
  FUN_0055d206(local_10,param_2,param_3,param_4,param_5,param_6,param_7,param_8,param_9);
  if (local_8 != '\0') {
    *(uint *)(local_14 + 0x350) = *(uint *)(local_14 + 0x350) & 0xfffffffd;
  }
  return;
}

