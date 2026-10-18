/* Entry: 0x004bd000; symbol: FUN_004bd000; body bytes: 83 */

void __thiscall FUN_004bd000(void *this,uint param_1)

{
  uint *puVar1;
  int *piVar2;
  
  puVar1 = (uint *)FUN_004bd080((void *)((int)this + 0x168),param_1);
  if (*puVar1 < 9) {
    piVar2 = (int *)FUN_004bd080((void *)((int)this + 0x168),param_1);
    *piVar2 = *piVar2 + 1;
  }
  return;
}

