/* Entry: 0x0055710f; symbol: parse_bcp47_script; body bytes: 111 */

/* Library Function - Single Match
    _parse_bcp47_script
   
   Library: Visual Studio 2019 Release */

uint __cdecl parse_bcp47_script(int param_1,int *param_2)

{
  uint in_EAX;
  int iVar1;
  
  if (((param_2[2] == 0) && (param_2[1] == 4)) &&
     (in_EAX = FUN_0055717f(*param_2,4), (char)in_EAX != '\0')) {
    iVar1 = FUN_00560700((short *)(param_1 + 0x120),0x55,(short *)&DAT_00597064,1);
    if ((iVar1 == 0) &&
       (iVar1 = FUN_00560700((short *)(param_1 + 0x120),0x55,(short *)*param_2,param_2[1]),
       iVar1 == 0)) {
      return 1;
    }
                    /* WARNING: Subroutine does not return */
    __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
  }
  return in_EAX & 0xffffff00;
}

