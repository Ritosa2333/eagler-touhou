/* Entry: 0x004f5510; symbol: FUN_004f5510; body bytes: 41 */

void __fastcall FUN_004f5510(undefined4 *param_1)

{
  BOOL BVar1;
  
  BVar1 = InitOnceComplete((LPINIT_ONCE)*param_1,param_1[1],(LPVOID)0x0);
  if (BVar1 == 0) {
                    /* WARNING: Subroutine does not return */
    _abort();
  }
  return;
}

