/* Entry: 0x00477520; symbol: FUN_00477520; body bytes: 285 */

void __thiscall FUN_00477520(void *this,int param_1)

{
  *(int *)((int)this + 0xb00) = (int)this + param_1 * 0x16c + 0x278;
  FUN_004da1f0(*(int *)((int)this + 0xb00),(int *)(*(int *)((int)this + 0xb00) + 0xe0));
  FUN_004d9f60(*(float **)((int)this + 0xb00));
  (**(code **)(**(int **)((int)this + 8) + 0xbc))
            (*(undefined4 *)((int)this + 8),*(int *)((int)this + 0xb00) + 0x104);
  *(float *)(DAT_005c0028 + 0xd0) = (float)*(int *)(*(int *)((int)this + 0xb00) + 0xfc);
  *(float *)(DAT_005c0028 + 0xd4) = (float)*(int *)(*(int *)((int)this + 0xb00) + 0x100);
  *(undefined4 *)((int)this + 0xb04) = 0;
  *(undefined4 *)(DAT_005c0028 + 200) = *(undefined4 *)(*(int *)((int)this + 0xb00) + 0x13c);
  *(undefined4 *)(DAT_005c0028 + 0xcc) = *(undefined4 *)(*(int *)((int)this + 0xb00) + 0x140);
  return;
}

