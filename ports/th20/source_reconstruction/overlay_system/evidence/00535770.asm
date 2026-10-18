00535770 55                       push       ebp
00535771 8bec                     mov        ebp, esp
00535773 51                       push       ecx
00535774 894dfc                   mov        dword ptr [ebp - 4], ecx
00535777 f30f104508               movss      xmm0, dword ptr [ebp + 8]
0053577c 0f5705d0d75600           xorps      xmm0, xmmword ptr [0x56d7d0]
00535783 51                       push       ecx
00535784 f30f110424               movss      dword ptr [esp], xmm0
00535789 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0053578c e85fd9f1ff               call       0x4530f0
00535791 90                       nop        
00535792 8be5                     mov        esp, ebp
00535794 5d                       pop        ebp
00535795 c20400                   ret        4
00535798 cc                       int3       
00535799 cc                       int3       
0053579a cc                       int3       
0053579b cc                       int3       
0053579c cc                       int3       
0053579d cc                       int3       
0053579e cc                       int3       
0053579f cc                       int3       
