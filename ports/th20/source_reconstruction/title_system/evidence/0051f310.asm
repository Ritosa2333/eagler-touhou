0051f310 55                       push       ebp
0051f311 8bec                     mov        ebp, esp
0051f313 83ec14                   sub        esp, 0x14
0051f316 6800010000               push       0x100
0051f31b 6a00                     push       0
0051f31d 8b4508                   mov        eax, dword ptr [ebp + 8]
0051f320 50                       push       eax
0051f321 e83a540200               call       0x544760
0051f326 83c40c                   add        esp, 0xc
0051f329 0fb60dc0675b00           movzx      ecx, byte ptr [0x5b67c0]
0051f330 85c9                     test       ecx, ecx
0051f332 747e                     je         0x51f3b2
0051f334 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
0051f33a e861cf0000               call       0x52c2a0
0051f33f 8945fc                   mov        dword ptr [ebp - 4], eax
0051f342 8b55fc                   mov        edx, dword ptr [ebp - 4]
0051f345 8b02                     mov        eax, dword ptr [edx]
0051f347 8b4824                   mov        ecx, dword ptr [eax + 0x24]
0051f34a 894dec                   mov        dword ptr [ebp - 0x14], ecx
0051f34d 8b5508                   mov        edx, dword ptr [ebp + 8]
0051f350 52                       push       edx
0051f351 6800010000               push       0x100
0051f356 8b45fc                   mov        eax, dword ptr [ebp - 4]
0051f359 50                       push       eax
0051f35a ff55ec                   call       dword ptr [ebp - 0x14]
0051f35d 8945f4                   mov        dword ptr [ebp - 0xc], eax
0051f360 817df41e000780           cmp        dword ptr [ebp - 0xc], 0x8007001e
0051f367 751f                     jne        0x51f388
0051f369 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
0051f36f e82ccf0000               call       0x52c2a0
0051f374 8945f8                   mov        dword ptr [ebp - 8], eax
0051f377 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0051f37a 8b11                     mov        edx, dword ptr [ecx]
0051f37c 8b45f8                   mov        eax, dword ptr [ebp - 8]
0051f37f 50                       push       eax
0051f380 8b4a1c                   mov        ecx, dword ptr [edx + 0x1c]
0051f383 ffd1                     call       ecx
0051f385 90                       nop        
0051f386 eb23                     jmp        0x51f3ab
0051f388 837df400                 cmp        dword ptr [ebp - 0xc], 0
0051f38c 741d                     je         0x51f3ab
0051f38e 8b0d98885b00             mov        ecx, dword ptr [0x5b8898]
0051f394 e807cf0000               call       0x52c2a0
0051f399 8945f0                   mov        dword ptr [ebp - 0x10], eax
0051f39c 8b55f0                   mov        edx, dword ptr [ebp - 0x10]
0051f39f 8b02                     mov        eax, dword ptr [edx]
0051f3a1 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0051f3a4 51                       push       ecx
0051f3a5 8b501c                   mov        edx, dword ptr [eax + 0x1c]
0051f3a8 ffd2                     call       edx
0051f3aa 90                       nop        
0051f3ab b801000000               mov        eax, 1
0051f3b0 eb02                     jmp        0x51f3b4
0051f3b2 33c0                     xor        eax, eax
0051f3b4 8be5                     mov        esp, ebp
0051f3b6 5d                       pop        ebp
0051f3b7 c3                       ret        
0051f3b8 cc                       int3       
0051f3b9 cc                       int3       
0051f3ba cc                       int3       
0051f3bb cc                       int3       
0051f3bc cc                       int3       
0051f3bd cc                       int3       
0051f3be cc                       int3       
0051f3bf cc                       int3       
