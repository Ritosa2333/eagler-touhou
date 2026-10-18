/* Entry: 0x004e1ab0; symbol: FUN_004e1ab0; body bytes: 122 */

void FUN_004e1ab0(undefined4 param_1)

{
  char cVar1;
  undefined4 local_c;
  int *local_8;
  
  cVar1 = FUN_004e1c30();
  if (cVar1 == '\0') {
    local_c = param_1;
    FUN_004e67b0(local_8 + 4,&local_c);
    while (cVar1 = FUN_0046a4d0(), cVar1 != '\0') {
      *local_8 = *local_8 + 1;
      if (local_8[2] <= *local_8) {
        *local_8 = 0;
      }
    }
  }
  return;
}

