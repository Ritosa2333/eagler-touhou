00511270 55                       push       ebp
00511271 8bec                     mov        ebp, esp
00511273 83ec08                   sub        esp, 8
00511276 894dfc                   mov        dword ptr [ebp - 4], ecx
00511279 6858065700               push       0x570658
0051127e 6a05                     push       5
00511280 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00511286 e8a5d8f3ff               call       0x44eb30
0051128b 8945f8                   mov        dword ptr [ebp - 8], eax
0051128e 8b45fc                   mov        eax, dword ptr [ebp - 4]
00511291 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00511294 894810                   mov        dword ptr [eax + 0x10], ecx
00511297 837df800                 cmp        dword ptr [ebp - 8], 0
0051129b 7505                     jne        0x5112a2
0051129d 83c8ff                   or         eax, 0xffffffff
005112a0 eb54                     jmp        0x5112f6
005112a2 8b55fc                   mov        edx, dword ptr [ebp - 4]
005112a5 52                       push       edx
005112a6 6800135100               push       0x511300
005112ab 6a2d                     push       0x2d
005112ad e80e10f0ff               call       0x4122c0
005112b2 83c40c                   add        esp, 0xc
005112b5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005112b8 894108                   mov        dword ptr [ecx + 8], eax
005112bb 8b55fc                   mov        edx, dword ptr [ebp - 4]
005112be 52                       push       edx
005112bf 6810135100               push       0x511310
005112c4 6a56                     push       0x56
005112c6 e89510f0ff               call       0x412360
005112cb 83c40c                   add        esp, 0xc
005112ce 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005112d1 89410c                   mov        dword ptr [ecx + 0xc], eax
005112d4 8b55fc                   mov        edx, dword ptr [ebp - 4]
005112d7 c7422401000000           mov        dword ptr [edx + 0x24], 1
005112de 6a00                     push       0
005112e0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
005112e3 83c12c                   add        ecx, 0x2c
005112e6 e83522f1ff               call       0x423520
005112eb 8b45fc                   mov        eax, dword ptr [ebp - 4]
005112ee 8b4d08                   mov        ecx, dword ptr [ebp + 8]
005112f1 894820                   mov        dword ptr [eax + 0x20], ecx
005112f4 33c0                     xor        eax, eax
005112f6 8be5                     mov        esp, ebp
005112f8 5d                       pop        ebp
005112f9 c20400                   ret        4
005112fc cc                       int3       
005112fd cc                       int3       
005112fe cc                       int3       
005112ff cc                       int3       
