/* Entry: 0x0054fba4; symbol: FUN_0054fba4; body bytes: 111 */

uint __thiscall FUN_0054fba4(void *this,uint param_1,uint param_2)

{
  int *piVar1;
  uint *puVar2;
  __acrt_ptd *p_Var3;
  uint uVar4;
  uint uVar5;
  
  piVar1 = *(int **)((int)this + 0x54);
  *(int **)((int)this + 0x54) = piVar1 + 1;
  puVar2 = (uint *)*piVar1;
  if (puVar2 == (uint *)0x0) {
    p_Var3 = FUN_005516c1();
    *(undefined4 *)p_Var3 = 0x16;
    uVar4 = FUN_005480bc();
LAB_0054fbc9:
    uVar4 = uVar4 & 0xffffff00;
  }
  else {
    uVar4 = FUN_0054ef86((int)this + 0x18);
    if (uVar4 == 1) {
      uVar5 = 0;
      *(undefined1 *)puVar2 = (undefined1)param_1;
    }
    else if (uVar4 == 2) {
      uVar5 = param_1 & 0xffff;
      *(undefined2 *)puVar2 = (undefined2)param_1;
    }
    else {
      uVar5 = param_1;
      if (uVar4 == 4) {
        *puVar2 = param_1;
      }
      else {
        uVar4 = uVar4 - 8;
        if (uVar4 != 0) goto LAB_0054fbc9;
        *puVar2 = param_1;
        puVar2[1] = param_2;
      }
    }
    uVar4 = CONCAT31((int3)(uVar5 >> 8),1);
  }
  return uVar4;
}

