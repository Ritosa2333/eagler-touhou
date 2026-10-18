/* Entry: 0x0054a407; symbol: FUN_0054a407; body bytes: 111 */

void __thiscall FUN_0054a407(void *this,uint *param_1,uint param_2,int *param_3)

{
  undefined4 *puVar1;
  uint uVar2;
  uint uVar3;
  
  if (param_2 != 0) {
                    /* WARNING: Load size is inaccurate */
    puVar1 = *this;
    if (puVar1[2] == puVar1[1]) {
      if (*(char *)(puVar1 + 3) == '\0') {
        *param_3 = -1;
      }
      else {
        *param_3 = *param_3 + param_2;
      }
    }
    else {
      uVar2 = puVar1[1] - puVar1[2];
      uVar3 = param_2;
      if (uVar2 < param_2) {
        uVar3 = uVar2;
      }
      FUN_00543e20((uint *)*puVar1,param_1,uVar3);
                    /* WARNING: Load size is inaccurate */
      **this = **this + uVar3;
                    /* WARNING: Load size is inaccurate */
      *(int *)(*this + 8) = *(int *)(*this + 8) + uVar3;
                    /* WARNING: Load size is inaccurate */
      if (*(char *)(*this + 0xc) == '\0') {
        if (uVar3 == param_2) {
          *param_3 = *param_3 + uVar3;
        }
        else {
          *param_3 = -1;
        }
      }
      else {
        *param_3 = *param_3 + param_2;
      }
    }
  }
  return;
}

