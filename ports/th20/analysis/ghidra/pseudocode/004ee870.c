/* Entry: 0x004ee870; symbol: FUN_004ee870; body bytes: 845 */

undefined4 * __thiscall FUN_004ee870(void *this,undefined4 *param_1,undefined4 param_2)

{
  undefined4 *puVar1;
  void *pvVar2;
  
                    /* WARNING: Load size is inaccurate */
  switch(*this) {
  case 0:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e7640(pvVar2,param_1);
    break;
  case 1:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e7000(pvVar2,param_1,*(uint *)((int)this + 8));
    break;
  case 2:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e70f0(pvVar2,param_1,*(uint *)((int)this + 8));
    break;
  case 3:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e77b0(pvVar2,param_1,*(uint *)((int)this + 8),*(uint *)((int)this + 0xc));
    break;
  case 4:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e78d0(pvVar2,param_1,*(uint *)((int)this + 8),*(int *)((int)this + 0xc));
    break;
  case 5:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e79a0(pvVar2,param_1,*(byte *)((int)this + 8));
    break;
  case 6:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e6e10(pvVar2,param_1,*(byte *)((int)this + 8));
    break;
  case 7:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e71b0(pvVar2,param_1,*(float *)((int)this + 8));
    break;
  case 8:
    puVar1 = (undefined4 *)move<>(param_2);
    FUN_004e7270(puVar1,param_1,*(double *)((int)this + 8));
    break;
  case 9:
    puVar1 = (undefined4 *)move<>(param_2);
    FUN_004e7330(puVar1,param_1,*(double *)((int)this + 8));
    break;
  case 10:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e7590(pvVar2,param_1,*(uint *)((int)this + 8));
    break;
  case 0xb:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e74e0(pvVar2,param_1,*(char **)((int)this + 8));
    break;
  case 0xc:
    pvVar2 = (void *)move<>(param_2);
    FUN_004e76f0(pvVar2,param_1,*(undefined4 *)((int)this + 8),*(undefined4 *)((int)this + 0xc));
    break;
  case 0xd:
    move<>(param_2);
    FUN_004f6fa0();
    break;
  default:
                    /* WARNING: Subroutine does not return */
    __invoke_watson((wchar_t *)0x0,(wchar_t *)0x0,(wchar_t *)0x0,0,0);
  }
  return param_1;
}

