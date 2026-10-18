/* Entry: 0x0040f740; symbol: FUN_0040f740; body bytes: 317 */

void __thiscall FUN_0040f740(void *this,char **param_1)

{
  char cVar1;
  bool bVar2;
  uint *puVar3;
  uint *puVar4;
  undefined1 local_6;
  undefined1 local_5;
  
  cVar1 = FUN_00411170();
  if (cVar1 == '\0') {
    puVar3 = (uint *)move<>(this);
    puVar4 = (uint *)move<>(param_1);
    FUN_00543e20(puVar3,puVar4,0x18);
    param_1[4] = (char *)0x0;
    param_1[5] = (char *)0xf;
    FUN_0040eba0((int)param_1);
    local_5 = 0;
    FUN_0040fe90((undefined1 *)param_1,&local_5);
  }
  else {
    bVar2 = FUN_0040f180((int)param_1);
    if (bVar2) {
      FUN_0040f670();
      FUN_0040cd70(this,param_1);
      FUN_0040f6a0(param_1);
    }
    else {
      FUN_0040e5e0();
      FUN_0040eba0((int)this);
      FUN_0040ffd0((uint *)this,(uint *)param_1,(uint)(param_1[4] + 1));
    }
    *(char **)((int)this + 0x14) = param_1[5];
    *(char **)((int)this + 0x10) = param_1[4];
    param_1[4] = (char *)0x0;
    param_1[5] = (char *)0xf;
    local_6 = 0;
    FUN_0040fe90((undefined1 *)param_1,&local_6);
  }
  return;
}

