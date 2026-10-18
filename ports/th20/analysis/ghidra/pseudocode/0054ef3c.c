/* Entry: 0x0054ef3c; symbol: FUN_0054ef3c; body bytes: 74 */

uint __thiscall FUN_0054ef3c(void *this,int param_1,ushort param_2)

{
  bool bVar1;
  uint uVar2;
  undefined3 extraout_var;
  undefined2 in_stack_0000000a;
  
  uVar2 = 0xffff;
  if (param_2 == 0xffff) goto LAB_0054ef80;
  uVar2 = param_1;
  if (param_1 == 0) {
LAB_0054ef7c:
    uVar2 = CONCAT31((int3)(uVar2 >> 8),1);
  }
  else {
    if (param_1 == 1) {
      uVar2 = _param_2 - 9;
      if (4 < (ushort)uVar2) {
        bVar1 = param_2 == 0x20;
        goto LAB_0054ef7a;
      }
    }
    else {
      uVar2 = param_1 - 8;
      if (uVar2 == 0) {
        bVar1 = FUN_0054f9a3((void *)((int)this + 0x4c),param_2);
        uVar2 = CONCAT31(extraout_var,bVar1);
        bVar1 = !bVar1;
LAB_0054ef7a:
        if (!bVar1) goto LAB_0054ef7c;
      }
    }
LAB_0054ef80:
    uVar2 = uVar2 & 0xffffff00;
  }
  return uVar2;
}

