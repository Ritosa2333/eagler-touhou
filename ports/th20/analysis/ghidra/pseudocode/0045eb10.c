/* Entry: 0x0045eb10; symbol: FUN_0045eb10; body bytes: 252 */

undefined4 __thiscall FUN_0045eb10(void *this,undefined4 *param_1,undefined4 param_2)

{
  void *pvVar1;
  
  *(undefined4 *)((int)this + 0xb5c) = param_2;
  *(undefined4 *)((int)this + 0xb48) = param_1[9];
  FUN_00423520((void *)((int)this + 0xb4c),0);
  pvVar1 = (void *)FUN_0045d690((void *)((int)this + 8),0);
  FUN_0045ec10(pvVar1,0xffff4040);
  pvVar1 = (void *)FUN_0045d690((void *)((int)this + 8),1);
  FUN_0045ec10(pvVar1,0xffc040ff);
  pvVar1 = (void *)FUN_0045d690((void *)((int)this + 8),2);
  FUN_0045ec10(pvVar1,0xffffff40);
  FUN_0045dd60(*(void **)((int)this + 4),param_1);
  FUN_00450350(*(void **)((int)this + 4),0xb);
  FUN_00438c60(*(void **)((int)this + 4),0x1e,0,0xff,0);
  FUN_0045dd40(*(void **)((int)this + 4),1);
  return 0;
}

