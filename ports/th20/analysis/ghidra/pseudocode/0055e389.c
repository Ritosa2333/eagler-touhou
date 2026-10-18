/* Entry: 0x0055e389; symbol: copy_and_add_argument_to_buffer<char>; body bytes: 176 */

/* Library Function - Single Match
    int __cdecl copy_and_add_argument_to_buffer<char>(char const * const,char const * const,unsigned
   int,class `anonymous namespace'::argument_list<char> &)
   
   Library: Visual Studio 2019 Release */

int __cdecl
copy_and_add_argument_to_buffer<char>
          (char *param_1,char *param_2,uint param_3,argument_list<char> *param_4)

{
  char cVar1;
  char *pcVar2;
  int iVar3;
  char *pcVar4;
  char *pcVar5;
  
  pcVar4 = param_1;
  do {
    cVar1 = *pcVar4;
    pcVar4 = pcVar4 + 1;
  } while (cVar1 != '\0');
  pcVar4 = pcVar4 + (1 - (int)(param_1 + 1));
  if ((char *)~param_3 < pcVar4) {
    iVar3 = 0xc;
  }
  else {
    pcVar5 = pcVar4 + param_3 + 1;
    pcVar2 = (char *)FUN_00557b40((uint)pcVar5,1);
    if (param_3 != 0) {
      iVar3 = FUN_005651e0(pcVar2,(int)pcVar5,(int)param_2,param_3);
      if (iVar3 != 0) goto LAB_0055e42d;
    }
    iVar3 = FUN_005651e0(pcVar2 + param_3,(int)pcVar5 - param_3,(int)param_1,(int)pcVar4);
    if (iVar3 != 0) {
LAB_0055e42d:
                    /* WARNING: Subroutine does not return */
      __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
    }
    iVar3 = _anon_5F5C8891::argument_list<char>::expand_if_necessary(param_4);
    if (iVar3 == 0) {
      **(undefined4 **)(param_4 + 4) = pcVar2;
      iVar3 = 0;
      *(int *)(param_4 + 4) = *(int *)(param_4 + 4) + 4;
    }
    else {
      FUN_00557ba0(pcVar2);
    }
    FUN_00557ba0((LPVOID)0x0);
  }
  return iVar3;
}

