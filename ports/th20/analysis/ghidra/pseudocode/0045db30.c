/* Entry: 0x0045db30; symbol: FUN_0045db30; body bytes: 182 */

undefined4 __thiscall FUN_0045db30(void *this,undefined4 *param_1,undefined4 param_2)

{
  void *this_00;
  
  *(undefined4 *)((int)this + 0x3dc) = param_2;
  *(undefined4 *)((int)this + 0x3c8) = param_1[9];
  FUN_00423520((void *)((int)this + 0x3cc),0);
  this_00 = (void *)FUN_0045d690((void *)((int)this + 8),0);
  FUN_0045dbf0(this_00,0xff0000);
  FUN_0045dd60(*(void **)((int)this + 4),param_1);
  FUN_00450350(*(void **)((int)this + 4),0xb);
  FUN_00438c60(*(void **)((int)this + 4),0x1e,0,0xff,0);
  FUN_0045dd40(*(void **)((int)this + 4),1);
  return 0;
}

