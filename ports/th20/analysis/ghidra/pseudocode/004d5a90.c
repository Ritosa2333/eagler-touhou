/* Entry: 0x004d5a90; symbol: FUN_004d5a90; body bytes: 126 */

int __thiscall FUN_004d5a90(void *this,int param_1)

{
  undefined4 *puVar1;
  int *local_8;
  
  for (local_8 = (int *)((int)this + 0x1330); *local_8 != 0; local_8 = (int *)*local_8) {
  }
  puVar1 = FUN_004c7f90();
  if (puVar1 != (undefined4 *)0x0) {
    local_8[3] = param_1;
    puVar1[2] = param_1;
    *puVar1 = 0;
    puVar1[1] = local_8;
  }
  *local_8 = (int)puVar1;
  return *local_8;
}

