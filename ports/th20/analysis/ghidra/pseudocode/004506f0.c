/* Entry: 0x004506f0; symbol: FUN_004506f0; body bytes: 343 */

void __thiscall FUN_004506f0(void *this,int param_1,undefined4 *param_2)

{
  int iVar1;
  undefined4 *puVar2;
  undefined4 *puVar3;
  
  puVar2 = param_2;
  puVar3 = (undefined4 *)(*(int *)((int)this + 0x50) + param_1 * 0x58);
  for (iVar1 = 0x16; iVar1 != 0; iVar1 = iVar1 + -1) {
    *puVar3 = *puVar2;
    puVar2 = puVar2 + 1;
    puVar3 = puVar3 + 1;
  }
  *(float *)(*(int *)((int)this + 0x50) + 0x38 + param_1 * 0x58) =
       *(float *)(*(int *)((int)this + 0x50) + 0xc + param_1 * 0x58) /
       *(float *)(*(int *)((int)this + 0x50) + 0x20 + param_1 * 0x58);
  *(float *)(*(int *)((int)this + 0x50) + 0x40 + param_1 * 0x58) =
       *(float *)(*(int *)((int)this + 0x50) + 0x14 + param_1 * 0x58) /
       *(float *)(*(int *)((int)this + 0x50) + 0x20 + param_1 * 0x58);
  *(float *)(*(int *)((int)this + 0x50) + 0x3c + param_1 * 0x58) =
       *(float *)(*(int *)((int)this + 0x50) + 0x10 + param_1 * 0x58) /
       *(float *)(*(int *)((int)this + 0x50) + 0x1c + param_1 * 0x58);
  *(float *)(*(int *)((int)this + 0x50) + 0x44 + param_1 * 0x58) =
       *(float *)(*(int *)((int)this + 0x50) + 0x18 + param_1 * 0x58) /
       *(float *)(*(int *)((int)this + 0x50) + 0x1c + param_1 * 0x58);
  *(float *)(*(int *)((int)this + 0x50) + 0x4c + param_1 * 0x58) =
       (*(float *)(*(int *)((int)this + 0x50) + 0x14 + param_1 * 0x58) -
       *(float *)(*(int *)((int)this + 0x50) + 0xc + param_1 * 0x58)) / (float)param_2[0x14];
  *(float *)(*(int *)((int)this + 0x50) + 0x48 + param_1 * 0x58) =
       (*(float *)(*(int *)((int)this + 0x50) + 0x18 + param_1 * 0x58) -
       *(float *)(*(int *)((int)this + 0x50) + 0x10 + param_1 * 0x58)) / (float)param_2[0x15];
  return;
}

