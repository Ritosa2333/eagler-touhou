005372c0 55                       push       ebp
005372c1 8bec                     mov        ebp, esp
005372c3 83ec34                   sub        esp, 0x34
005372c6 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
005372cb 33c5                     xor        eax, ebp
005372cd 8945fc                   mov        dword ptr [ebp - 4], eax
005372d0 894de4                   mov        dword ptr [ebp - 0x1c], ecx
005372d3 8b45e4                   mov        eax, dword ptr [ebp - 0x1c]
005372d6 c6403500                 mov        byte ptr [eax + 0x35], 0
005372da 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
005372dd 83c138                   add        ecx, 0x38
005372e0 e84b5df2ff               call       0x45d030
005372e5 85c0                     test       eax, eax
005372e7 0f84fd000000             je         0x5373ea
005372ed c745e800000000           mov        dword ptr [ebp - 0x18], 0
005372f4 eb09                     jmp        0x5372ff
005372f6 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
005372f9 83c101                   add        ecx, 1
005372fc 894de8                   mov        dword ptr [ebp - 0x18], ecx
005372ff 6a00                     push       0
00537301 e87acdf2ff               call       0x464080
00537306 83c404                   add        esp, 4
00537309 8bc8                     mov        ecx, eax
0053730b e8900ef8ff               call       0x4b81a0
00537310 3945e8                   cmp        dword ptr [ebp - 0x18], eax
00537313 0f8dd1000000             jge        0x5373ea
00537319 6a00                     push       0
0053731b e860cdf2ff               call       0x464080
00537320 83c404                   add        esp, 4
00537323 8bc8                     mov        ecx, eax
00537325 e8d6d0f2ff               call       0x464400
0053732a 0fb6d0                   movzx      edx, al
0053732d 85d2                     test       edx, edx
0053732f 7409                     je         0x53733a
00537331 c745e016000000           mov        dword ptr [ebp - 0x20], 0x16
00537338 eb07                     jmp        0x537341
0053733a c745e010000000           mov        dword ptr [ebp - 0x20], 0x10
00537341 8b45e0                   mov        eax, dword ptr [ebp - 0x20]
00537344 8945dc                   mov        dword ptr [ebp - 0x24], eax
00537347 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0053734a 83c138                   add        ecx, 0x38
0053734d e83e8cedff               call       0x40ff90
00537352 99                       cdq        
00537353 f77ddc                   idiv       dword ptr [ebp - 0x24]
00537356 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
00537359 c1e102                   shl        ecx, 2
0053735c 3bd1                     cmp        edx, ecx
0053735e 0f8581000000             jne        0x5373e5
00537364 6a00                     push       0
00537366 e8c594f2ff               call       0x460830
0053736b 83c404                   add        esp, 4
0053736e 8945d8                   mov        dword ptr [ebp - 0x28], eax
00537371 8b55e8                   mov        edx, dword ptr [ebp - 0x18]
00537374 52                       push       edx
00537375 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
00537378 e843e4ffff               call       0x5357c0
0053737d 8bc8                     mov        ecx, eax
0053737f e8eca5eeff               call       0x421970
00537384 8b08                     mov        ecx, dword ptr [eax]
00537386 894dec                   mov        dword ptr [ebp - 0x14], ecx
00537389 8b5004                   mov        edx, dword ptr [eax + 4]
0053738c 8955f0                   mov        dword ptr [ebp - 0x10], edx
0053738f 8b4008                   mov        eax, dword ptr [eax + 8]
00537392 8945f4                   mov        dword ptr [ebp - 0xc], eax
00537395 6a04                     push       4
00537397 8d4df8                   lea        ecx, [ebp - 8]
0053739a e8e14cedff               call       0x40c080
0053739f 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
005373a2 51                       push       ecx
005373a3 8d4df8                   lea        ecx, [ebp - 8]
005373a6 e8f5ceedff               call       0x4142a0
005373ab 90                       nop        
005373ac 6a00                     push       0
005373ae e8cd1af4ff               call       0x478e80
005373b3 83c404                   add        esp, 4
005373b6 8945d4                   mov        dword ptr [ebp - 0x2c], eax
005373b9 83ec28                   sub        esp, 0x28
005373bc 8bcc                     mov        ecx, esp
005373be 8965cc                   mov        dword ptr [ebp - 0x34], esp
005373c1 8d55f8                   lea        edx, [ebp - 8]
005373c4 52                       push       edx
005373c5 e846fbffff               call       0x536f10
005373ca 51                       push       ecx
005373cb f30f1005d8c85600         movss      xmm0, dword ptr [0x56c8d8]
005373d3 f30f110424               movss      dword ptr [esp], xmm0
005373d8 8d45ec                   lea        eax, [ebp - 0x14]
005373db 50                       push       eax
005373dc 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
005373df e85c5ef4ff               call       0x47d240
005373e4 90                       nop        
005373e5 e90cffffff               jmp        0x5372f6
005373ea 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
005373ed 83c138                   add        ecx, 0x38
005373f0 894dd0                   mov        dword ptr [ebp - 0x30], ecx
005373f3 6a00                     push       0
005373f5 8b4dd0                   mov        ecx, dword ptr [ebp - 0x30]
005373f8 e843c1eeff               call       0x423540
005373fd 90                       nop        
005373fe 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00537401 33cd                     xor        ecx, ebp
00537403 e8e6b50000               call       0x5429ee
00537408 8be5                     mov        esp, ebp
0053740a 5d                       pop        ebp
0053740b c3                       ret        
0053740c cc                       int3       
0053740d cc                       int3       
0053740e cc                       int3       
0053740f cc                       int3       
