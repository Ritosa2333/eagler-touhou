/* Entry: 0x00547db0; symbol: FUN_00547db0; body bytes: 72 */

undefined4 * __thiscall FUN_00547db0(void *this,undefined4 *param_1)

{
  undefined *puVar1;
  undefined *puVar2;
  
  *(undefined1 *)((int)this + 0x14) = 0;
  *(undefined4 *)this = 0;
  *(undefined1 *)((int)this + 8) = 0;
  *(undefined1 *)((int)this + 0x1c) = 0;
  *(undefined1 *)((int)this + 0x24) = 0;
  if (param_1 == (undefined4 *)0x0) {
    puVar1 = PTR_PTR_005b2840;
    puVar2 = PTR_DAT_005b2844;
    if (DAT_005e55dc != 0) {
      return (undefined4 *)this;
    }
  }
  else {
    puVar1 = (undefined *)*param_1;
    puVar2 = (undefined *)param_1[1];
  }
  *(undefined1 *)((int)this + 0x14) = 1;
  *(undefined **)((int)this + 0x10) = puVar2;
  *(undefined **)((int)this + 0xc) = puVar1;
  return (undefined4 *)this;
}

