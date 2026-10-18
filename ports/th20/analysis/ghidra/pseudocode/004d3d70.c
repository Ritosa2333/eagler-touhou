/* Entry: 0x004d3d70; symbol: FUN_004d3d70; body bytes: 986 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_004d3d70(void *this,undefined4 *param_1)

{
  void *this_00;
  undefined *puVar1;
  int iVar2;
  undefined4 *puVar3;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_004c8e90((void *)((int)this + 0x6f8),param_1);
  *(undefined4 *)((int)this + 0x20) = 3;
  *(undefined4 *)((int)this + 0x24) = 1;
  *(undefined4 *)((int)this + 0x6e4) = *(undefined4 *)((int)this + 0x74c);
  *(undefined4 *)((int)this + 0x6e8) = *(undefined4 *)((int)this + 0x750);
  FUN_004d7dd0(this,*(undefined4 *)((int)this + 0x768));
  *(undefined4 *)((int)this + 0x6d0) = 0xffd08080;
  puVar1 = FUN_004856e0(*(int *)((int)this + 0x6e4));
  if (*(int *)(puVar1 + 0x14c) == 6) {
    puVar1 = FUN_004856e0(*(int *)((int)this + 0x6e4));
    *(undefined4 *)((int)this + 0x6d0) =
         *(undefined4 *)(puVar1 + *(int *)((int)this + 0x6e8) * 0x14 + 0x14);
  }
  FUN_004299d0((int)this + 0x770);
  FUN_00438a30((void *)((int)this + 0x770),FUN_004cf550);
  *(void **)((int)this + 0xd38) = this;
  iVar2 = FUN_0041ca90(*(int *)((int)this + 0x6f0));
  this_00 = *(void **)(iVar2 + 0x48);
  iVar2 = FUN_004856c0(*(int *)((int)this + 0x6e4));
  FUN_004383a0(this_00,(void *)((int)this + 0x770),iVar2);
  FUN_004388f0((void *)((int)this + 0x770),2);
  FUN_0042b5d0((void *)((int)this + 0x770));
  FUN_0045e5b0((int)this + 0x770);
  *(undefined1 *)((int)this + 0xc08) = 1;
  *(undefined4 *)((int)this + 0xc18) = 0;
  *(undefined4 *)((int)this + 0xc1c) = 2;
  *(byte *)((int)this + 0xc13) = *(byte *)((int)this + 0xc13) & 0xfc | 1;
  iVar2 = FUN_0041ca90(*(int *)((int)this + 0x6f0));
  FUN_00438380(*(void **)(iVar2 + 0x48),(void *)((int)this + 0xd54),
               *(int *)((int)this + 0x750) + 0x3a);
  FUN_004388f0((void *)((int)this + 0xd54),2);
  FUN_0042b5d0((void *)((int)this + 0xd54));
  FUN_0045e5b0((int)this + 0xd54);
  *(undefined1 *)((int)this + 0x11ec) = 1;
  *(byte *)((int)this + 0x11f7) = *(byte *)((int)this + 0x11f7) & 0xfc | 1;
  if (-1 < *(int *)((int)this + 0x738)) {
    FUN_00426eb0(&DAT_005ba830,*(int *)((int)this + 0x738),0.0);
  }
  *(undefined4 *)((int)this + 0x58) = *(undefined4 *)((int)this + 0x6f8);
  *(undefined4 *)((int)this + 0x5c) = *(undefined4 *)((int)this + 0x6fc);
  *(undefined4 *)((int)this + 0x60) = *(undefined4 *)((int)this + 0x700);
  if (*(float *)((int)this + 0x744) != _DAT_0056e0e8) {
    FUN_00422e10(&local_14);
    FUN_00439330(this,&local_14,*(float *)((int)this + 0x710),*(float *)((int)this + 0x744));
    *(float *)((int)this + 0x58) = *(float *)((int)this + 0x58) + local_14;
    *(float *)((int)this + 0x5c) = *(float *)((int)this + 0x5c) + local_10;
  }
  *(undefined4 *)((int)this + 0x74) = *(undefined4 *)((int)this + 0x71c);
  *(undefined4 *)((int)this + 0x78) = DAT_0056c8d0;
  *(undefined4 *)((int)this + 0x7c) = *(undefined4 *)((int)this + 0x724);
  *(undefined4 *)((int)this + 0x70) = *(undefined4 *)((int)this + 0x710);
  *(undefined4 *)((int)this + 0x690) = *(undefined4 *)((int)this + 0x748);
  *(undefined4 *)((int)this + 0x8c) = *(undefined4 *)((int)this + 0x740);
  FUN_00423520((void *)((int)this + 0x38),0);
  *(undefined4 *)((int)this + 0x76c) = 0;
  puVar3 = FUN_004c7fd0();
  *(undefined4 **)((int)this + 0x6d8) = puVar3;
  *(undefined4 *)((int)this + 0x6dc) = 1;
  if (*(int *)((int)this + 0x6e4) == 0x26) {
    *(uint *)((int)this + 0x1c) = *(uint *)((int)this + 0x1c) | 0x80;
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

