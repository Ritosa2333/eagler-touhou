/* Entry: 0x00437310; symbol: FUN_00437310; body bytes: 244 */

float * __thiscall FUN_00437310(void *this,float *param_1,ushort param_2,byte param_3)

{
  if (((uint)param_2 & 1 << (param_3 & 0x1f)) != 0) {
    switch((int)*param_1) {
    case 0x2714:
      param_1 = (float *)((int)this + 0x454);
      break;
    case 0x2715:
      param_1 = (float *)((int)this + 0x458);
      break;
    case 0x2716:
      param_1 = (float *)((int)this + 0x45c);
      break;
    case 0x2717:
      param_1 = (float *)((int)this + 0x460);
      break;
    default:
      break;
    case 0x271d:
      param_1 = (float *)((int)this + 0x2c);
      break;
    case 0x271e:
      param_1 = (float *)((int)this + 0x30);
      break;
    case 0x271f:
      param_1 = (float *)((int)this + 0x34);
      break;
    case 0x2727:
      param_1 = (float *)((int)this + 0x38);
      break;
    case 0x2728:
      param_1 = (float *)((int)this + 0x3c);
      break;
    case 0x2729:
      param_1 = (float *)((int)this + 0x40);
      break;
    case 0x272b:
      param_1 = (float *)((int)this + 0x478);
      break;
    case 0x272c:
      param_1 = (float *)((int)this + 0x47c);
      break;
    case 0x2731:
      param_1 = (float *)((int)this + 0x464);
      break;
    case 0x2732:
      param_1 = (float *)((int)this + 0x468);
      break;
    case 0x2733:
      param_1 = (float *)((int)this + 0x46c);
    }
  }
  return param_1;
}

