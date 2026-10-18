/* Entry: 0x004219d0; symbol: FUN_004219d0; body bytes: 218 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

void __thiscall FUN_004219d0(void *this,int param_1,undefined4 param_2)

{
  code *pcVar1;
  int *piVar2;
  undefined4 uVar3;
  
  piVar2 = (int *)FUN_00421850(DAT_005b8898,param_1);
  if (piVar2 != (int *)0x0) {
    (**(code **)(*piVar2 + 0x2c))(piVar2,&DAT_00576b60);
    pcVar1 = *(code **)(*piVar2 + 0x34);
    uVar3 = FUN_0040c300(&DAT_005b6758);
    (*pcVar1)(piVar2,uVar3,10);
    _DAT_005c4d5c = 0x2c;
    (**(code **)(*piVar2 + 0xc))(piVar2,&DAT_005c4d5c);
    *(int **)((int)this + 8) = piVar2;
    (**(code **)(*piVar2 + 0x10))(piVar2,FUN_00420180,0,0);
    FUN_00454150(&DAT_005c0678,(wchar_t *)&DAT_0056d654);
    *(undefined4 *)this = 1;
    *(undefined4 *)((int)this + 0xc) = 0xffffffff;
    *(undefined4 *)((int)this + 4) = param_2;
  }
  return;
}

