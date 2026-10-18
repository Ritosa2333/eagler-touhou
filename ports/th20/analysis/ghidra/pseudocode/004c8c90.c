/* Entry: 0x004c8c90; symbol: FUN_004c8c90; body bytes: 216 */

undefined4 * __thiscall FUN_004c8c90(void *this,undefined4 *param_1)

{
  *(undefined4 *)this = *param_1;
  *(undefined4 *)((int)this + 4) = param_1[1];
  *(undefined4 *)((int)this + 8) = param_1[2];
  *(undefined4 *)((int)this + 0xc) = param_1[3];
  *(undefined4 *)((int)this + 0x10) = param_1[4];
  *(undefined4 *)((int)this + 0x14) = param_1[5];
  *(undefined4 *)((int)this + 0x18) = param_1[6];
  *(undefined4 *)((int)this + 0x1c) = param_1[7];
  *(undefined4 *)((int)this + 0x20) = param_1[8];
  *(undefined4 *)((int)this + 0x24) = param_1[9];
  *(undefined4 *)((int)this + 0x28) = param_1[10];
  *(undefined4 *)((int)this + 0x2c) = param_1[0xb];
  *(undefined4 *)((int)this + 0x30) = param_1[0xc];
  *(undefined4 *)((int)this + 0x34) = param_1[0xd];
  *(undefined4 *)((int)this + 0x38) = param_1[0xe];
  *(undefined4 *)((int)this + 0x38) = param_1[0xe];
  FUN_0047c570((void *)((int)this + 0x3c),
               (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_1 + 0xf));
  *(undefined4 *)((int)this + 0x4c) = param_1[0x13];
  return (undefined4 *)this;
}

