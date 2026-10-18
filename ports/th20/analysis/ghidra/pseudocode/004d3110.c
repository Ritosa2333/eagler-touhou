/* Entry: 0x004d3110; symbol: FUN_004d3110; body bytes: 275 */

undefined4 __thiscall FUN_004d3110(void *this,undefined4 *param_1)

{
  int local_c;
  
  FUN_004c8c90((void *)((int)this + 0x6f8),param_1);
  *(undefined4 *)((int)this + 0x20) = 3;
  *(undefined4 *)((int)this + 0x24) = 3;
  *(undefined4 *)((int)this + 0x58) = *(undefined4 *)((int)this + 0x6f8);
  *(undefined4 *)((int)this + 0x5c) = *(undefined4 *)((int)this + 0x6fc);
  *(undefined4 *)((int)this + 0x60) = *(undefined4 *)((int)this + 0x700);
  FUN_004d7dd0(this,*(undefined4 *)((int)this + 0x744));
  *(undefined4 *)((int)this + 0x74) = *(undefined4 *)((int)this + 0x718);
  *(undefined4 *)((int)this + 0x70) = *(undefined4 *)((int)this + 0x710);
  *(undefined4 *)((int)this + 0x6e8) = *(undefined4 *)((int)this + 0x724);
  *(undefined4 *)((int)this + 0x8c) = *(undefined4 *)((int)this + 0x720);
  for (local_c = 0; local_c < 0x200; local_c = local_c + 1) {
    *(undefined4 *)((int)this + local_c * 4 + 0xd30) = *(undefined4 *)((int)this + 0x74);
  }
  *(undefined4 *)((int)this + 0x78) = DAT_0056c8cc;
  *(undefined4 *)((int)this + 0xd2c) = 0;
  FUN_004299d0((int)this + 0x748);
  FUN_0045e5b0((int)this + 0x748);
  return 0;
}

