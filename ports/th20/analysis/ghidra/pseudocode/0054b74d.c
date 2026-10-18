/* Entry: 0x0054b74d; symbol: parse_floating_point_write_result<double>; body bytes: 249 */

/* Library Function - Single Match
    enum SLD_STATUS __cdecl __crt_strtox::parse_floating_point_write_result<double>(enum
   __crt_strtox::floating_point_parse_result,struct __crt_strtox::floating_point_string const
   &,double * const)
   
   Library: Visual Studio 2019 Release */

SLD_STATUS __cdecl
__crt_strtox::parse_floating_point_write_result<double>
          (floating_point_parse_result param_1,floating_point_string *param_2,double *param_3)

{
  void *in_ECX;
  uint uVar1;
  SLD_STATUS SVar2;
  
  switch(param_1) {
  case 0:
    SVar2 = FID_conflict_convert_hexadecimal_string_to_floating_type<double>
                      (in_ECX,(uint *)param_2,param_3);
    return SVar2;
  case 1:
    SVar2 = FID_conflict_convert_hexadecimal_string_to_floating_type<double>
                      (in_ECX,(int *)param_2,param_3);
    return SVar2;
  case 2:
    uVar1 = (uint)(param_2[0x308] != (floating_point_string)0x0) << 0x1f;
    break;
  case 3:
    uVar1 = (uint)(param_2[0x308] != (floating_point_string)0x0) << 0x1f | 0x7ff00000;
    break;
  case 4:
    uVar1 = (uint)(param_2[0x308] != (floating_point_string)0x0) << 0x1f | 0x7fffffff;
    *(undefined4 *)param_3 = 0xffffffff;
    goto LAB_0054b798;
  case 5:
    assemble_floating_point_snan<double>((bool)param_2[0x308],param_3);
    return 0;
  case 6:
    *(undefined4 *)param_3 = 0;
    *(undefined4 *)((int)param_3 + 4) = 0xfff80000;
    return 0;
  case 7:
    *(undefined4 *)param_3 = 0;
    *(undefined4 *)((int)param_3 + 4) = 0;
    return 1;
  case 8:
    SVar2 = 2;
    uVar1 = (uint)(param_2[0x308] != (floating_point_string)0x0) << 0x1f;
    goto LAB_0054b81f;
  case 9:
    SVar2 = 3;
    uVar1 = (uint)(param_2[0x308] != (floating_point_string)0x0) << 0x1f | 0x7ff00000;
LAB_0054b81f:
    *(undefined4 *)param_3 = 0;
    *(uint *)((int)param_3 + 4) = uVar1;
    return SVar2;
  default:
    return 1;
  }
  *(undefined4 *)param_3 = 0;
LAB_0054b798:
  *(uint *)((int)param_3 + 4) = uVar1;
  return 0;
}

