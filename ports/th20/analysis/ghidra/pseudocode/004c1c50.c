/* Entry: 0x004c1c50; symbol: FUN_004c1c50; body bytes: 290 */

void __thiscall
FUN_004c1c50(void *this,undefined4 *param_1,undefined4 param_2,undefined4 param_3,float param_4,
            int param_5,undefined4 param_6)

{
  *(uint *)((int)this + 0x14) = *(uint *)((int)this + 0x14) | 1;
  *(uint *)((int)this + 0x14) = *(uint *)((int)this + 0x14) & 0xfffffff1;
  *(uint *)((int)this + 0x14) = *(uint *)((int)this + 0x14) & 0xffffffef;
  *(uint *)((int)this + 0x14) = *(uint *)((int)this + 0x14) & 0xffffffbf;
  FUN_004a73d0((void *)((int)this + 0x34));
  FUN_004c2030(this,param_1);
  *(undefined4 *)((int)this + 0x2c) = param_2;
  *(undefined4 *)((int)this + 0x30) = param_3;
  FUN_00452f20((void *)((int)this + 0x24),param_4);
  *(undefined4 *)((int)this + 0x28) = 0;
  FUN_00423520((void *)((int)this + 0x7c),param_5);
  *(undefined4 *)((int)this + 0x94) = param_6;
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

