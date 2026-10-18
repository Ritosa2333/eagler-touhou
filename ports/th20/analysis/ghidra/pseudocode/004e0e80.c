/* Entry: 0x004e0e80; symbol: FUN_004e0e80; body bytes: 179 */

undefined4 __thiscall FUN_004e0e80(void *this,int *param_1)

{
  int *piVar1;
  
  piVar1 = FUN_004122c0(10,FUN_004e0f60,this);
  *(int **)((int)this + 8) = piVar1;
  piVar1 = FUN_00412360(0x5e,FUN_004e0f70,this);
  *(int **)((int)this + 0xc) = piVar1;
                    /* WARNING: Load size is inaccurate */
  (**(code **)(*this + 4))();
  FUN_004bed50((void *)((int)this + 0x14),0);
  FUN_004bfbe0((void *)((int)this + 0x14),1);
  FUN_00423520((void *)((int)this + 0x60),0);
  FUN_00423520((void *)((int)this + 0x7c),0);
  FUN_00423520((void *)((int)this + 0x8c),0);
  *(int *)((int)this + 0x70) = *param_1;
  *(int *)((int)this + 0x74) = param_1[1];
  *(int *)((int)this + 0x78) = param_1[2];
  return 0;
}

