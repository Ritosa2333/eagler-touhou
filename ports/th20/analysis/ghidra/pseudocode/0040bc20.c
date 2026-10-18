/* Entry: 0x0040bc20; symbol: FUN_0040bc20; body bytes: 56 */

void FUN_0040bc20(undefined4 param_1,undefined4 param_2,int param_3,undefined *param_4)

{
  while( true ) {
    if (param_3 == 0) break;
    (*(code *)param_4)();
    param_3 = param_3 + -1;
  }
  return;
}

