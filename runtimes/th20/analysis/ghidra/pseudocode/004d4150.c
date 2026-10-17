/* Entry: 0x004d4150; symbol: FUN_004d4150; body bytes: 1284 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_004d4150(void *this,undefined4 *param_1)

{
  void *this_00;
  undefined *puVar1;
  int iVar2;
  undefined4 *puVar3;
  float local_14;
  float local_10;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
  FUN_004c8fe0((void *)((int)this + 0x6f8),param_1);
  *(undefined4 *)((int)this + 0x20) = 2;
  *(undefined4 *)((int)this + 0x24) = 0;
  *(undefined4 *)((int)this + 0x6e4) = *(undefined4 *)((int)this + 0x71c);
  *(undefined4 *)((int)this + 0x6e8) = *(undefined4 *)((int)this + 0x720);
  FUN_004d7dd0(this,*(undefined4 *)((int)this + 0x748));
  *(undefined4 *)((int)this + 0x6d0) = 0xffd08080;
  puVar1 = FUN_004856e0(*(int *)((int)this + 0x6e4));
  if (*(int *)(puVar1 + 0x14c) == 6) {
    puVar1 = FUN_004856e0(*(int *)((int)this + 0x6e4));
    *(undefined4 *)((int)this + 0x6d0) =
         *(undefined4 *)(puVar1 + *(int *)((int)this + 0x6e8) * 0x14 + 0x14);
  }
  FUN_004299d0((int)this + 0x74c);
  FUN_00438a30((void *)((int)this + 0x74c),FUN_004cf550);
  *(void **)((int)this + 0xd14) = this;
  iVar2 = FUN_0041ca90(*(int *)((int)this + 0x6f0));
  this_00 = *(void **)(iVar2 + 0x48);
  iVar2 = FUN_004856c0(*(int *)((int)this + 0x6e4));
  FUN_004383a0(this_00,(void *)((int)this + 0x74c),iVar2);
  FUN_004388f0((void *)((int)this + 0x74c),2);
  FUN_0042b5d0((void *)((int)this + 0x74c));
  FUN_0045e5b0((int)this + 0x74c);
  *(undefined1 *)((int)this + 0xbe4) = 1;
  *(undefined4 *)((int)this + 0xbf4) = 0;
  *(undefined4 *)((int)this + 0xbf8) = 2;
  *(byte *)((int)this + 0xbef) = *(byte *)((int)this + 0xbef) & 0xfc | 1;
  iVar2 = FUN_0041ca90(*(int *)((int)this + 0x6f0));
  FUN_00438380(*(void **)(iVar2 + 0x48),(void *)((int)this + 0xd30),
               *(int *)((int)this + 0x720) + 0x3a);
  FUN_004388f0((void *)((int)this + 0xd30),2);
  FUN_0042b5d0((void *)((int)this + 0xd30));
  FUN_0045e5b0((int)this + 0xd30);
  *(undefined1 *)((int)this + 0x11c8) = 1;
  *(byte *)((int)this + 0x11d3) = *(byte *)((int)this + 0x11d3) & 0xfc | 1;
  if ((*(int *)((int)this + 0x6e4) < 0x12) || (*(int *)((int)this + 0x6e4) == 0x26)) {
    iVar2 = FUN_0041ca90(*(int *)((int)this + 0x6f0));
    FUN_00438380(*(void **)(iVar2 + 0x48),(void *)((int)this + 0x1314),
                 *(int *)((int)this + 0x720) + 0x5d);
    FUN_0045e5b0((int)this + 0x1314);
  }
  else {
    iVar2 = FUN_0041ca90(*(int *)((int)this + 0x6f0));
    FUN_00438380(*(void **)(iVar2 + 0x48),(void *)((int)this + 0x1314),
                 *(int *)((int)this + 0x720) + 0x55);
  }
  *(byte *)((int)this + 0x17b7) = *(byte *)((int)this + 0x17b7) & 0xfc | 1;
  FUN_00423520((void *)((int)this + 0x6ac),0x1e);
  FUN_00423520((void *)((int)this + 0x6bc),3);
  if (-1 < *(int *)((int)this + 0x740)) {
    FUN_00426eb0(&DAT_005ba830,*(int *)((int)this + 0x740),0.0);
  }
  FUN_00423520((void *)((int)this + 0x38),0);
  FUN_00423520((void *)((int)this + 0x48),0);
  *(undefined4 *)((int)this + 0x58) = *(undefined4 *)((int)this + 0x6f8);
  *(undefined4 *)((int)this + 0x5c) = *(undefined4 *)((int)this + 0x6fc);
  *(undefined4 *)((int)this + 0x60) = *(undefined4 *)((int)this + 0x700);
  if (*(float *)((int)this + 0x724) != _DAT_0056e0e8) {
    FUN_00422e10(&local_14);
    FUN_00439330(this,&local_14,*(float *)((int)this + 0x704),*(float *)((int)this + 0x724));
    *(float *)((int)this + 0x58) = *(float *)((int)this + 0x58) + local_14;
    *(float *)((int)this + 0x5c) = *(float *)((int)this + 0x5c) + local_10;
  }
  *(undefined4 *)((int)this + 0x74) = *(undefined4 *)((int)this + 0x70c);
  *(undefined4 *)((int)this + 0x78) = *(undefined4 *)((int)this + 0x714);
  *(undefined4 *)((int)this + 0x7c) = *(undefined4 *)((int)this + 0x718);
  *(undefined4 *)((int)this + 0x70) = *(undefined4 *)((int)this + 0x704);
  if (*(float *)((int)this + 0x74) < *(float *)((int)this + 0x708) ||
      *(float *)((int)this + 0x74) == *(float *)((int)this + 0x708)) {
    *(undefined4 *)((int)this + 0x80) = 0;
  }
  else {
    *(undefined4 *)((int)this + 0x80) = DAT_0056e1a0;
  }
  FUN_00439330((void *)((int)this + 100),(float *)((int)this + 100),*(float *)((int)this + 0x70),
               *(float *)((int)this + 0x7c));
  *(undefined4 *)((int)this + 0x690) = *(undefined4 *)((int)this + 0x728);
  *(undefined4 *)((int)this + 0x88) = DAT_0056c8cc;
  *(undefined4 *)((int)this + 0x84) = DAT_0056c8cc;
  puVar3 = FUN_004c7fd0();
  *(undefined4 **)((int)this + 0x6d8) = puVar3;
  *(undefined4 *)((int)this + 0x6dc) = 1;
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

