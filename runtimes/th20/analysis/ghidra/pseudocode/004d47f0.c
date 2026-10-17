/* Entry: 0x004d47f0; symbol: FUN_004d47f0; body bytes: 474 */

undefined4 __thiscall FUN_004d47f0(void *this,undefined4 param_1,undefined4 param_2)

{
  int *piVar1;
  int iVar2;
  
  if (0x1ff < *(int *)((int)this + 0x28)) {
    return 0;
  }
  *(int *)((int)this + 0x2c) = *(int *)((int)this + 0x2c) + 1;
  if (*(int *)((int)this + 0x2c) < 0x10000) {
    *(undefined4 *)((int)this + 0x2c) = 0x10000;
  }
  switch(param_1) {
  case 0:
    piVar1 = FUN_004c8190();
    if (piVar1 == (int *)0x0) {
      return 0;
    }
    piVar1[0x23] = *(int *)((int)this + 0x2c);
    iVar2 = (**(code **)(*piVar1 + 0x14))(param_2);
    if (iVar2 < 0) {
      return 0;
    }
    FUN_004c9300(this,(int)piVar1);
    break;
  case 1:
    piVar1 = FUN_004c8130();
    if (piVar1 == (int *)0x0) {
      return 0;
    }
    piVar1[0x23] = *(int *)((int)this + 0x2c);
    iVar2 = (**(code **)(*piVar1 + 0x14))(param_2);
    if (iVar2 < 0) {
      return 0;
    }
    FUN_004c9300(this,(int)piVar1);
    break;
  case 2:
    piVar1 = FUN_004c8080();
    if (piVar1 == (int *)0x0) {
      return 0;
    }
    piVar1[0x23] = *(int *)((int)this + 0x2c);
    iVar2 = (**(code **)(*piVar1 + 0x14))(param_2);
    if (iVar2 < 0) {
      return 0;
    }
    FUN_004c9300(this,(int)piVar1);
    break;
  case 3:
    piVar1 = FUN_004c8020();
    if (piVar1 == (int *)0x0) {
      return 0;
    }
    piVar1[0x23] = *(int *)((int)this + 0x2c);
    iVar2 = (**(code **)(*piVar1 + 0x14))(param_2);
    if (iVar2 < 0) {
      return 0;
    }
    FUN_004c9300(this,(int)piVar1);
  }
  return *(undefined4 *)((int)this + 0x2c);
}

