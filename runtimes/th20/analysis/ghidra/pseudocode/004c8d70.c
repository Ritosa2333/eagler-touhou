/* Entry: 0x004c8d70; symbol: FUN_004c8d70; body bytes: 278 */

void * __thiscall FUN_004c8d70(void *this,void *param_1,undefined4 *param_2)

{
  *(undefined4 *)this = *param_2;
  *(undefined4 *)((int)this + 4) = param_2[1];
  *(undefined4 *)((int)this + 8) = param_2[2];
  *(undefined4 *)((int)this + 0xc) = param_2[3];
  *(undefined4 *)((int)this + 0x10) = param_2[4];
  *(undefined4 *)((int)this + 0x14) = param_2[5];
  *(undefined4 *)((int)this + 0x18) = param_2[6];
  *(undefined4 *)((int)this + 0x1c) = param_2[7];
  *(undefined4 *)((int)this + 0x20) = param_2[8];
  *(undefined4 *)((int)this + 0x24) = param_2[9];
  *(undefined4 *)((int)this + 0x28) = param_2[10];
  FUN_0047c570((void *)((int)this + 0x2c),
               (_Compressed_pair<struct_std::default_delete<class_std::_Facet_base>,class_std::_Facet_base*,1>
                *)(param_2 + 0xb));
  *(undefined4 *)((int)this + 0x3c) = param_2[0xf];
  *(undefined4 *)((int)this + 0x40) = param_2[0x10];
  *(undefined4 *)((int)this + 0x44) = param_2[0x11];
  *(undefined4 *)((int)this + 0x48) = param_2[0x12];
  *(undefined4 *)((int)this + 0x4c) = param_2[0x13];
  *(undefined4 *)((int)this + 0x50) = param_2[0x14];
  *(undefined4 *)((int)this + 0x54) = param_2[0x15];
  *(undefined4 *)((int)this + 0x58) = param_2[0x16];
  FUN_004c84b0(param_1,(undefined4 *)this);
  return param_1;
}

