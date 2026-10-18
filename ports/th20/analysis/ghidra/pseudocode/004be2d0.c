/* Entry: 0x004be2d0; symbol: FUN_004be2d0; body bytes: 106 */

void __thiscall FUN_004be2d0(void *this,uint param_1,int param_2)

{
  longlong lVar1;
  undefined4 local_14;
  undefined4 local_10;
  
  lVar1 = __allmul(param_1,param_2,10,0);
  local_10 = (uint)((ulonglong)lVar1 >> 0x20);
  if ((*(uint *)((int)this + 0xdc) <= local_10) &&
     ((*(uint *)((int)this + 0xdc) < local_10 ||
      (local_14 = (uint)lVar1, *(uint *)((int)this + 0xd8) < local_14)))) {
    lVar1 = __allmul(param_1,param_2,10,0);
    *(longlong *)((int)this + 0xd8) = lVar1;
  }
  return;
}

