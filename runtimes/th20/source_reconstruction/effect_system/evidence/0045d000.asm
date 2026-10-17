0045d000 55                       push       ebp
0045d001 8bec                     mov        ebp, esp
0045d003 51                       push       ecx
0045d004 8b4508                   mov        eax, dword ptr [ebp + 8]
0045d007 50                       push       eax
0045d008 6830ec5600               push       0x56ec30
0045d00d 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
0045d013 e878f0ffff               call       0x45c090
0045d018 8945fc                   mov        dword ptr [ebp - 4], eax
0045d01b 8b4d10                   mov        ecx, dword ptr [ebp + 0x10]
0045d01e 51                       push       ecx
0045d01f 8b550c                   mov        edx, dword ptr [ebp + 0xc]
0045d022 52                       push       edx
0045d023 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045d026 e8b5ffffff               call       0x45cfe0
0045d02b 8be5                     mov        esp, ebp
0045d02d 5d                       pop        ebp
0045d02e c3                       ret        
0045d02f cc                       int3       
