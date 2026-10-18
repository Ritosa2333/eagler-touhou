/* Entry: 0x0045fb60; symbol: FUN_0045fb60; body bytes: 296 */

undefined4 __thiscall FUN_0045fb60(void *this,undefined4 *param_1,undefined4 param_2)

{
  void *pvVar1;
  
  *(undefined4 *)((int)this + 0xb98) = param_2;
  *(undefined4 *)((int)this + 0xb84) = param_1[9];
  *(undefined4 *)((int)this + 0xb9c) = *param_1;
  *(undefined4 *)((int)this + 0xba0) = param_1[1];
  *(undefined4 *)((int)this + 0xba4) = param_1[2];
  *(undefined1 *)((int)this + 0xba8) = *(undefined1 *)(param_1 + 10);
  FUN_00423520((void *)((int)this + 0xb88),0);
  pvVar1 = (void *)FUN_0045f400((void *)((int)this + 8),0);
  FUN_0045fc90(pvVar1,0x8040f010);
  pvVar1 = (void *)FUN_0045f400((void *)((int)this + 8),1);
  FUN_0045fc90(pvVar1,0xff20f000);
  pvVar1 = (void *)FUN_0045f400((void *)((int)this + 8),2);
  FUN_0045fc90(pvVar1,0x80ff1010);
  FUN_0045dd60(*(void **)((int)this + 4),param_1);
  FUN_00450350(*(void **)((int)this + 4),0xb);
  FUN_00438c60(*(void **)((int)this + 4),0x1e,0,0xff,0);
  FUN_0045dd40(*(void **)((int)this + 4),1);
  return 0;
}

