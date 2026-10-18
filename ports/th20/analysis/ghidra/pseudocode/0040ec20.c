/* Entry: 0x0040ec20; symbol: FUN_0040ec20; body bytes: 101 */

void __cdecl FUN_0040ec20(int *param_1,int *param_2)

{
  uint uVar1;
  
  *param_2 = *param_2 + 0x23;
  uVar1 = *param_1 - *(int *)(*param_1 + -4);
  if ((3 < uVar1) && (uVar1 < 0x24)) {
    *param_1 = *(int *)(*param_1 + -4);
    return;
  }
                    /* WARNING: Subroutine does not return */
  __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
}

