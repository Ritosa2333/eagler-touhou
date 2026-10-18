/* Entry: 0x004646e0; symbol: FUN_004646e0; body bytes: 175 */

undefined4 * __thiscall FUN_004646e0(void *this,void *param_1)

{
  int *unaff_FS_OFFSET;
  int local_10;
  undefined1 *puStack_c;
  undefined4 local_8;
  
  local_8 = 0xffffffff;
  puStack_c = &LAB_00568b0d;
  local_10 = *unaff_FS_OFFSET;
  *unaff_FS_OFFSET = (int)&local_10;
  FUN_00446b60(this,param_1);
  local_8 = 0;
  *(undefined ***)this = &PTR_FUN_0056f098;
  _eh_vector_constructor_iterator_((void *)((int)this + 8),0x5e4,4,FUN_00448b40,FUN_00449370);
  FUN_00448b40((undefined4 *)((int)this + 0x1798));
  *(undefined4 *)((int)this + 0x1d7c) = 0;
  *(undefined4 *)((int)this + 0x1d80) = 0;
  *(undefined4 *)((int)this + 0x1d84) = 0;
  *unaff_FS_OFFSET = local_10;
  return (undefined4 *)this;
}

