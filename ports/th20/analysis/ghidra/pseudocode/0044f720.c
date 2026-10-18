/* Entry: 0x0044f720; symbol: FUN_0044f720; body bytes: 280 */

void __thiscall FUN_0044f720(void *this,int param_1)

{
  undefined4 uStack00000008;
  
  *(int *)((int)this + 0xb00) = (int)this + param_1 * 0x16c + 0x278;
  uStack00000008 = 0;
  FUN_004da1f0(*(int *)((int)this + 0xb00),(int *)(*(int *)((int)this + 0xb00) + 0x104));
  FUN_004da120(*(int *)((int)this + 0xb00));
  (**(code **)(**(int **)((int)this + 8) + 0xbc))
            (*(undefined4 *)((int)this + 8),*(int *)((int)this + 0xb00) + 0x104);
  *(float *)(DAT_005c0028 + 0xd0) = (float)*(int *)(*(int *)((int)this + 0xb00) + 0xfc);
  *(float *)(DAT_005c0028 + 0xd4) = (float)*(int *)(*(int *)((int)this + 0xb00) + 0x100);
  *(undefined4 *)(DAT_005c0028 + 200) = *(undefined4 *)(*(int *)((int)this + 0xb00) + 300);
  *(undefined4 *)(DAT_005c0028 + 0xcc) = *(undefined4 *)(*(int *)((int)this + 0xb00) + 0x130);
  return;
}

