/* Entry: 0x0054c321; symbol: FUN_0054c321; body bytes: 59 */

uint __thiscall FUN_0054c321(void *this,undefined4 *param_1)

{
  int *piVar1;
  undefined4 *puVar2;
  undefined4 uVar3;
  undefined4 uVar4;
  __acrt_ptd *p_Var5;
  uint uVar6;
  
  piVar1 = *(int **)((int)this + 0x54);
  *(int **)((int)this + 0x54) = piVar1 + 1;
  puVar2 = (undefined4 *)*piVar1;
  if (puVar2 == (undefined4 *)0x0) {
    p_Var5 = FUN_005516c1();
    *(undefined4 *)p_Var5 = 0x16;
    uVar6 = FUN_005480bc();
    uVar6 = uVar6 & 0xffffff00;
  }
  else {
    uVar3 = *param_1;
    uVar4 = param_1[1];
    puVar2[1] = uVar4;
    uVar6 = CONCAT31((int3)((uint)uVar4 >> 8),1);
    *puVar2 = uVar3;
  }
  return uVar6;
}

