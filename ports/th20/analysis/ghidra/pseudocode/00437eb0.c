/* Entry: 0x00437eb0; symbol: FUN_00437eb0; body bytes: 835 */

float10 __thiscall FUN_00437eb0(int param_1,float param_2)

{
  int iVar1;
  float10 fVar2;
  
  switch((int)param_2) {
  case 10000:
    fVar2 = (float10)*(int *)(param_1 + 0x444);
    break;
  case 0x2711:
    fVar2 = (float10)*(int *)(param_1 + 0x448);
    break;
  case 0x2712:
    fVar2 = (float10)*(int *)(param_1 + 0x44c);
    break;
  case 0x2713:
    fVar2 = (float10)*(int *)(param_1 + 0x450);
    break;
  case 0x2714:
    fVar2 = (float10)*(float *)(param_1 + 0x454);
    break;
  case 0x2715:
    fVar2 = (float10)*(float *)(param_1 + 0x458);
    break;
  case 0x2716:
    fVar2 = (float10)*(float *)(param_1 + 0x45c);
    break;
  case 0x2717:
    fVar2 = (float10)*(float *)(param_1 + 0x460);
    break;
  case 0x2718:
    fVar2 = (float10)*(int *)(param_1 + 0x470);
    break;
  case 0x2719:
    fVar2 = (float10)*(int *)(param_1 + 0x474);
    break;
  case 0x271a:
    fVar2 = FUN_004298b0(0x5ba4c4,*(float *)(param_1 + 0x47c));
    break;
  case 0x271b:
    fVar2 = FUN_00429800(0x5ba4c4,*(float *)(param_1 + 0x478));
    break;
  case 0x271c:
    fVar2 = FUN_004298b0(0x5ba4c4,*(float *)(param_1 + 0x478));
    break;
  case 0x271d:
    fVar2 = (float10)*(float *)(param_1 + 0x2c);
    break;
  case 0x271e:
    fVar2 = (float10)*(float *)(param_1 + 0x30);
    break;
  case 0x271f:
    fVar2 = (float10)*(float *)(param_1 + 0x34);
    break;
  case 0x2720:
    fVar2 = (float10)(DAT_005c53fc + DAT_005c5438);
    break;
  case 0x2721:
    fVar2 = (float10)(DAT_005c5400 + DAT_005c543c);
    break;
  case 0x2722:
    fVar2 = (float10)(DAT_005c5404 + DAT_005c5440);
    break;
  case 0x2723:
    fVar2 = (float10)DAT_005c5420;
    break;
  case 0x2724:
    fVar2 = (float10)DAT_005c5424;
    break;
  case 0x2725:
    fVar2 = (float10)DAT_005c5428;
    break;
  case 0x2726:
    iVar1 = FUN_00423ee0(0x5ba4c4);
    fVar2 = (float10)(float)((double)iVar1 + *(double *)(&DAT_0056cdd0 + (iVar1 >> 0x1f) * -8));
    break;
  case 0x2727:
    fVar2 = (float10)*(float *)(param_1 + 0x38);
    break;
  case 0x2728:
    fVar2 = (float10)*(float *)(param_1 + 0x3c);
    break;
  case 0x2729:
    fVar2 = (float10)*(float *)(param_1 + 0x40);
    break;
  case 0x272a:
    iVar1 = FUN_00437770(param_1);
    fVar2 = (float10)*(float *)(iVar1 + 8);
    break;
  case 0x272b:
    fVar2 = (float10)*(float *)(param_1 + 0x478);
    break;
  case 0x272c:
    fVar2 = (float10)*(float *)(param_1 + 0x47c);
    break;
  case 0x272d:
    fVar2 = (float10)*(int *)(param_1 + 0x480);
    break;
  case 0x272e:
    fVar2 = FUN_004298b0(0x5ba4c4,*(float *)(param_1 + 0x47c));
    break;
  case 0x272f:
    fVar2 = FUN_00429800(0x5ba4c4,*(float *)(param_1 + 0x478));
    break;
  case 0x2730:
    fVar2 = FUN_004298b0(0x5ba4c4,*(float *)(param_1 + 0x478));
    break;
  case 0x2731:
    fVar2 = (float10)*(float *)(param_1 + 0x464);
    break;
  case 0x2732:
    fVar2 = (float10)*(float *)(param_1 + 0x468);
    break;
  case 0x2733:
    fVar2 = (float10)*(float *)(param_1 + 0x46c);
    break;
  default:
    fVar2 = (float10)param_2;
  }
  return fVar2;
}

