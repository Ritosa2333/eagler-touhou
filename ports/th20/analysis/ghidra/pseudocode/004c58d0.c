/* Entry: 0x004c58d0; symbol: FUN_004c58d0; body bytes: 192 */

undefined4 __thiscall FUN_004c58d0(void *this,int *param_1)

{
  int *piVar1;
  
  piVar1 = FUN_004122c0(10,FUN_004c5d70,this);
  *(int **)((int)this + 8) = piVar1;
  piVar1 = FUN_00412360(0x5e,FUN_004c5d80,this);
  *(int **)((int)this + 0xc) = piVar1;
                    /* WARNING: Load size is inaccurate */
  (**(code **)(*this + 4))();
  FUN_004bed50((void *)((int)this + 0x14),0);
  FUN_004bfbe0((void *)((int)this + 0x14),1);
  FUN_00423520((void *)((int)this + 0xc0),0);
  *(int *)((int)this + 0xe0) = *param_1;
  *(int *)((int)this + 0xe4) = param_1[1];
  *(int *)((int)this + 0xe8) = param_1[2];
  *(undefined4 *)((int)this + 0x110) = 1;
  *(undefined4 *)((int)this + 0x10c) = 0;
  FUN_004c61d0(this,0);
  return 0;
}

