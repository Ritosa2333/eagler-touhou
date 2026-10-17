/* Entry: 0x00544a5a; symbol: FUN_00544a5a; body bytes: 34 */

void __cdecl FUN_00544a5a(undefined4 *param_1)

{
  if (*(char *)(param_1 + 1) != '\0') {
    thunk_FUN_00557ba0((LPVOID)*param_1);
  }
  *(undefined1 *)(param_1 + 1) = 0;
  *param_1 = 0;
  return;
}

