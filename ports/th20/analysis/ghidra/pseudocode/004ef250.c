/* Entry: 0x004ef250; symbol: FUN_004ef250; body bytes: 542 */

undefined4 __thiscall FUN_004ef250(void *this,undefined4 param_1)

{
  undefined4 uVar1;
  undefined8 uVar2;
  
                    /* WARNING: Load size is inaccurate */
  switch(*this) {
  case 0:
    move<>(param_1);
    uVar1 = FUN_004e6f40();
    break;
  case 1:
    move<>(param_1);
    uVar1 = FUN_004e70d0(*(int *)((int)this + 8));
    break;
  case 2:
    move<>(param_1);
    uVar1 = FUN_004e71a0(*(undefined4 *)((int)this + 8));
    break;
  case 3:
    move<>(param_1);
    uVar2 = FUN_004e78a0(*(undefined4 *)((int)this + 8),*(int *)((int)this + 0xc));
    uVar1 = (undefined4)uVar2;
    break;
  case 4:
    move<>(param_1);
    uVar2 = FUN_004e7990(*(undefined8 *)((int)this + 8));
    uVar1 = (undefined4)uVar2;
    break;
  case 5:
    move<>(param_1);
    uVar1 = FUN_004e6f40();
    break;
  case 6:
    move<>(param_1);
    uVar1 = FUN_004e6f40();
    break;
  case 7:
    move<>(param_1);
    uVar1 = FUN_004e6f40();
    break;
  case 8:
    move<>(param_1);
    uVar1 = FUN_004e6f40();
    break;
  case 9:
    move<>(param_1);
    uVar1 = FUN_004e6f40();
    break;
  case 10:
    move<>(param_1);
    uVar1 = FUN_004e6f40();
    break;
  case 0xb:
    move<>(param_1);
    uVar1 = FUN_004e6f40();
    break;
  case 0xc:
    move<>(param_1);
    uVar1 = FUN_004e6f40();
    break;
  case 0xd:
    move<>(param_1);
    uVar1 = FUN_004e6f40();
    break;
  default:
                    /* WARNING: Subroutine does not return */
    __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
  }
  return uVar1;
}

