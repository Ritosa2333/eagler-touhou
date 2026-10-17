/* Entry: 0x00423bf0; symbol: FUN_00423bf0; body bytes: 637 */

void __thiscall
FUN_00423bf0(void *this,undefined4 param_1,undefined4 param_2,undefined4 param_3,undefined4 param_4,
            undefined4 param_5,int param_6)

{
  int *piVar1;
  
  switch(param_1) {
  case 0:
    piVar1 = FUN_004122c0(0x18,FUN_004240c0,this);
    *(int **)((int)this + 8) = piVar1;
    piVar1 = FUN_00412360(param_6,FUN_00424e00,this);
    *(int **)((int)this + 0xc) = piVar1;
    break;
  case 1:
    piVar1 = FUN_004122c0(0x18,FUN_004249f0,this);
    *(int **)((int)this + 8) = piVar1;
    break;
  case 2:
    piVar1 = FUN_004122c0(0x18,FUN_00424260,this);
    *(int **)((int)this + 8) = piVar1;
    piVar1 = FUN_00412360(param_6,FUN_00424f30,this);
    *(int **)((int)this + 0xc) = piVar1;
    break;
  case 3:
    *(undefined4 *)((int)this + 0x18) = 0xff;
    piVar1 = FUN_004122c0(0x18,FUN_004240c0,this);
    *(int **)((int)this + 8) = piVar1;
    piVar1 = FUN_00412360(param_6,FUN_00424f30,this);
    *(int **)((int)this + 0xc) = piVar1;
    break;
  case 4:
    piVar1 = FUN_004122c0(0x18,FUN_00424350,this);
    *(int **)((int)this + 8) = piVar1;
    piVar1 = FUN_00412360(param_6,FUN_00424fa0,this);
    *(int **)((int)this + 0xc) = piVar1;
    break;
  case 5:
    piVar1 = FUN_004122c0(0x18,FUN_00424260,this);
    *(int **)((int)this + 8) = piVar1;
    piVar1 = FUN_00412360(param_6,FUN_00424e00,this);
    *(int **)((int)this + 0xc) = piVar1;
    break;
  case 6:
    piVar1 = FUN_004122c0(0x18,FUN_00424180,this);
    *(int **)((int)this + 8) = piVar1;
    piVar1 = FUN_00412360(param_6,FUN_00424f30,this);
    *(int **)((int)this + 0xc) = piVar1;
    break;
  case 7:
    piVar1 = FUN_004122c0(0x18,FUN_00424180,this);
    *(int **)((int)this + 8) = piVar1;
    piVar1 = FUN_00412360(param_6,FUN_00424e80,this);
    *(int **)((int)this + 0xc) = piVar1;
    break;
  case 8:
    piVar1 = FUN_004122c0(0x18,FUN_004244c0,this);
    *(int **)((int)this + 8) = piVar1;
    break;
  case 9:
    piVar1 = FUN_004122c0(0x18,FUN_00424460,this);
    *(int **)((int)this + 8) = piVar1;
    piVar1 = FUN_00412360(param_6,FUN_00424f30,this);
    *(int **)((int)this + 0xc) = piVar1;
  }
  FUN_00412da0(*(void **)((int)this + 8),FUN_00425070);
  FUN_00423520((void *)((int)this + 0x30),0);
  *(undefined4 *)((int)this + 0x10) = param_1;
  *(undefined4 *)((int)this + 0x1c) = param_2;
  *(undefined4 *)((int)this + 0x20) = param_3;
  *(undefined4 *)((int)this + 0x24) = param_4;
  *(undefined4 *)((int)this + 0x28) = param_5;
  return;
}

