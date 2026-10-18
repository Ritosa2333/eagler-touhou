/* Entry: 0x004eec20; symbol: FUN_004eec20; body bytes: 845 */

undefined4 * __thiscall FUN_004eec20(void *this,undefined4 *param_1,undefined4 param_2)

{
  undefined4 uVar1;
  void *pvVar2;
  
                    /* WARNING: Load size is inaccurate */
  switch(*this) {
  case 0:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e76c0(pvVar2,param_1);
    break;
  case 1:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e7080(pvVar2,param_1,*(uint *)((int)this + 8));
    break;
  case 2:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e7170(pvVar2,param_1,*(uint *)((int)this + 8));
    break;
  case 3:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e7830(pvVar2,param_1,*(int *)((int)this + 8),*(int *)((int)this + 0xc));
    break;
  case 4:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e7950(pvVar2,param_1,*(undefined4 *)((int)this + 8),*(undefined4 *)((int)this + 0xc));
    break;
  case 5:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e7a90(pvVar2,param_1,*(char *)((int)this + 8));
    break;
  case 6:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e6f00(pvVar2,param_1);
    break;
  case 7:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e7230(pvVar2,param_1,*(float *)((int)this + 8));
    break;
  case 8:
    uVar1 = move<>(param_2);
    FUN_004e72f0(uVar1,param_1,*(double *)((int)this + 8));
    break;
  case 9:
    uVar1 = move<>(param_2);
    FUN_004e72f0(uVar1,param_1,*(double *)((int)this + 8));
    break;
  case 10:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e7610(pvVar2,param_1,*(uint *)((int)this + 8));
    break;
  case 0xb:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e7560(pvVar2,param_1,*(char **)((int)this + 8));
    break;
  case 0xc:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e7770(pvVar2,param_1);
    break;
  case 0xd:
    pvVar2 = (void *)move<>(param_2);
    FUN_004f6fe0(pvVar2,param_1);
    break;
  default:
                    /* WARNING: Subroutine does not return */
    __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
  }
  return param_1;
}

