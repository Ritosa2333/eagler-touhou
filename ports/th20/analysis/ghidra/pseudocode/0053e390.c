/* Entry: 0x0053e390; symbol: FUN_0053e390; body bytes: 182 */

undefined4 __thiscall FUN_0053e390(void *this,undefined4 param_1,int param_2)

{
  undefined4 *puVar1;
  undefined4 *this_00;
  undefined4 uVar2;
  
  puVar1 = FUN_0053b3d0();
  this_00 = FUN_0053b410();
  puVar1[10] = this;
  *puVar1 = 0;
  puVar1[2] = 0xffffffff;
  puVar1[1] = 0xffffffff;
  puVar1[9] = param_1;
  *(undefined1 *)(puVar1 + 0xc) = *(undefined1 *)(*(int *)((int)this + 0xc) + 0x30);
  FUN_0044a670(this_00,puVar1);
  FUN_00411ee0((void *)((int)this + 0x5c),(int)this_00);
  uVar2 = FUN_0053f3b0(*(void **)((int)this + 0xc),(int)puVar1,param_2,0);
  return uVar2;
}

