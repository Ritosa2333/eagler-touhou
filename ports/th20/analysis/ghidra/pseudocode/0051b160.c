/* Entry: 0x0051b160; symbol: FUN_0051b160; body bytes: 112 */

void __thiscall FUN_0051b160(void *this,uint param_1)

{
  uint *puVar1;
  uint *puVar2;
  int *piVar3;
  
  puVar1 = (uint *)FUN_004bd080((void *)((int)this + 0x168),param_1);
  puVar2 = (uint *)FUN_004bd080((void *)((int)this + 0x18c),param_1);
  if (*puVar2 < *puVar1) {
    piVar3 = (int *)FUN_004bd080((void *)((int)this + 0x18c),param_1);
    *piVar3 = *piVar3 + 1;
  }
  return;
}

