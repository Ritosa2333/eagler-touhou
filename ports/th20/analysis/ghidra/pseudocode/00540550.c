/* Entry: 0x00540550; symbol: FUN_00540550; body bytes: 51 */

void __thiscall FUN_00540550(void *this,char *param_1)

{
  int iVar1;
  
  iVar1 = FUN_00540340(*(void **)((int)this + 0x58),param_1);
  *(int *)(*(int *)((int)this + 0xc) + 4) = iVar1;
  return;
}

