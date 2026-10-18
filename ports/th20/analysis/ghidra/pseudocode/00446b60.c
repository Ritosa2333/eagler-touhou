/* Entry: 0x00446b60; symbol: FUN_00446b60; body bytes: 57 */

undefined4 * __thiscall FUN_00446b60(void *this,void *param_1)

{
  *(undefined ***)this = &PTR_FID_conflict__scalar_deleting_destructor__0056e1ac;
  *(undefined4 *)((int)this + 4) = 0;
  *(void **)((int)this + 4) = param_1;
  FUN_00446bf0(param_1,this);
  return (undefined4 *)this;
}

