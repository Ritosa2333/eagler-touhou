/* Entry: 0x0044b970; symbol: FUN_0044b970; body bytes: 118 */

void * __thiscall FUN_0044b970(void *this,uint *param_1,undefined1 *param_2)

{
  uint *puVar1;
  byte local_6;
  undefined1 local_5;
  
  if (*(undefined1 **)((int)this + 0x18) < param_2) {
    this = (void *)FUN_00447050(this,(uint)param_2,(uint)local_6,param_1);
  }
  else {
    puVar1 = Myptr((undefined4 *)((int)this + 4));
    *(undefined1 **)((int)this + 0x14) = param_2;
    FUN_00411480(puVar1,param_1,param_2);
    local_5 = 0;
    FUN_0040fe90((undefined1 *)((int)puVar1 + (int)param_2),&local_5);
  }
  return this;
}

