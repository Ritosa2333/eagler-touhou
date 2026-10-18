/* Entry: 0x0044cfb0; symbol: FUN_0044cfb0; body bytes: 77 */

int __cdecl FUN_0044cfb0(int param_1)

{
  int iVar1;
  
  iVar1 = FUN_00412540();
  if (iVar1 != 0) {
    if ((*(int *)(&DAT_0056e208 + param_1 * 4) == 0x15) ||
       (*(int *)(&DAT_0056e208 + param_1 * 4) == 0)) {
      param_1 = 5;
    }
    else if (*(int *)(&DAT_0056e208 + param_1 * 4) == 0x14) {
      param_1 = 3;
    }
  }
  return param_1;
}

