0047c800 55                       push       ebp
0047c801 8bec                     mov        ebp, esp
0047c803 83ec30                   sub        esp, 0x30
0047c806 894dfc                   mov        dword ptr [ebp - 4], ecx
0047c809 8b45fc                   mov        eax, dword ptr [ebp - 4]
0047c80c 05a0000000               add        eax, 0xa0
0047c811 8945f8                   mov        dword ptr [ebp - 8], eax
0047c814 6a09                     push       9
0047c816 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0047c819 e862feffff               call       0x47c680
0047c81e 8945ec                   mov        dword ptr [ebp - 0x14], eax
0047c821 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c824 81c1a0000000             add        ecx, 0xa0
0047c82a 894df4                   mov        dword ptr [ebp - 0xc], ecx
0047c82d 6a09                     push       9
0047c82f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0047c832 e849feffff               call       0x47c680
0047c837 8b5030                   mov        edx, dword ptr [eax + 0x30]
0047c83a 8955f0                   mov        dword ptr [ebp - 0x10], edx
0047c83d 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
0047c840 50                       push       eax
0047c841 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0047c844 e8776dfaff               call       0x4235c0
0047c849 0fb6c8                   movzx      ecx, al
0047c84c 85c9                     test       ecx, ecx
0047c84e 7420                     je         0x47c870
0047c850 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047c853 8b8290000000             mov        eax, dword ptr [edx + 0x90]
0047c859 25fffff7ff               and        eax, 0xfff7ffff
0047c85e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c861 898190000000             mov        dword ptr [ecx + 0x90], eax
0047c867 b801000000               mov        eax, 1
0047c86c eb70                     jmp        0x47c8de
0047c86e eb4d                     jmp        0x47c8bd
0047c870 8b55fc                   mov        edx, dword ptr [ebp - 4]
0047c873 81c2a0000000             add        edx, 0xa0
0047c879 8955e8                   mov        dword ptr [ebp - 0x18], edx
0047c87c 6a09                     push       9
0047c87e 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0047c881 e8fafdffff               call       0x47c680
0047c886 83c024                   add        eax, 0x24
0047c889 8945e0                   mov        dword ptr [ebp - 0x20], eax
0047c88c b9e4ef5a00               mov        ecx, 0x5aefe4
0047c891 e84acafaff               call       0x4292e0
0047c896 d95de4                   fstp       dword ptr [ebp - 0x1c]
0047c899 51                       push       ecx
0047c89a f30f1045e4               movss      xmm0, dword ptr [ebp - 0x1c]
0047c89f f30f110424               movss      dword ptr [esp], xmm0
0047c8a4 8d45d0                   lea        eax, [ebp - 0x30]
0047c8a7 50                       push       eax
0047c8a8 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
0047c8ab e840cafaff               call       0x4292f0
0047c8b0 50                       push       eax
0047c8b1 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c8b4 83c164                   add        ecx, 0x64
0047c8b7 e824cefaff               call       0x4296e0
0047c8bc 90                       nop        
0047c8bd 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0047c8c0 81c1a0000000             add        ecx, 0xa0
0047c8c6 894ddc                   mov        dword ptr [ebp - 0x24], ecx
0047c8c9 6a00                     push       0
0047c8cb 6a09                     push       9
0047c8cd 8b4ddc                   mov        ecx, dword ptr [ebp - 0x24]
0047c8d0 e8abfdffff               call       0x47c680
0047c8d5 8bc8                     mov        ecx, eax
0047c8d7 e8446cfaff               call       0x423520
0047c8dc 33c0                     xor        eax, eax
0047c8de 8be5                     mov        esp, ebp
0047c8e0 5d                       pop        ebp
0047c8e1 c3                       ret        
0047c8e2 cc                       int3       
0047c8e3 cc                       int3       
0047c8e4 cc                       int3       
0047c8e5 cc                       int3       
0047c8e6 cc                       int3       
0047c8e7 cc                       int3       
0047c8e8 cc                       int3       
0047c8e9 cc                       int3       
0047c8ea cc                       int3       
0047c8eb cc                       int3       
0047c8ec cc                       int3       
0047c8ed cc                       int3       
0047c8ee cc                       int3       
0047c8ef cc                       int3       
