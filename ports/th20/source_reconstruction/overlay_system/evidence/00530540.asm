00530540 55                       push       ebp
00530541 8bec                     mov        ebp, esp
00530543 51                       push       ecx
00530544 894dfc                   mov        dword ptr [ebp - 4], ecx
00530547 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053054a 0fb64035                 movzx      eax, byte ptr [eax + 0x35]
0053054e 8be5                     mov        esp, ebp
00530550 5d                       pop        ebp
00530551 c3                       ret        
00530552 cc                       int3       
00530553 cc                       int3       
00530554 cc                       int3       
00530555 cc                       int3       
00530556 cc                       int3       
00530557 cc                       int3       
00530558 cc                       int3       
00530559 cc                       int3       
0053055a cc                       int3       
0053055b cc                       int3       
0053055c cc                       int3       
0053055d cc                       int3       
0053055e cc                       int3       
0053055f cc                       int3       
