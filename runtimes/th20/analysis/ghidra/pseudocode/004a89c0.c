/* Entry: 0x004a89c0; symbol: FUN_004a89c0; body bytes: 780 */

undefined4 __thiscall FUN_004a89c0(void *this,undefined4 *param_1)

{
  void *this_00;
  undefined4 uVar1;
  GpImage *pGVar2;
  uint uVar3;
  int iVar4;
  undefined4 *puVar5;
  undefined4 *puVar6;
  
  puVar5 = param_1;
  puVar6 = (undefined4 *)((int)this + 0x378);
  for (iVar4 = 0x15; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar6 = *puVar5;
    puVar5 = puVar5 + 1;
    puVar6 = puVar6 + 1;
  }
  FUN_0049c2c0((void *)((int)this + 0x1e0),1);
  this_00 = (void *)FUN_0048bd10((void *)((int)this + 0x1e0),0);
  FUN_004393f0(this_00,param_1);
  FUN_004117a0((void *)((int)this + 0x210),param_1[3]);
  FUN_004117a0((void *)((int)this + 0x214),param_1[5]);
  Gdiplus::Image::SetNativeImage((Image *)((int)this + 0x214),(GpImage *)param_1[5]);
  FUN_0049c4e0((void *)((int)this + 0x214),param_1[5]);
  Gdiplus::Image::SetNativeImage((Image *)((int)this + 0x230),(GpImage *)param_1[4]);
  *(uint *)((int)this + 0x354) = *(uint *)((int)this + 0x354) & 0xfffffff7 | (param_1[6] & 1) << 3;
  uVar1 = FUN_004640a0(0x5ba568);
  *(undefined4 *)((int)this + 0xd8) = uVar1;
  if (*(int *)((int)this + 0xd8) < 4) {
    *(char *)((int)this + 0x40) = (char)(1 << ((byte)*(undefined4 *)((int)this + 0xd8) & 0x1f));
  }
  else {
    *(undefined1 *)((int)this + 0x40) = 2;
  }
  puVar5 = param_1 + 8;
  puVar6 = (undefined4 *)((int)this + 0x100);
  for (iVar4 = 0xc; iVar4 != 0; iVar4 = iVar4 + -1) {
    *puVar6 = *puVar5;
    puVar5 = puVar5 + 1;
    puVar6 = puVar6 + 1;
  }
  FUN_00423520((void *)((int)this + 0x310),2);
  *(uint *)((int)this + 0x354) = *(uint *)((int)this + 0x354) & 0xfffffbff | (param_1[7] & 1) << 10;
  *(undefined4 *)((int)this + 0xb8) = 0;
  *(undefined4 *)((int)this + 0x8c) = param_1[0x14];
  if (999 < (int)param_1[5]) {
    *(uint *)((int)this + 0x354) = *(uint *)((int)this + 0x354) | 0x4000;
  }
  uVar1 = FUN_00499330(0x5ba568);
  *(undefined4 *)((int)this + 0x308) = uVar1;
  FUN_004a8760((int)this);
  if ((*(uint *)((int)this + 0x354) >> 7 & 1) != 0) {
    pGVar2 = (GpImage *)FUN_0040ff90((int)this + 0x230);
    Gdiplus::Image::SetNativeImage((Image *)((int)this + 0x230),pGVar2);
  }
  uVar3 = FUN_0040c300((undefined4 *)((int)this + 0x88));
  *(uint *)((int)this + 0x2d8) = (uVar3 & 1) + 3;
  if (*(int *)((int)this + 0x2dc) == 0) {
    *(undefined4 *)((int)this + 0x2dc) = 0x25;
    if (*(int *)((int)this + 0xa8) == 2) {
      switch(*(undefined4 *)((int)this + 0xac)) {
      case 0:
      case 0x14:
      case 0x3b:
      case 0x3e:
      case 0x68:
        *(undefined4 *)((int)this + 0x2dc) = 0x25;
        break;
      case 5:
      case 0x19:
      case 0x35:
      case 0x5e:
        *(undefined4 *)((int)this + 0x2dc) = 0x21;
        break;
      case 10:
      case 0x38:
      case 99:
        *(undefined4 *)((int)this + 0x2dc) = 0x29;
        break;
      case 0xf:
      case 0x6d:
        *(undefined4 *)((int)this + 0x2dc) = 0x2d;
        break;
      case 0x1e:
        *(undefined4 *)((int)this + 0x2dc) = 0x33;
        break;
      case 0x23:
        *(undefined4 *)((int)this + 0x2dc) = 0x32;
        break;
      case 0x28:
        *(undefined4 *)((int)this + 0x2dc) = 0x31;
      }
    }
    *(undefined4 *)((int)this + 0x2e0) = 1;
  }
  return 0;
}

