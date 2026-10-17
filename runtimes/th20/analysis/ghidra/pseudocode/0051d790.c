/* Entry: 0x0051d790; symbol: FUN_0051d790; body bytes: 116 */

void __thiscall FUN_0051d790(void *this,undefined4 param_1)

{
  char cVar1;
  
  cVar1 = FUN_0046a4d0();
  if (cVar1 == '\0') {
    FUN_0052cbd0((void *)((int)this + 0x10),&param_1);
    while (cVar1 = FUN_0046a4d0(), cVar1 != '\0') {
                    /* WARNING: Load size is inaccurate */
      *(int *)this = *this + 1;
                    /* WARNING: Load size is inaccurate */
      if (*(int *)((int)this + 8) <= *this) {
        *(undefined4 *)this = 0;
      }
    }
  }
  return;
}

