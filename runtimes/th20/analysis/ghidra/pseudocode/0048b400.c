/* Entry: 0x0048b400; symbol: FUN_0048b400; body bytes: 248 */

undefined4 * __thiscall FUN_0048b400(void *this,undefined4 *param_1)

{
  *(undefined4 *)this = *param_1;
  FUN_0048b2f0((_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 1));
  *(undefined4 *)((int)this + 0x14) = param_1[5];
  *(undefined4 *)((int)this + 0x18) = param_1[6];
  *(undefined4 *)((int)this + 0x1c) = param_1[7];
  *(undefined4 *)((int)this + 0x20) = param_1[8];
  *(undefined4 *)((int)this + 0x24) = param_1[9];
  *(undefined4 *)((int)this + 0x28) = param_1[10];
  *(undefined4 *)((int)this + 0x2c) = param_1[0xb];
  *(undefined4 *)((int)this + 0x30) = param_1[0xc];
  *(undefined4 *)((int)this + 0x34) = param_1[0xd];
  *(undefined2 *)((int)this + 0x38) = *(undefined2 *)(param_1 + 0xe);
  *(undefined2 *)((int)this + 0x3a) = *(undefined2 *)((int)param_1 + 0x3a);
  *(undefined4 *)((int)this + 0x3c) = param_1[0xf];
  *(undefined4 *)((int)this + 0x40) = param_1[0x10];
  *(undefined4 *)((int)this + 0x44) = param_1[0x11];
  *(undefined1 *)((int)this + 0x48) = *(undefined1 *)(param_1 + 0x12);
  *(undefined1 *)((int)this + 0x49) = *(undefined1 *)((int)param_1 + 0x49);
  return (undefined4 *)this;
}

