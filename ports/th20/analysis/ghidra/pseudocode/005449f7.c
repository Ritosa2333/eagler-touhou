/* Entry: 0x005449f7; symbol: ___std_exception_copy; body bytes: 99 */

/* Library Function - Single Match
    ___std_exception_copy
   
   Libraries: Visual Studio 2015 Release, Visual Studio 2017 Release, Visual Studio 2019 Release */

void __cdecl ___std_exception_copy(int *param_1,int *param_2)

{
  char *pcVar1;
  char cVar2;
  char *pcVar3;
  char *pcVar4;
  char *pcVar5;
  
  if (((char)param_1[1] == '\0') || (pcVar4 = (char *)*param_1, pcVar4 == (char *)0x0)) {
    *param_2 = *param_1;
    *(undefined1 *)(param_2 + 1) = 0;
  }
  else {
    pcVar1 = pcVar4 + 1;
    do {
      cVar2 = *pcVar4;
      pcVar4 = pcVar4 + 1;
    } while (cVar2 != '\0');
    pcVar3 = (char *)FUN_0054a550((SIZE_T)(pcVar4 + (1 - (int)pcVar1)));
    pcVar5 = pcVar3;
    if (pcVar3 != (char *)0x0) {
      FUN_00548610(pcVar3,(int)(pcVar4 + (1 - (int)pcVar1)),*param_1);
      pcVar5 = (char *)0x0;
      *param_2 = (int)pcVar3;
      *(undefined1 *)(param_2 + 1) = 1;
    }
    thunk_FUN_00557ba0(pcVar5);
  }
  return;
}

