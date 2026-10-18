00447c60 55                       push       ebp
00447c61 8bec                     mov        ebp, esp
00447c63 6aff                     push       -1
00447c65 6800765600               push       0x567600
00447c6a 64a100000000             mov        eax, dword ptr fs:[0]
00447c70 50                       push       eax
00447c71 51                       push       ecx
00447c72 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
00447c77 33c5                     xor        eax, ebp
00447c79 50                       push       eax
00447c7a 8d45f4                   lea        eax, [ebp - 0xc]
00447c7d 64a300000000             mov        dword ptr fs:[0], eax
00447c83 894df0                   mov        dword ptr [ebp - 0x10], ecx
00447c86 68f0924400               push       0x4492f0
00447c8b 68f0894400               push       0x4489f0
00447c90 6800000100               push       0x10000
00447c95 6800060000               push       0x600
00447c9a 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00447c9d 50                       push       eax
00447c9e e80cb20f00               call       0x542eaf
00447ca3 90                       nop        
00447ca4 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
00447ca7 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00447caa 64890d00000000           mov        dword ptr fs:[0], ecx
00447cb1 59                       pop        ecx
00447cb2 8be5                     mov        esp, ebp
00447cb4 5d                       pop        ebp
00447cb5 c3                       ret        
00447cb6 cc                       int3       
00447cb7 cc                       int3       
00447cb8 cc                       int3       
00447cb9 cc                       int3       
00447cba cc                       int3       
00447cbb cc                       int3       
00447cbc cc                       int3       
00447cbd cc                       int3       
00447cbe cc                       int3       
00447cbf cc                       int3       
