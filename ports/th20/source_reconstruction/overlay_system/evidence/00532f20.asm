00532f20 55                       push       ebp
00532f21 8bec                     mov        ebp, esp
00532f23 51                       push       ecx
00532f24 894dfc                   mov        dword ptr [ebp - 4], ecx
00532f27 8b45fc                   mov        eax, dword ptr [ebp - 4]
00532f2a c6403400                 mov        byte ptr [eax + 0x34], 0
00532f2e 33c0                     xor        eax, eax
00532f30 8be5                     mov        esp, ebp
00532f32 5d                       pop        ebp
00532f33 c3                       ret        
00532f34 cc                       int3       
00532f35 cc                       int3       
00532f36 cc                       int3       
00532f37 cc                       int3       
00532f38 cc                       int3       
00532f39 cc                       int3       
00532f3a cc                       int3       
00532f3b cc                       int3       
00532f3c cc                       int3       
00532f3d cc                       int3       
00532f3e cc                       int3       
00532f3f cc                       int3       
