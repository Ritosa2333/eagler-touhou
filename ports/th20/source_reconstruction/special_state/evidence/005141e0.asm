005141e0 55                       push       ebp
005141e1 8bec                     mov        ebp, esp
005141e3 51                       push       ecx
005141e4 894dfc                   mov        dword ptr [ebp - 4], ecx
005141e7 8b4508                   mov        eax, dword ptr [ebp + 8]
005141ea 50                       push       eax
005141eb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005141ee e89d9cf0ff               call       0x41de90
005141f3 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
005141f6 51                       push       ecx
005141f7 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005141fa e8c1ffffff               call       0x5141c0
005141ff 8b5510                   mov        edx, dword ptr [ebp + 0x10]
00514202 52                       push       edx
00514203 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00514206 e845ffffff               call       0x514150
0051420b 8b4514                   mov        eax, dword ptr [ebp + 0x14]
0051420e 50                       push       eax
0051420f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00514212 e8f9feffff               call       0x514110
00514217 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0051421a 8b5510                   mov        edx, dword ptr [ebp + 0x10]
0051421d 0fb602                   movzx      eax, byte ptr [edx]
00514220 884104                   mov        byte ptr [ecx + 4], al
00514223 6a00                     push       0
00514225 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00514228 83c108                   add        ecx, 8
0051422b e8f0f2f0ff               call       0x423520
00514230 90                       nop        
00514231 8be5                     mov        esp, ebp
00514233 5d                       pop        ebp
00514234 c21000                   ret        0x10
00514237 cc                       int3       
00514238 cc                       int3       
00514239 cc                       int3       
0051423a cc                       int3       
0051423b cc                       int3       
0051423c cc                       int3       
0051423d cc                       int3       
0051423e cc                       int3       
0051423f cc                       int3       
