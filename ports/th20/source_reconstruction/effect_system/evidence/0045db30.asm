0045db30 55                       push       ebp
0045db31 8bec                     mov        ebp, esp
0045db33 83ec1c                   sub        esp, 0x1c
0045db36 894dfc                   mov        dword ptr [ebp - 4], ecx
0045db39 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045db3c 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
0045db3f 8988dc030000             mov        dword ptr [eax + 0x3dc], ecx
0045db45 8b55fc                   mov        edx, dword ptr [ebp - 4]
0045db48 8b4508                   mov        eax, dword ptr [ebp + 8]
0045db4b 8b4824                   mov        ecx, dword ptr [eax + 0x24]
0045db4e 898ac8030000             mov        dword ptr [edx + 0x3c8], ecx
0045db54 6a00                     push       0
0045db56 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045db59 81c1cc030000             add        ecx, 0x3cc
0045db5f e8bc59fcff               call       0x423520
0045db64 8b55fc                   mov        edx, dword ptr [ebp - 4]
0045db67 83c208                   add        edx, 8
0045db6a 8955f8                   mov        dword ptr [ebp - 8], edx
0045db6d 6a00                     push       0
0045db6f 8b4df8                   mov        ecx, dword ptr [ebp - 8]
0045db72 e819fbffff               call       0x45d690
0045db77 8945f4                   mov        dword ptr [ebp - 0xc], eax
0045db7a 680000ff00               push       0xff0000
0045db7f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
0045db82 e869000000               call       0x45dbf0
0045db87 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045db8a 8b4804                   mov        ecx, dword ptr [eax + 4]
0045db8d 894df0                   mov        dword ptr [ebp - 0x10], ecx
0045db90 8b5508                   mov        edx, dword ptr [ebp + 8]
0045db93 52                       push       edx
0045db94 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
0045db97 e8c4010000               call       0x45dd60
0045db9c 8b45fc                   mov        eax, dword ptr [ebp - 4]
0045db9f 8b4804                   mov        ecx, dword ptr [eax + 4]
0045dba2 894dec                   mov        dword ptr [ebp - 0x14], ecx
0045dba5 6a0b                     push       0xb
0045dba7 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
0045dbaa e8a127ffff               call       0x450350
0045dbaf 8b55fc                   mov        edx, dword ptr [ebp - 4]
0045dbb2 8b4204                   mov        eax, dword ptr [edx + 4]
0045dbb5 8945e8                   mov        dword ptr [ebp - 0x18], eax
0045dbb8 6a00                     push       0
0045dbba 68ff000000               push       0xff
0045dbbf 6a00                     push       0
0045dbc1 6a1e                     push       0x1e
0045dbc3 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
0045dbc6 e895b0fdff               call       0x438c60
0045dbcb 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
0045dbce 8b5104                   mov        edx, dword ptr [ecx + 4]
0045dbd1 8955e4                   mov        dword ptr [ebp - 0x1c], edx
0045dbd4 6a01                     push       1
0045dbd6 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
0045dbd9 e862010000               call       0x45dd40
0045dbde 33c0                     xor        eax, eax
0045dbe0 8be5                     mov        esp, ebp
0045dbe2 5d                       pop        ebp
0045dbe3 c20800                   ret        8
0045dbe6 cc                       int3       
0045dbe7 cc                       int3       
0045dbe8 cc                       int3       
0045dbe9 cc                       int3       
0045dbea cc                       int3       
0045dbeb cc                       int3       
0045dbec cc                       int3       
0045dbed cc                       int3       
0045dbee cc                       int3       
0045dbef cc                       int3       
