004c38d0 53                       push       ebx
004c38d1 8bdc                     mov        ebx, esp
004c38d3 83ec08                   sub        esp, 8
004c38d6 83e4f8                   and        esp, 0xfffffff8
004c38d9 83c404                   add        esp, 4
004c38dc 55                       push       ebp
004c38dd 8b6b04                   mov        ebp, dword ptr [ebx + 4]
004c38e0 896c2404                 mov        dword ptr [esp + 4], ebp
004c38e4 8bec                     mov        ebp, esp
004c38e6 6aff                     push       -1
004c38e8 688d9a5600               push       0x569a8d
004c38ed 64a100000000             mov        eax, dword ptr fs:[0]
004c38f3 50                       push       eax
004c38f4 53                       push       ebx
004c38f5 83ec60                   sub        esp, 0x60
004c38f8 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
004c38fd 33c5                     xor        eax, ebp
004c38ff 8945ec                   mov        dword ptr [ebp - 0x14], eax
004c3902 50                       push       eax
004c3903 8d45f4                   lea        eax, [ebp - 0xc]
004c3906 64a300000000             mov        dword ptr fs:[0], eax
004c390c 894dd8                   mov        dword ptr [ebp - 0x28], ecx
004c390f 8b45d8                   mov        eax, dword ptr [ebp - 0x28]
004c3912 8b8894c84900             mov        ecx, dword ptr [eax + 0x49c894]
004c3918 894dcc                   mov        dword ptr [ebp - 0x34], ecx
004c391b 837b0801                 cmp        dword ptr [ebx + 8], 1
004c391f 7509                     jne        0x4c392a
004c3921 c745dc0c000000           mov        dword ptr [ebp - 0x24], 0xc
004c3928 eb07                     jmp        0x4c3931
004c392a c745dc06000000           mov        dword ptr [ebp - 0x24], 6
004c3931 8b55cc                   mov        edx, dword ptr [ebp - 0x34]
004c3934 52                       push       edx
004c3935 8b45dc                   mov        eax, dword ptr [ebp - 0x24]
004c3938 50                       push       eax
004c3939 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
004c393f e88cbaf8ff               call       0x44f3d0
004c3944 90                       nop        
004c3945 8b4dd8                   mov        ecx, dword ptr [ebp - 0x28]
004c3948 81c118c84900             add        ecx, 0x49c818
004c394e 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
004c3951 6a08                     push       8
004c3953 8d4de4                   lea        ecx, [ebp - 0x1c]
004c3956 e82587f4ff               call       0x40c080
004c395b 8d55e4                   lea        edx, [ebp - 0x1c]
004c395e 52                       push       edx
004c395f 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004c3962 e819e9f4ff               call       0x412280
004c3967 c745fc00000000           mov        dword ptr [ebp - 4], 0
004c396e 8b4dd4                   mov        ecx, dword ptr [ebp - 0x2c]
004c3971 e8caebf4ff               call       0x412540
004c3976 8945c8                   mov        dword ptr [ebp - 0x38], eax
004c3979 eb09                     jmp        0x4c3984
004c397b 8d4de4                   lea        ecx, [ebp - 0x1c]
004c397e e8ade2f4ff               call       0x411c30
004c3983 90                       nop        
004c3984 8b45c8                   mov        eax, dword ptr [ebp - 0x38]
004c3987 50                       push       eax
004c3988 8d4de4                   lea        ecx, [ebp - 0x1c]
004c398b e820e2f4ff               call       0x411bb0
004c3990 0fb6c8                   movzx      ecx, al
004c3993 85c9                     test       ecx, ecx
004c3995 0f8431020000             je         0x4c3bcc
004c399b 8d4de4                   lea        ecx, [ebp - 0x1c]
004c399e e85d89f4ff               call       0x40c300
004c39a3 8945e0                   mov        dword ptr [ebp - 0x20], eax
004c39a6 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c39a9 e8b2b7f4ff               call       0x40f160
004c39ae 8b10                     mov        edx, dword ptr [eax]
004c39b0 83ba240c000000           cmp        dword ptr [edx + 0xc24], 0
004c39b7 7502                     jne        0x4c39bb
004c39b9 ebc0                     jmp        0x4c397b
004c39bb 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c39be e89db7f4ff               call       0x40f160
004c39c3 8b08                     mov        ecx, dword ptr [eax]
004c39c5 83c118                   add        ecx, 0x18
004c39c8 e8e320f8ff               call       0x445ab0
004c39cd 85c0                     test       eax, eax
004c39cf 7502                     jne        0x4c39d3
004c39d1 eba8                     jmp        0x4c397b
004c39d3 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c39d6 e885b7f4ff               call       0x40f160
004c39db 8b00                     mov        eax, dword ptr [eax]
004c39dd 83b8340c000000           cmp        dword ptr [eax + 0xc34], 0
004c39e4 7e02                     jle        0x4c39e8
004c39e6 eb93                     jmp        0x4c397b
004c39e8 837b0800                 cmp        dword ptr [ebx + 8], 0
004c39ec 7502                     jne        0x4c39f0
004c39ee eb8b                     jmp        0x4c397b
004c39f0 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c39f3 e868b7f4ff               call       0x40f160
004c39f8 8b08                     mov        ecx, dword ptr [eax]
004c39fa 81c1e40b0000             add        ecx, 0xbe4
004c3a00 8b11                     mov        edx, dword ptr [ecx]
004c3a02 8955a0                   mov        dword ptr [ebp - 0x60], edx
004c3a05 8b4104                   mov        eax, dword ptr [ecx + 4]
004c3a08 8945a4                   mov        dword ptr [ebp - 0x5c], eax
004c3a0b 8b4908                   mov        ecx, dword ptr [ecx + 8]
004c3a0e 894da8                   mov        dword ptr [ebp - 0x58], ecx
004c3a11 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c3a14 e847b7f4ff               call       0x40f160
004c3a19 8b10                     mov        edx, dword ptr [eax]
004c3a1b 81c2d4050000             add        edx, 0x5d4
004c3a21 8b45a0                   mov        eax, dword ptr [ebp - 0x60]
004c3a24 8902                     mov        dword ptr [edx], eax
004c3a26 8b4da4                   mov        ecx, dword ptr [ebp - 0x5c]
004c3a29 894a04                   mov        dword ptr [edx + 4], ecx
004c3a2c 8b45a8                   mov        eax, dword ptr [ebp - 0x58]
004c3a2f 894208                   mov        dword ptr [edx + 8], eax
004c3a32 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c3a35 e826b7f4ff               call       0x40f160
004c3a3a 8b08                     mov        ecx, dword ptr [eax]
004c3a3c 81c1e40b0000             add        ecx, 0xbe4
004c3a42 8b11                     mov        edx, dword ptr [ecx]
004c3a44 895590                   mov        dword ptr [ebp - 0x70], edx
004c3a47 8b4104                   mov        eax, dword ptr [ecx + 4]
004c3a4a 894594                   mov        dword ptr [ebp - 0x6c], eax
004c3a4d 8b4908                   mov        ecx, dword ptr [ecx + 8]
004c3a50 894d98                   mov        dword ptr [ebp - 0x68], ecx
004c3a53 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c3a56 e805b7f4ff               call       0x40f160
004c3a5b 8b10                     mov        edx, dword ptr [eax]
004c3a5d 81c2b80b0000             add        edx, 0xbb8
004c3a63 8b4590                   mov        eax, dword ptr [ebp - 0x70]
004c3a66 8902                     mov        dword ptr [edx], eax
004c3a68 8b4d94                   mov        ecx, dword ptr [ebp - 0x6c]
004c3a6b 894a04                   mov        dword ptr [edx + 4], ecx
004c3a6e 8b4598                   mov        eax, dword ptr [ebp - 0x68]
004c3a71 894208                   mov        dword ptr [edx + 8], eax
004c3a74 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c3a77 e8e4b6f4ff               call       0x40f160
004c3a7c 8b08                     mov        ecx, dword ptr [eax]
004c3a7e 83c118                   add        ecx, 0x18
004c3a81 e8ea100000               call       0x4c4b70
004c3a86 d95dc4                   fstp       dword ptr [ebp - 0x3c]
004c3a89 f30f1005b40d5700         movss      xmm0, dword ptr [0x570db4]
004c3a91 0f2f45c4                 comiss     xmm0, dword ptr [ebp - 0x3c]
004c3a95 0f8606010000             jbe        0x4c3ba1
004c3a9b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c3a9e e8bdb6f4ff               call       0x40f160
004c3aa3 8b08                     mov        ecx, dword ptr [eax]
004c3aa5 81c1fc050000             add        ecx, 0x5fc
004c3aab e80020f8ff               call       0x445ab0
004c3ab0 85c0                     test       eax, eax
004c3ab2 0f84d3000000             je         0x4c3b8b
004c3ab8 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c3abb e8a0b6f4ff               call       0x40f160
004c3ac0 8b08                     mov        ecx, dword ptr [eax]
004c3ac2 81c1fc050000             add        ecx, 0x5fc
004c3ac8 e8a3100000               call       0x4c4b70
004c3acd d95dc0                   fstp       dword ptr [ebp - 0x40]
004c3ad0 f30f1045c0               movss      xmm0, dword ptr [ebp - 0x40]
004c3ad5 f30f5805bcd75600         addss      xmm0, dword ptr [0x56d7bc]
004c3add f30f1145d0               movss      dword ptr [ebp - 0x30], xmm0
004c3ae2 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c3ae5 e876b6f4ff               call       0x40f160
004c3aea 8b08                     mov        ecx, dword ptr [eax]
004c3aec 81c1fc050000             add        ecx, 0x5fc
004c3af2 894dbc                   mov        dword ptr [ebp - 0x44], ecx
004c3af5 51                       push       ecx
004c3af6 f30f1005bcd75600         movss      xmm0, dword ptr [0x56d7bc]
004c3afe f30f110424               movss      dword ptr [esp], xmm0
004c3b03 8b4dbc                   mov        ecx, dword ptr [ebp - 0x44]
004c3b06 e835d0faff               call       0x470b40
004c3b0b 90                       nop        
004c3b0c f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
004c3b11 0f2f0594cd5600           comiss     xmm0, dword ptr [0x56cd94]
004c3b18 7223                     jb         0x4c3b3d
004c3b1a 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c3b1d e83eb6f4ff               call       0x40f160
004c3b22 8b10                     mov        edx, dword ptr [eax]
004c3b24 81c2fc050000             add        edx, 0x5fc
004c3b2a 8955b8                   mov        dword ptr [ebp - 0x48], edx
004c3b2d 68ff000000               push       0xff
004c3b32 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
004c3b35 e826d0faff               call       0x470b60
004c3b3a 90                       nop        
004c3b3b eb38                     jmp        0x4c3b75
004c3b3d 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c3b40 e81bb6f4ff               call       0x40f160
004c3b45 8b00                     mov        eax, dword ptr [eax]
004c3b47 05fc050000               add        eax, 0x5fc
004c3b4c 8945b4                   mov        dword ptr [ebp - 0x4c], eax
004c3b4f f30f1045d0               movss      xmm0, dword ptr [ebp - 0x30]
004c3b54 f30f5e0594cd5600         divss      xmm0, dword ptr [0x56cd94]
004c3b5c f30f59059ccd5600         mulss      xmm0, dword ptr [0x56cd9c]
004c3b64 f30f2cc8                 cvttss2si  ecx, xmm0
004c3b68 0fb6d1                   movzx      edx, cl
004c3b6b 52                       push       edx
004c3b6c 8b4db4                   mov        ecx, dword ptr [ebp - 0x4c]
004c3b6f e8eccffaff               call       0x470b60
004c3b74 90                       nop        
004c3b75 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c3b78 e8e3b5f4ff               call       0x40f160
004c3b7d 8b08                     mov        ecx, dword ptr [eax]
004c3b7f 81c1fc050000             add        ecx, 0x5fc
004c3b85 e8e689f8ff               call       0x44c570
004c3b8a 90                       nop        
004c3b8b 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c3b8e e8cdb5f4ff               call       0x40f160
004c3b93 8b00                     mov        eax, dword ptr [eax]
004c3b95 c7802c0c000001000000     mov        dword ptr [eax + 0xc2c], 1
004c3b9f eb26                     jmp        0x4c3bc7
004c3ba1 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c3ba4 e8b7b5f4ff               call       0x40f160
004c3ba9 8b08                     mov        ecx, dword ptr [eax]
004c3bab 83c118                   add        ecx, 0x18
004c3bae e8bd89f8ff               call       0x44c570
004c3bb3 8b4de0                   mov        ecx, dword ptr [ebp - 0x20]
004c3bb6 e8a5b5f4ff               call       0x40f160
004c3bbb 8b08                     mov        ecx, dword ptr [eax]
004c3bbd c7812c0c000000000000     mov        dword ptr [ecx + 0xc2c], 0
004c3bc7 e9affdffff               jmp        0x4c397b
004c3bcc c745fcffffffff           mov        dword ptr [ebp - 4], 0xffffffff
004c3bd3 8d4de4                   lea        ecx, [ebp - 0x1c]
004c3bd6 e825dff4ff               call       0x411b00
004c3bdb 90                       nop        
004c3bdc b801000000               mov        eax, 1
004c3be1 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
004c3be4 64890d00000000           mov        dword ptr fs:[0], ecx
004c3beb 59                       pop        ecx
004c3bec 8b4dec                   mov        ecx, dword ptr [ebp - 0x14]
004c3bef 33cd                     xor        ecx, ebp
004c3bf1 e8f8ed0700               call       0x5429ee
004c3bf6 8be5                     mov        esp, ebp
004c3bf8 5d                       pop        ebp
004c3bf9 8be3                     mov        esp, ebx
004c3bfb 5b                       pop        ebx
004c3bfc c20400                   ret        4
004c3bff cc                       int3       
