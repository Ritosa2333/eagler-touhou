/* Entry: 0x00558eb5; symbol: shift_bytes; body bytes: 52 */

/* Library Function - Single Match
    void __cdecl shift_bytes(char * const,unsigned int,char * const,int)
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl shift_bytes(char *param_1,uint param_2,char *param_3,int param_4)

{
  char cVar1;
  char *pcVar2;
  
  if (param_4 != 0) {
    pcVar2 = param_3;
    do {
      cVar1 = *pcVar2;
      pcVar2 = pcVar2 + 1;
    } while (cVar1 != '\0');
    FUN_00543e20((uint *)(param_3 + param_4),(uint *)param_3,
                 (uint)(pcVar2 + (1 - (int)(param_3 + 1))));
  }
  return;
}

