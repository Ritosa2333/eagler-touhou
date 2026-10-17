/* Entry: 0x0046ca40; symbol: FUN_0046ca40; body bytes: 467 */

void __thiscall FUN_0046ca40(void *this,undefined4 *param_1)

{
  int iVar1;
  float10 fVar2;
  
  if (*(int *)((int)this + 0x1a1bc) < 0x140) {
    switch(*(undefined4 *)((int)this + 0x1a1e0)) {
    case 6:
      FUN_0046d090(this,param_1,8);
      break;
    case 7:
      FUN_0046d090(this,param_1,9);
      break;
    case 10:
      FUN_0046d090(this,param_1,0xb);
      break;
    case 0xc:
      FUN_0046d090(this,param_1,0xd);
    }
    iVar1 = FUN_0046b210((void *)((int)this + 0x11bc),*(int *)((int)this + 0x1a1bc));
    *(int *)((int)this + 0x1a1bc) = *(int *)((int)this + 0x1a1bc) + 1;
    *(undefined4 *)(iVar1 + 0x100) = *param_1;
    *(undefined4 *)(iVar1 + 0x104) = param_1[1];
    *(undefined4 *)(iVar1 + 0x108) = param_1[2];
    FUN_00429690((void *)(iVar1 + 0x100),DAT_005b8818);
    *(undefined4 *)(iVar1 + 0x130) = *(undefined4 *)((int)this + 0x1a1ec);
    *(undefined4 *)(iVar1 + 0x10c) = *(undefined4 *)((int)this + 0x1a1c0);
    *(undefined4 *)(iVar1 + 0x114) = *(undefined4 *)((int)this + 0x1a1cc);
    *(undefined4 *)(iVar1 + 0x118) = *(undefined4 *)((int)this + 0x1a1d0);
    *(undefined4 *)(iVar1 + 0x124) = *(undefined4 *)((int)this + 0x1a1d4);
    *(undefined4 *)(iVar1 + 0x128) = *(undefined4 *)((int)this + 0x1a1e0);
    *(undefined4 *)(iVar1 + 300) = *(undefined4 *)((int)this + 0x1a1dc);
    *(undefined4 *)(iVar1 + 0x134) = *(undefined4 *)((int)this + 0x1a1f0);
    *(undefined4 *)(iVar1 + 0x110) = *(undefined4 *)((int)this + 0x1a1fc);
    *(undefined4 *)(iVar1 + 0x138) = *(undefined4 *)((int)this + 0x1a1f4);
    *(undefined4 *)(iVar1 + 0x13c) = *(undefined4 *)((int)this + 0x1a1f8);
    fVar2 = FUN_004292e0((float *)((int)this + 0x1a208));
    *(float *)(iVar1 + 0x11c) = (float)fVar2;
  }
  return;
}

