/* Entry: 0x00437220; symbol: FUN_00437220; body bytes: 165 */

undefined4 * __thiscall FUN_00437220(void *this,undefined4 *param_1,ushort param_2,byte param_3)

{
  if (((uint)param_2 & 1 << (param_3 & 0x1f)) != 0) {
    switch(*param_1) {
    case 10000:
      param_1 = (undefined4 *)((int)this + 0x444);
      break;
    case 0x2711:
      param_1 = (undefined4 *)((int)this + 0x448);
      break;
    case 0x2712:
      param_1 = (undefined4 *)((int)this + 0x44c);
      break;
    case 0x2713:
      param_1 = (undefined4 *)((int)this + 0x450);
      break;
    default:
      break;
    case 0x2718:
      param_1 = (undefined4 *)((int)this + 0x470);
      break;
    case 0x2719:
      param_1 = (undefined4 *)((int)this + 0x474);
      break;
    case 0x272d:
      param_1 = (undefined4 *)((int)this + 0x480);
    }
  }
  return param_1;
}

