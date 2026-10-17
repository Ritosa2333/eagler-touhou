0052db90 55                       push       ebp
0052db91 8bec                     mov        ebp, esp
0052db93 83ec10                   sub        esp, 0x10
0052db96 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0052db9b 33c5                     xor        eax, ebp
0052db9d 8945fc                   mov        dword ptr [ebp - 4], eax
0052dba0 894df4                   mov        dword ptr [ebp - 0xc], ecx
0052dba3 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052dba6 e8c5120000               call       0x52ee70
0052dbab 6a04                     push       4
0052dbad 8d4df8                   lea        ecx, [ebp - 8]
0052dbb0 e8cbe4edff               call       0x40c080
0052dbb5 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052dbb8 e8a315eeff               call       0x40f160
0052dbbd 50                       push       eax
0052dbbe 8d4df8                   lea        ecx, [ebp - 8]
0052dbc1 e8aadaedff               call       0x40b670
0052dbc6 90                       nop        
0052dbc7 c745f000000000           mov        dword ptr [ebp - 0x10], 0
0052dbce 8d45f0                   lea        eax, [ebp - 0x10]
0052dbd1 50                       push       eax
0052dbd2 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0052dbd5 e816e4fcff               call       0x4fbff0
0052dbda 50                       push       eax
0052dbdb e8f0d8edff               call       0x40b4d0
0052dbe0 83c408                   add        esp, 8
0052dbe3 50                       push       eax
0052dbe4 8d4df8                   lea        ecx, [ebp - 8]
0052dbe7 51                       push       ecx
0052dbe8 e863f6ffff               call       0x52d250
0052dbed 83c408                   add        esp, 8
0052dbf0 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0052dbf3 33cd                     xor        ecx, ebp
0052dbf5 e8f44d0100               call       0x5429ee
0052dbfa 8be5                     mov        esp, ebp
0052dbfc 5d                       pop        ebp
0052dbfd c3                       ret        
0052dbfe cc                       int3       
0052dbff cc                       int3       
