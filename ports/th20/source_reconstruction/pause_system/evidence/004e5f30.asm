004e5f30 55                       push       ebp
004e5f31 8bec                     mov        ebp, esp
004e5f33 83ec0c                   sub        esp, 0xc
004e5f36 894dfc                   mov        dword ptr [ebp - 4], ecx
004e5f39 8b0dfc605c00             mov        ecx, dword ptr [0x5c60fc]
004e5f3f e84c080000               call       0x4e6790
004e5f44 85c0                     test       eax, eax
004e5f46 7405                     je         0x4e5f4d
004e5f48 e946010000               jmp        0x4e6093
004e5f4d 6a01                     push       1
004e5f4f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5f52 e8390b0000               call       0x4e6a90
004e5f57 6a01                     push       1
004e5f59 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5f5c e83f0a0000               call       0x4e69a0
004e5f61 a128a85b00               mov        eax, dword ptr [0x5ba828]
004e5f66 8b88e8000000             mov        ecx, dword ptr [eax + 0xe8]
004e5f6c 83c910                   or         ecx, 0x10
004e5f6f 8b1528a85b00             mov        edx, dword ptr [0x5ba828]
004e5f75 898ae8000000             mov        dword ptr [edx + 0xe8], ecx
004e5f7b 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5f7e e83d010000               call       0x4e60c0
004e5f83 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e5f89 e8c222fdff               call       0x4b8250
004e5f8e 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5f91 8981d4020000             mov        dword ptr [ecx + 0x2d4], eax
004e5f97 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5f9a 81c1c8000000             add        ecx, 0xc8
004e5fa0 e82b9df6ff               call       0x44fcd0
004e5fa5 90                       nop        
004e5fa6 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e5fa9 8b82d4020000             mov        eax, dword ptr [edx + 0x2d4]
004e5faf 8945f8                   mov        dword ptr [ebp - 8], eax
004e5fb2 6a00                     push       0
004e5fb4 6892000000               push       0x92
004e5fb9 6838065700               push       0x570638
004e5fbe 8d4df4                   lea        ecx, [ebp - 0xc]
004e5fc1 51                       push       ecx
004e5fc2 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004e5fc5 e8b680fbff               call       0x49e080
004e5fca 8b10                     mov        edx, dword ptr [eax]
004e5fcc 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e5fcf 8990c8000000             mov        dword ptr [eax + 0xc8], edx
004e5fd5 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e5fd8 81c1c8000000             add        ecx, 0xc8
004e5fde e86d03faff               call       0x486350
004e5fe3 b930a85b00               mov        ecx, 0x5ba830
004e5fe8 e85344fdff               call       0x4ba440
004e5fed b930a85b00               mov        ecx, 0x5ba830
004e5ff2 e8a9000000               call       0x4e60a0
004e5ff7 b9e4ef5a00               mov        ecx, 0x5aefe4
004e5ffc e8df32f4ff               call       0x4292e0
004e6001 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e6004 d999c4010000             fstp       dword ptr [ecx + 0x1c4]
004e600a 51                       push       ecx
004e600b f30f1005ccc85600         movss      xmm0, dword ptr [0x56c8cc]
004e6013 f30f110424               movss      dword ptr [esp], xmm0
004e6018 b9e4ef5a00               mov        ecx, 0x5aefe4
004e601d e87e32f4ff               call       0x4292a0
004e6022 8b55fc                   mov        edx, dword ptr [ebp - 4]
004e6025 a158885b00               mov        eax, dword ptr [0x5b8858]
004e602a 8982ec000000             mov        dword ptr [edx + 0xec], eax
004e6030 c70558885b0000000000     mov        dword ptr [0x5b8858], 0
004e603a 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e6040 e81b21f9ff               call       0x478160
004e6045 85c0                     test       eax, eax
004e6047 7413                     je         0x4e605c
004e6049 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e604f e81c20fdff               call       0x4b8070
004e6054 8bc8                     mov        ecx, eax
004e6056 e8d52afdff               call       0x4b8b30
004e605b 90                       nop        
004e605c 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e6062 e8a927fdff               call       0x4b8810
004e6067 8b0da4065c00             mov        ecx, dword ptr [0x5c06a4]
004e606d e8be0a0000               call       0x4e6b30
004e6072 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004e6075 8b91d0020000             mov        edx, dword ptr [ecx + 0x2d0]
004e607b 83e2fb                   and        edx, 0xfffffffb
004e607e 8b45fc                   mov        eax, dword ptr [ebp - 4]
004e6081 8990d0020000             mov        dword ptr [eax + 0x2d0], edx
004e6087 8b0dfc605c00             mov        ecx, dword ptr [0x5c60fc]
004e608d e83e090000               call       0x4e69d0
004e6092 90                       nop        
004e6093 8be5                     mov        esp, ebp
004e6095 5d                       pop        ebp
004e6096 c3                       ret        
004e6097 cc                       int3       
004e6098 cc                       int3       
004e6099 cc                       int3       
004e609a cc                       int3       
004e609b cc                       int3       
004e609c cc                       int3       
004e609d cc                       int3       
004e609e cc                       int3       
004e609f cc                       int3       
