004792f0 55                       push       ebp
004792f1 8bec                     mov        ebp, esp
004792f3 51                       push       ecx
004792f4 894dfc                   mov        dword ptr [ebp - 4], ecx
004792f7 68d0914700               push       0x4791d0
004792fc 6a18                     push       0x18
004792fe 68d8000000               push       0xd8
00479303 8b45fc                   mov        eax, dword ptr [ebp - 4]
00479306 50                       push       eax
00479307 e81429f9ff               call       0x40bc20
0047930c 90                       nop        
0047930d 8b45fc                   mov        eax, dword ptr [ebp - 4]
00479310 8be5                     mov        esp, ebp
00479312 5d                       pop        ebp
00479313 c3                       ret        
00479314 cc                       int3       
00479315 cc                       int3       
00479316 cc                       int3       
00479317 cc                       int3       
00479318 cc                       int3       
00479319 cc                       int3       
0047931a cc                       int3       
0047931b cc                       int3       
0047931c cc                       int3       
0047931d cc                       int3       
0047931e cc                       int3       
0047931f cc                       int3       
