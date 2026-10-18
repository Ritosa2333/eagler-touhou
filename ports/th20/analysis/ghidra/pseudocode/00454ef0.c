/* Entry: 0x00454ef0; symbol: FUN_00454ef0; body bytes: 4154 */

/* WARNING: Globals starting with '_' overlap smaller symbols at the same address */

float10 __cdecl FUN_00454ef0(void *param_1,float param_2,float param_3)

{
  float10 fVar1;
  float local_2c;
  float local_28;
  float local_24;
  float local_20;
  float local_1c;
  float local_18;
  float local_14;
  float local_10;
  
  if (param_3 == _DAT_0056e0e8) {
    fVar1 = (float10)1;
  }
  else {
    local_10 = param_2 / param_3;
    switch(param_1) {
    default:
      fVar1 = (float10)local_10;
      break;
    case (void *)0x1:
      fVar1 = (float10)(local_10 * local_10);
      break;
    case (void *)0x2:
      fVar1 = (float10)(local_10 * local_10 * local_10);
      break;
    case (void *)0x3:
      fVar1 = (float10)(local_10 * local_10 * local_10 * local_10);
      break;
    case (void *)0x4:
      fVar1 = (float10)(DAT_0056c8cc - (DAT_0056c8cc - local_10) * (DAT_0056c8cc - local_10));
      break;
    case (void *)0x5:
      fVar1 = (float10)(DAT_0056c8cc -
                       (DAT_0056c8cc - local_10) * (DAT_0056c8cc - local_10) *
                       (DAT_0056c8cc - local_10));
      break;
    case (void *)0x6:
      fVar1 = (float10)(DAT_0056c8cc -
                       (DAT_0056c8cc - local_10) * (DAT_0056c8cc - local_10) *
                       (DAT_0056c8cc - local_10) * (DAT_0056c8cc - local_10));
      break;
    case (void *)0x9:
      local_10 = local_10 * DAT_0056c8d0;
      if (DAT_0056c8cc <= local_10) {
        local_10 = DAT_0056c8d0 - (DAT_0056c8d0 - local_10) * (DAT_0056c8d0 - local_10);
      }
      else {
        local_10 = local_10 * local_10;
      }
      local_10 = local_10 / DAT_0056c8d0;
      fVar1 = (float10)local_10;
      break;
    case (void *)0xa:
      local_10 = local_10 * DAT_0056c8d0;
      if (DAT_0056c8cc <= local_10) {
        local_10 = DAT_0056c8d0 -
                   (DAT_0056c8d0 - local_10) * (DAT_0056c8d0 - local_10) * (DAT_0056c8d0 - local_10)
        ;
      }
      else {
        local_10 = local_10 * local_10 * local_10;
      }
      local_18 = local_10 / DAT_0056c8d0;
      fVar1 = (float10)local_18;
      break;
    case (void *)0xb:
      local_10 = local_10 * DAT_0056c8d0;
      if (DAT_0056c8cc <= local_10) {
        local_10 = DAT_0056c8d0 -
                   (DAT_0056c8d0 - local_10) * (DAT_0056c8d0 - local_10) * (DAT_0056c8d0 - local_10)
                   * (DAT_0056c8d0 - local_10);
      }
      else {
        local_10 = local_10 * local_10 * local_10 * local_10;
      }
      local_20 = local_10 / DAT_0056c8d0;
      fVar1 = (float10)local_20;
      break;
    case (void *)0xc:
      local_10 = local_10 * DAT_0056c8d0;
      if (DAT_0056c8cc <= local_10) {
        local_14 = ((local_10 - DAT_0056c8cc) * (local_10 - DAT_0056c8cc)) / DAT_0056c8d0 +
                   DAT_0056e0ec;
      }
      else {
        local_14 = DAT_0056e0ec -
                   ((DAT_0056c8cc - local_10) * (DAT_0056c8cc - local_10)) / DAT_0056c8d0;
      }
      fVar1 = (float10)local_14;
      break;
    case (void *)0xd:
      local_10 = local_10 * DAT_0056c8d0;
      if (DAT_0056c8cc <= local_10) {
        local_1c = ((local_10 - DAT_0056c8cc) * (local_10 - DAT_0056c8cc) *
                   (local_10 - DAT_0056c8cc)) / DAT_0056c8d0 + DAT_0056e0ec;
      }
      else {
        local_1c = DAT_0056e0ec -
                   ((DAT_0056c8cc - local_10) * (DAT_0056c8cc - local_10) *
                   (DAT_0056c8cc - local_10)) / DAT_0056c8d0;
      }
      fVar1 = (float10)local_1c;
      break;
    case (void *)0xe:
      local_10 = local_10 * DAT_0056c8d0;
      if (DAT_0056c8cc <= local_10) {
        local_24 = ((local_10 - DAT_0056c8cc) * (local_10 - DAT_0056c8cc) *
                    (local_10 - DAT_0056c8cc) * (local_10 - DAT_0056c8cc)) / DAT_0056c8d0 +
                   DAT_0056e0ec;
      }
      else {
        local_24 = DAT_0056e0ec -
                   ((DAT_0056c8cc - local_10) * (DAT_0056c8cc - local_10) *
                    (DAT_0056c8cc - local_10) * (DAT_0056c8cc - local_10)) / DAT_0056c8d0;
      }
      fVar1 = (float10)local_24;
      break;
    case (void *)0xf:
      fVar1 = (float10)0;
      break;
    case (void *)0x10:
      fVar1 = (float10)1;
      break;
    case (void *)0x12:
      fVar1 = FUN_00439820(param_1,(local_10 * DAT_0056e0f0) / DAT_0056c8d0);
      break;
    case (void *)0x13:
      fVar1 = FUN_00439820(param_1,(local_10 * DAT_0056e0f0) / DAT_0056c8d0 +
                                   DAT_0056e0f0 / DAT_0056c8d0);
      fVar1 = (float10)(DAT_0056c8cc - (float)fVar1);
      break;
    case (void *)0x14:
      local_10 = local_10 * DAT_0056c8d0;
      if (DAT_0056c8cc <= local_10) {
        fVar1 = FUN_00439820(param_1,(local_10 * DAT_0056e0f0) / DAT_0056c8d0);
        local_28 = (DAT_0056c8cc - (float)fVar1) / DAT_0056c8d0 + DAT_0056e0ec;
      }
      else {
        fVar1 = FUN_00439820(param_1,(local_10 * DAT_0056e0f0) / DAT_0056c8d0);
        local_28 = (float)fVar1 / DAT_0056c8d0;
      }
      fVar1 = (float10)local_28;
      break;
    case (void *)0x15:
      local_10 = local_10 * DAT_0056c8d0;
      if (DAT_0056c8cc <= local_10) {
        fVar1 = FUN_00439820(param_1,((local_10 - DAT_0056c8cc) * DAT_0056e0f0) / DAT_0056c8d0);
        local_2c = (float)fVar1 / DAT_0056c8d0 + DAT_0056e0ec;
      }
      else {
        fVar1 = FUN_00439820(param_1,(local_10 * DAT_0056e0f0) / DAT_0056c8d0 +
                                     DAT_0056e0f0 / DAT_0056c8d0);
        local_2c = (DAT_0056c8cc - (float)fVar1) / DAT_0056c8d0;
      }
      fVar1 = (float10)local_2c;
      break;
    case (void *)0x16:
      fVar1 = (float10)((((local_10 - DAT_0056e718) * (local_10 - DAT_0056e718)) /
                         ((DAT_0056c8cc - DAT_0056e718) * (DAT_0056c8cc - DAT_0056e718)) -
                        (DAT_0056e718 * DAT_0056e718) /
                        ((DAT_0056c8cc - DAT_0056e718) * (DAT_0056c8cc - DAT_0056e718))) /
                       (DAT_0056c8cc -
                       (DAT_0056e718 * DAT_0056e718) /
                       ((DAT_0056c8cc - DAT_0056e718) * (DAT_0056c8cc - DAT_0056e718))));
      break;
    case (void *)0x17:
      fVar1 = (float10)((((local_10 - DAT_0056e71c) * (local_10 - DAT_0056e71c)) /
                         ((DAT_0056c8cc - DAT_0056e71c) * (DAT_0056c8cc - DAT_0056e71c)) -
                        (DAT_0056e71c * DAT_0056e71c) /
                        ((DAT_0056c8cc - DAT_0056e71c) * (DAT_0056c8cc - DAT_0056e71c))) /
                       (DAT_0056c8cc -
                       (DAT_0056e71c * DAT_0056e71c) /
                       ((DAT_0056c8cc - DAT_0056e71c) * (DAT_0056c8cc - DAT_0056e71c))));
      break;
    case (void *)0x18:
      fVar1 = (float10)((((local_10 - DAT_0056e720) * (local_10 - DAT_0056e720)) /
                         ((DAT_0056c8cc - DAT_0056e720) * (DAT_0056c8cc - DAT_0056e720)) -
                        (DAT_0056e720 * DAT_0056e720) /
                        ((DAT_0056c8cc - DAT_0056e720) * (DAT_0056c8cc - DAT_0056e720))) /
                       (DAT_0056c8cc -
                       (DAT_0056e720 * DAT_0056e720) /
                       ((DAT_0056c8cc - DAT_0056e720) * (DAT_0056c8cc - DAT_0056e720))));
      break;
    case (void *)0x19:
      fVar1 = (float10)((((local_10 - DAT_0056e724) * (local_10 - DAT_0056e724)) /
                         ((DAT_0056c8cc - DAT_0056e724) * (DAT_0056c8cc - DAT_0056e724)) -
                        (DAT_0056e724 * DAT_0056e724) /
                        ((DAT_0056c8cc - DAT_0056e724) * (DAT_0056c8cc - DAT_0056e724))) /
                       (DAT_0056c8cc -
                       (DAT_0056e724 * DAT_0056e724) /
                       ((DAT_0056c8cc - DAT_0056e724) * (DAT_0056c8cc - DAT_0056e724))));
      break;
    case (void *)0x1a:
      fVar1 = (float10)((((local_10 - DAT_0056e728) * (local_10 - DAT_0056e728)) /
                         ((DAT_0056c8cc - DAT_0056e728) * (DAT_0056c8cc - DAT_0056e728)) -
                        (DAT_0056e728 * DAT_0056e728) /
                        ((DAT_0056c8cc - DAT_0056e728) * (DAT_0056c8cc - DAT_0056e728))) /
                       (DAT_0056c8cc -
                       (DAT_0056e728 * DAT_0056e728) /
                       ((DAT_0056c8cc - DAT_0056e728) * (DAT_0056c8cc - DAT_0056e728))));
      break;
    case (void *)0x1b:
      fVar1 = (float10)(DAT_0056c8cc -
                       ((((DAT_0056c8cc - local_10) - DAT_0056e718) *
                        ((DAT_0056c8cc - local_10) - DAT_0056e718)) /
                        ((DAT_0056c8cc - DAT_0056e718) * (DAT_0056c8cc - DAT_0056e718)) -
                       (DAT_0056e718 * DAT_0056e718) /
                       ((DAT_0056c8cc - DAT_0056e718) * (DAT_0056c8cc - DAT_0056e718))) /
                       (DAT_0056c8cc -
                       (DAT_0056e718 * DAT_0056e718) /
                       ((DAT_0056c8cc - DAT_0056e718) * (DAT_0056c8cc - DAT_0056e718))));
      break;
    case (void *)0x1c:
      fVar1 = (float10)(DAT_0056c8cc -
                       ((((DAT_0056c8cc - local_10) - DAT_0056e71c) *
                        ((DAT_0056c8cc - local_10) - DAT_0056e71c)) /
                        ((DAT_0056c8cc - DAT_0056e71c) * (DAT_0056c8cc - DAT_0056e71c)) -
                       (DAT_0056e71c * DAT_0056e71c) /
                       ((DAT_0056c8cc - DAT_0056e71c) * (DAT_0056c8cc - DAT_0056e71c))) /
                       (DAT_0056c8cc -
                       (DAT_0056e71c * DAT_0056e71c) /
                       ((DAT_0056c8cc - DAT_0056e71c) * (DAT_0056c8cc - DAT_0056e71c))));
      break;
    case (void *)0x1d:
      fVar1 = (float10)(DAT_0056c8cc -
                       ((((DAT_0056c8cc - local_10) - DAT_0056e720) *
                        ((DAT_0056c8cc - local_10) - DAT_0056e720)) /
                        ((DAT_0056c8cc - DAT_0056e720) * (DAT_0056c8cc - DAT_0056e720)) -
                       (DAT_0056e720 * DAT_0056e720) /
                       ((DAT_0056c8cc - DAT_0056e720) * (DAT_0056c8cc - DAT_0056e720))) /
                       (DAT_0056c8cc -
                       (DAT_0056e720 * DAT_0056e720) /
                       ((DAT_0056c8cc - DAT_0056e720) * (DAT_0056c8cc - DAT_0056e720))));
      break;
    case (void *)0x1e:
      fVar1 = (float10)(DAT_0056c8cc -
                       ((((DAT_0056c8cc - local_10) - DAT_0056e724) *
                        ((DAT_0056c8cc - local_10) - DAT_0056e724)) /
                        ((DAT_0056c8cc - DAT_0056e724) * (DAT_0056c8cc - DAT_0056e724)) -
                       (DAT_0056e724 * DAT_0056e724) /
                       ((DAT_0056c8cc - DAT_0056e724) * (DAT_0056c8cc - DAT_0056e724))) /
                       (DAT_0056c8cc -
                       (DAT_0056e724 * DAT_0056e724) /
                       ((DAT_0056c8cc - DAT_0056e724) * (DAT_0056c8cc - DAT_0056e724))));
      break;
    case (void *)0x1f:
      fVar1 = (float10)(DAT_0056c8cc -
                       ((((DAT_0056c8cc - local_10) - DAT_0056e728) *
                        ((DAT_0056c8cc - local_10) - DAT_0056e728)) /
                        ((DAT_0056c8cc - DAT_0056e728) * (DAT_0056c8cc - DAT_0056e728)) -
                       (DAT_0056e728 * DAT_0056e728) /
                       ((DAT_0056c8cc - DAT_0056e728) * (DAT_0056c8cc - DAT_0056e728))) /
                       (DAT_0056c8cc -
                       (DAT_0056e728 * DAT_0056e728) /
                       ((DAT_0056c8cc - DAT_0056e728) * (DAT_0056c8cc - DAT_0056e728))));
    }
  }
  return fVar1;
}

