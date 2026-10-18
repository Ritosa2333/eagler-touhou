/* Entry: 0x004c1d80; symbol: FUN_004c1d80; body bytes: 260 */

void __thiscall
FUN_004c1d80(void *this,undefined4 *param_1,undefined4 param_2,undefined4 param_3,int param_4,
            undefined4 param_5)

{
  *(uint *)((int)this + 0x14) = *(uint *)((int)this + 0x14) | 1;
  *(uint *)((int)this + 0x14) = *(uint *)((int)this + 0x14) & 0xfffffff1 | 2;
  *(uint *)((int)this + 0x14) = *(uint *)((int)this + 0x14) & 0xffffffef;
  *(uint *)((int)this + 0x14) = *(uint *)((int)this + 0x14) & 0xffffffbf;
  FUN_004a73d0((void *)((int)this + 0x34));
  FUN_004c2030(this,param_1);
  *(undefined4 *)((int)this + 0x18) = param_2;
  *(undefined4 *)((int)this + 0x20) = param_3;
  FUN_00423520((void *)((int)this + 0x7c),param_4);
  *(undefined4 *)((int)this + 0x94) = param_5;
  *(undefined4 *)((int)this + 0x98) = 0;
  *(undefined4 *)((int)this + 0x9c) = 9999999;
  *(undefined4 *)((int)this + 0xa0) = 1;
  *(undefined4 *)((int)this + 0xb4) = 0;
  FUN_004117a0((void *)((int)this + 0xa4),0);
  *(undefined4 *)((int)this + 0xa8) = 0;
  *(undefined4 *)((int)this + 0xac) = 0;
  FUN_0040c300((undefined4 *)((int)this + 0x8c));
  return;
}

