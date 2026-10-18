/* Entry: 0x004ff0d0; symbol: FUN_004ff0d0; body bytes: 575 */

void __thiscall FUN_004ff0d0(void *this,undefined4 *param_1,uint param_2)

{
  undefined1 uVar1;
  undefined4 uVar2;
  undefined4 *puVar3;
  float10 fVar4;
  undefined8 uVar5;
  undefined4 local_28 [2];
  undefined4 local_20 [2];
  uint local_18;
  void *local_14;
  double *local_10;
  uint local_c;
  uint local_8;
  
  local_8 = DAT_005b25c0 ^ (uint)&stack0xfffffffc;
                    /* WARNING: Load size is inaccurate */
  local_14 = this;
  if (param_2 < *this) {
    local_c = *(uint *)(*(int *)((int)this + 4) + param_2 * 4);
                    /* WARNING: Load size is inaccurate */
    local_10 = (double *)(*(int *)((int)this + 4) + *this * 4 + (local_c & 0xfffffff));
    local_18 = FUN_004fce20(&local_c);
    local_18 = local_18 & 0xff;
    switch(local_18) {
    default:
      *param_1 = 0;
      param_1[1] = 0;
      param_1[2] = 0;
      param_1[3] = 0;
      FUN_004f4440((undefined1 *)param_1);
      break;
    case 1:
      uVar2 = FUN_004ec950((undefined4 *)local_10);
      FUN_004f4250(param_1,uVar2);
      break;
    case 2:
      uVar2 = FUN_004ec950((undefined4 *)local_10);
      FUN_004f4270(param_1,uVar2);
      break;
    case 3:
      uVar5 = FUN_004ec9f0(local_10);
      FUN_004f43c0(param_1,(int)uVar5,(int)((ulonglong)uVar5 >> 0x20));
      break;
    case 4:
      uVar5 = FUN_004ec9f0(local_10);
      FUN_004f43f0(param_1,(int)uVar5,(int)((ulonglong)uVar5 >> 0x20));
      break;
    case 5:
      uVar1 = FUN_004ec930((undefined1 *)local_10);
      FUN_004f4420(param_1,uVar1);
      break;
    case 6:
      uVar1 = FUN_004ec930((undefined1 *)local_10);
      FUN_004f4230(param_1,uVar1);
      break;
    case 7:
      fVar4 = (float10)FUN_004ec970((float *)local_10);
      FUN_004f4290(param_1,(float)fVar4);
      break;
    case 8:
      fVar4 = FUN_004ec990(local_10);
      FUN_004f42c0(param_1,(double)fVar4);
      break;
    case 9:
      fVar4 = FUN_004ec990(local_10);
      FUN_004f42f0(param_1,(double)fVar4);
      break;
    case 10:
      uVar2 = FUN_004ec950((undefined4 *)local_10);
      FUN_004f4340(param_1,uVar2);
      break;
    case 0xb:
      uVar2 = FUN_004ec950((undefined4 *)local_10);
      FUN_004f4320(param_1,uVar2);
      break;
    case 0xc:
      puVar3 = FUN_004ec9c0(local_20,(undefined4 *)local_10);
      FUN_004f4360(param_1,*puVar3,puVar3[1]);
      break;
    case 0xd:
      puVar3 = FUN_004ec9c0(local_28,(undefined4 *)local_10);
      FUN_004f4390(param_1,*puVar3,puVar3[1]);
    }
  }
  else {
    *param_1 = 0;
    param_1[1] = 0;
    param_1[2] = 0;
    param_1[3] = 0;
    FUN_004f4440((undefined1 *)param_1);
  }
  FUN_005429ee(local_8 ^ (uint)&stack0xfffffffc);
  return;
}

