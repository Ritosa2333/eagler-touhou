/* Entry: 0x0041bc60; symbol: FUN_0041bc60; body bytes: 321 */

void __thiscall FUN_0041bc60(void *this,char **param_1)

{
  char cVar1;
  bool bVar2;
  uint *puVar3;
  uint *puVar4;
  undefined2 local_8;
  undefined2 local_6;
  
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    puVar3 = (uint *)move<>(this);
    puVar4 = (uint *)move<>(param_1);
    FUN_00543e20(puVar3,puVar4,0x18);
    param_1[4] = (char *)0x0;
    param_1[5] = (char *)0x7;
    FUN_0040ebe0((int)param_1);
    local_6 = 0;
    FUN_0040ff00((undefined2 *)param_1,&local_6);
  }
  else {
    bVar2 = FUN_0040f1b0((int)param_1);
    if (bVar2) {
      FUN_0040f670();
      FUN_0040cd70(this,param_1);
      FUN_0040f6f0(param_1);
    }
    else {
      FUN_0040e5e0();
      FUN_0040ebe0((int)this);
      FUN_00410030((uint *)this,(uint *)param_1,(int)(param_1[4] + 1));
    }
    *(char **)((int)this + 0x14) = param_1[5];
    *(char **)((int)this + 0x10) = param_1[4];
    param_1[4] = (char *)0x0;
    param_1[5] = (char *)0x7;
    local_8 = 0;
    FUN_0040ff00((undefined2 *)param_1,&local_8);
  }
  return;
}

