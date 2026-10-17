/* Entry: 0x00459530; symbol: FUN_00459530; body bytes: 255 */

undefined4 * __thiscall
FUN_00459530(void *this,int param_1,undefined4 param_2,uint param_3,undefined4 param_4)

{
  undefined4 uVar1;
  uint local_c;
  
  *(undefined ***)this = &PTR__scalar_deleting_destructor__0056e73c;
  uVar1 = FUN_00447480("D:\\cygwin\\home\\zun\\prog\\th20\\src\\core\\zwave.cpp:278 LPDIRECTSOUNDBUFFER"
                       ,param_3);
  *(undefined4 *)((int)this + 8) = uVar1;
  for (local_c = 0; local_c < param_3; local_c = local_c + 1) {
    *(undefined4 *)(*(int *)((int)this + 8) + local_c * 4) = *(undefined4 *)(param_1 + local_c * 4);
  }
  *(undefined4 *)((int)this + 0xc) = param_2;
  *(uint *)((int)this + 0x14) = param_3;
  *(undefined4 *)((int)this + 0x10) = param_4;
  FUN_0045a240(this,(int *)**(undefined4 **)((int)this + 8),0,0);
  for (local_c = 0; local_c < param_3; local_c = local_c + 1) {
    (**(code **)(**(int **)(*(int *)((int)this + 8) + local_c * 4) + 0x34))
              (*(undefined4 *)(*(int *)((int)this + 8) + local_c * 4),0);
  }
  *(undefined4 *)((int)this + 0x58) = 0;
  *(undefined4 *)((int)this + 0x5c) = 0;
  return (undefined4 *)this;
}

