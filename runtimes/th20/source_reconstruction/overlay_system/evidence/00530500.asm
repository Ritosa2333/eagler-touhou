00530500 55                       push       ebp
00530501 8bec                     mov        ebp, esp
00530503 51                       push       ecx
00530504 894dfc                   mov        dword ptr [ebp - 4], ecx
00530507 8b45fc                   mov        eax, dword ptr [ebp - 4]
0053050a 0fb64034                 movzx      eax, byte ptr [eax + 0x34]
0053050e 8be5                     mov        esp, ebp
00530510 5d                       pop        ebp
00530511 c3                       ret        
00530512 cc                       int3       
00530513 cc                       int3       
00530514 cc                       int3       
00530515 cc                       int3       
00530516 cc                       int3       
00530517 cc                       int3       
00530518 cc                       int3       
00530519 cc                       int3       
0053051a cc                       int3       
0053051b cc                       int3       
0053051c cc                       int3       
0053051d cc                       int3       
0053051e cc                       int3       
0053051f cc                       int3       
