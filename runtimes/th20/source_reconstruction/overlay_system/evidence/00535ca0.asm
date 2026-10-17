00535ca0 55                       push       ebp
00535ca1 8bec                     mov        ebp, esp
00535ca3 83ec0c                   sub        esp, 0xc
00535ca6 894dfc                   mov        dword ptr [ebp - 4], ecx
00535ca9 6a00                     push       0
00535cab e880abf2ff               call       0x460830
00535cb0 83c404                   add        esp, 4
00535cb3 8bc8                     mov        ecx, eax
00535cb5 e8f699fcff               call       0x4ff6b0
00535cba 8945f4                   mov        dword ptr [ebp - 0xc], eax
00535cbd 8b45fc                   mov        eax, dword ptr [ebp - 4]
00535cc0 8b08                     mov        ecx, dword ptr [eax]
00535cc2 6b510414                 imul       edx, dword ptr [ecx + 4], 0x14
00535cc6 83c20f                   add        edx, 0xf
00535cc9 8955f8                   mov        dword ptr [ebp - 8], edx
00535ccc 8b4508                   mov        eax, dword ptr [ebp + 8]
00535ccf 50                       push       eax
00535cd0 6a00                     push       0
00535cd2 6a00                     push       0
00535cd4 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00535cd7 51                       push       ecx
00535cd8 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00535cdb e86001fdff               call       0x505e40
00535ce0 8b55fc                   mov        edx, dword ptr [ebp - 4]
00535ce3 8b02                     mov        eax, dword ptr [edx]
00535ce5 c6403501                 mov        byte ptr [eax + 0x35], 1
00535ce9 b801000000               mov        eax, 1
00535cee 8be5                     mov        esp, ebp
00535cf0 5d                       pop        ebp
00535cf1 c20400                   ret        4
00535cf4 cc                       int3       
00535cf5 cc                       int3       
00535cf6 cc                       int3       
00535cf7 cc                       int3       
00535cf8 cc                       int3       
00535cf9 cc                       int3       
00535cfa cc                       int3       
00535cfb cc                       int3       
00535cfc cc                       int3       
00535cfd cc                       int3       
00535cfe cc                       int3       
00535cff cc                       int3       
