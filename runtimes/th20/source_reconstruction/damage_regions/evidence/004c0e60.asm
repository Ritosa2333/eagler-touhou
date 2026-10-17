004c0e60 55                       push       ebp
004c0e61 8bec                     mov        ebp, esp
004c0e63 83ec1c                   sub        esp, 0x1c
004c0e66 894dfc                   mov        dword ptr [ebp - 4], ecx
004c0e69 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0e6c 81c12cc40000             add        ecx, 0xc42c
004c0e72 e8794bfcff               call       0x4859f0
004c0e77 8945f4                   mov        dword ptr [ebp - 0xc], eax
004c0e7a 837df400                 cmp        dword ptr [ebp - 0xc], 0
004c0e7e 744f                     je         0x4c0ecf
004c0e80 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c0e83 e8580ef5ff               call       0x411ce0
004c0e88 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c0e8b 0514c40000               add        eax, 0xc414
004c0e90 8945ec                   mov        dword ptr [ebp - 0x14], eax
004c0e93 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c0e96 51                       push       ecx
004c0e97 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c0e9a e80110f5ff               call       0x411ea0
004c0e9f 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c0ea2 e859b4f4ff               call       0x40c300
004c0ea7 8945f0                   mov        dword ptr [ebp - 0x10], eax
004c0eaa 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c0ead 8b8210c40000             mov        eax, dword ptr [edx + 0xc410]
004c0eb3 50                       push       eax
004c0eb4 8b4df0                   mov        ecx, dword ptr [ebp - 0x10]
004c0eb7 81c18c000000             add        ecx, 0x8c
004c0ebd e8de08f5ff               call       0x4117a0
004c0ec2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0ec5 e896fdffff               call       0x4c0c60
004c0eca 8b45f0                   mov        eax, dword ptr [ebp - 0x10]
004c0ecd eb6e                     jmp        0x4c0f3d
004c0ecf 68000d5700               push       0x570d00
004c0ed4 8b0d94885b00             mov        ecx, dword ptr [0x5b8894]
004c0eda e851efffff               call       0x4bfe30
004c0edf 8945f8                   mov        dword ptr [ebp - 8], eax
004c0ee2 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c0ee5 894de8                   mov        dword ptr [ebp - 0x18], ecx
004c0ee8 8b55f8                   mov        edx, dword ptr [ebp - 8]
004c0eeb 52                       push       edx
004c0eec 8b4de8                   mov        ecx, dword ptr [ebp - 0x18]
004c0eef e87c97f8ff               call       0x44a670
004c0ef4 8b45fc                   mov        eax, dword ptr [ebp - 4]
004c0ef7 0514c40000               add        eax, 0xc414
004c0efc 8945e4                   mov        dword ptr [ebp - 0x1c], eax
004c0eff 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c0f02 51                       push       ecx
004c0f03 8b4de4                   mov        ecx, dword ptr [ebp - 0x1c]
004c0f06 e8950ff5ff               call       0x411ea0
004c0f0b 8b55fc                   mov        edx, dword ptr [ebp - 4]
004c0f0e 8b8210c40000             mov        eax, dword ptr [edx + 0xc410]
004c0f14 0d00000001               or         eax, 0x1000000
004c0f19 50                       push       eax
004c0f1a 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c0f1d 81c18c000000             add        ecx, 0x8c
004c0f23 e87808f5ff               call       0x4117a0
004c0f28 6a00                     push       0
004c0f2a 8b4df8                   mov        ecx, dword ptr [ebp - 8]
004c0f2d e8ee7afcff               call       0x488a20
004c0f32 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004c0f35 e826fdffff               call       0x4c0c60
004c0f3a 8b45f8                   mov        eax, dword ptr [ebp - 8]
004c0f3d 8be5                     mov        esp, ebp
004c0f3f 5d                       pop        ebp
004c0f40 c3                       ret        
004c0f41 cc                       int3       
004c0f42 cc                       int3       
004c0f43 cc                       int3       
004c0f44 cc                       int3       
004c0f45 cc                       int3       
004c0f46 cc                       int3       
004c0f47 cc                       int3       
004c0f48 cc                       int3       
004c0f49 cc                       int3       
004c0f4a cc                       int3       
004c0f4b cc                       int3       
004c0f4c cc                       int3       
004c0f4d cc                       int3       
004c0f4e cc                       int3       
004c0f4f cc                       int3       
