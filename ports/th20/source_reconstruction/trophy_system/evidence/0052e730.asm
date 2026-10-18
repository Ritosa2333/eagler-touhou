0052e730 55                       push       ebp
0052e731 8bec                     mov        ebp, esp
0052e733 a158685c00               mov        eax, dword ptr [0x5c6858]
0052e738 50                       push       eax
0052e739 e88230efff               call       0x4217c0
0052e73e 83c404                   add        esp, 4
0052e741 6a14                     push       0x14
0052e743 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
0052e749 e8e2dcf1ff               call       0x44c430
0052e74e 8b0d5c685c00             mov        ecx, dword ptr [0x5c685c]
0052e754 51                       push       ecx
0052e755 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0052e75b e8e0f0edff               call       0x40d840
0052e760 c7055c685c0000000000     mov        dword ptr [0x5c685c], 0
0052e76a 33c0                     xor        eax, eax
0052e76c 5d                       pop        ebp
0052e76d c3                       ret        
0052e76e cc                       int3       
0052e76f cc                       int3       
