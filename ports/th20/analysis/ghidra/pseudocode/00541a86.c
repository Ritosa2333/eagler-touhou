/* Entry: 0x00541a86; symbol: FUN_00541a86; body bytes: 73 */

int * __thiscall FUN_00541a86(void *this,uint *param_1)

{
  char cVar1;
  uint *puVar2;
  char *pcVar3;
  
                    /* WARNING: Load size is inaccurate */
  if ((*this != param_1) && (FUN_004fcd50((int *)this), param_1 != (uint *)0x0)) {
    cVar1 = (char)*param_1;
    puVar2 = param_1;
    while (cVar1 != '\0') {
      puVar2 = (uint *)((int)puVar2 + 1);
      cVar1 = *(char *)puVar2;
    }
    pcVar3 = (char *)((int)puVar2 + (1 - (int)param_1));
    puVar2 = (uint *)FUN_0054a550((SIZE_T)pcVar3);
    *(uint **)this = puVar2;
    if (puVar2 != (uint *)0x0) {
      FUN_00543e20(puVar2,param_1,(uint)pcVar3);
    }
  }
  return (int *)this;
}

