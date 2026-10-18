/* Entry: 0x0040ba00; symbol: FUN_0040ba00; body bytes: 126 */

void __fastcall FUN_0040ba00(undefined4 *param_1)

{
  void *this;
  uint uVar1;
  int iVar2;
  
  this = (void *)*param_1;
  if (((this != (void *)0x0) &&
      (uVar1 = FUN_0040c2d0((void *)((int)this + 4),2,(char **)0x4), uVar1 >> 1 == 1)) &&
     (iVar2 = FUN_0040c2d0(this,1,(char **)0x4), iVar2 == 1)) {
    FUN_0054278d(this);
  }
  return;
}

