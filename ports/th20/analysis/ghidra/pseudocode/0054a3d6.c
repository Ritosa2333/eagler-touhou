/* Entry: 0x0054a3d6; symbol: FUN_0054a3d6; body bytes: 49 */

bool __thiscall FUN_0054a3d6(void *this,int *param_1)

{
  int iVar1;
  
                    /* WARNING: Load size is inaccurate */
  iVar1 = *this;
  if (iVar1 == 0) {
    *(undefined1 *)(param_1 + 7) = 1;
    param_1[6] = 0x16;
    FUN_0054803f((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0,param_1);
  }
  return iVar1 != 0;
}

