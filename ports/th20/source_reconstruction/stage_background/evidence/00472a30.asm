00472a30 55                       push       ebp
00472a31 8bec                     mov        ebp, esp
00472a33 83ec0c                   sub        esp, 0xc
00472a36 56                       push       esi
00472a37 57                       push       edi
00472a38 894dfc                   mov        dword ptr [ebp - 4], ecx
00472a3b 8b45fc                   mov        eax, dword ptr [ebp - 4]
00472a3e 8b8844330000             mov        ecx, dword ptr [eax + 0x3344]
00472a44 c1e903                   shr        ecx, 3
00472a47 83e101                   and        ecx, 1
00472a4a 740a                     je         0x472a56
00472a4c b801000000               mov        eax, 1
00472a51 e9b0020000               jmp        0x472d06
00472a56 8b55fc                   mov        edx, dword ptr [ebp - 4]
00472a59 8b8244330000             mov        eax, dword ptr [edx + 0x3344]
00472a5f c1e802                   shr        eax, 2
00472a62 83e001                   and        eax, 1
00472a65 7417                     je         0x472a7e
00472a67 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00472a6a 81c148330000             add        ecx, 0x3348
00472a70 e81bd5f9ff               call       0x40ff90
00472a75 83f83c                   cmp        eax, 0x3c
00472a78 0f8d20010000             jge        0x472b9e
00472a7e 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00472a84 e8372bfdff               call       0x4455c0
00472a89 b96c010000               mov        ecx, 0x16c
00472a8e 6bd103                   imul       edx, ecx, 3
00472a91 b808000000               mov        eax, 8
00472a96 d1e0                     shl        eax, 1
00472a98 b908000000               mov        ecx, 8
00472a9d 6bc900                   imul       ecx, ecx, 0
00472aa0 8b75fc                   mov        esi, dword ptr [ebp - 4]
00472aa3 8b9402e4505c00           mov        edx, dword ptr [edx + eax + 0x5c50e4]
00472aaa 89940e40030000           mov        dword ptr [esi + ecx + 0x340], edx
00472ab1 b86c010000               mov        eax, 0x16c
00472ab6 6bc803                   imul       ecx, eax, 3
00472ab9 ba08000000               mov        edx, 8
00472abe d1e2                     shl        edx, 1
00472ac0 b808000000               mov        eax, 8
00472ac5 6bc000                   imul       eax, eax, 0
00472ac8 8b75fc                   mov        esi, dword ptr [ebp - 4]
00472acb 8b8c11e8505c00           mov        ecx, dword ptr [ecx + edx + 0x5c50e8]
00472ad2 898c0644030000           mov        dword ptr [esi + eax + 0x344], ecx
00472ad9 8b75fc                   mov        esi, dword ptr [ebp - 4]
00472adc 81c614020000             add        esi, 0x214
00472ae2 ba6c010000               mov        edx, 0x16c
00472ae7 6bfa03                   imul       edi, edx, 3
00472aea 81c7b84f5c00             add        edi, 0x5c4fb8
00472af0 b95b000000               mov        ecx, 0x5b
00472af5 f3a5                     rep movsd  dword ptr es:[edi], dword ptr [esi]
00472af7 6a03                     push       3
00472af9 b9404d5c00               mov        ecx, 0x5c4d40
00472afe e81d4a0000               call       0x477520
00472b03 b9404d5c00               mov        ecx, 0x5c4d40
00472b08 e853af0600               call       0x4dda60
00472b0d b9404d5c00               mov        ecx, 0x5c4d40
00472b12 e869b40600               call       0x4ddf80
00472b17 6a08                     push       8
00472b19 6a17                     push       0x17
00472b1b b9404d5c00               mov        ecx, 0x5c4d40
00472b20 e88b720600               call       0x4d9db0
00472b25 6a27                     push       0x27
00472b27 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00472b2d e80e73fdff               call       0x449e40
00472b32 6a04                     push       4
00472b34 6a17                     push       0x17
00472b36 b9404d5c00               mov        ecx, 0x5c4d40
00472b3b e870720600               call       0x4d9db0
00472b40 6a28                     push       0x28
00472b42 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00472b48 e8f372fdff               call       0x449e40
00472b4d 6a04                     push       4
00472b4f 6a17                     push       0x17
00472b51 b9404d5c00               mov        ecx, 0x5c4d40
00472b56 e855720600               call       0x4d9db0
00472b5b 8b45fc                   mov        eax, dword ptr [ebp - 4]
00472b5e 8b887c030000             mov        ecx, dword ptr [eax + 0x37c]
00472b64 51                       push       ecx
00472b65 6a22                     push       0x22
00472b67 b9404d5c00               mov        ecx, 0x5c4d40
00472b6c e83f720600               call       0x4d9db0
00472b71 8b55fc                   mov        edx, dword ptr [ebp - 4]
00472b74 8b8264030000             mov        eax, dword ptr [edx + 0x364]
00472b7a 50                       push       eax
00472b7b 6a24                     push       0x24
00472b7d b9404d5c00               mov        ecx, 0x5c4d40
00472b82 e829720600               call       0x4d9db0
00472b87 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00472b8a 8b9168030000             mov        edx, dword ptr [ecx + 0x368]
00472b90 52                       push       edx
00472b91 6a25                     push       0x25
00472b93 b9404d5c00               mov        ecx, 0x5c4d40
00472b98 e813720600               call       0x4d9db0
00472b9d 90                       nop        
00472b9e 8b45fc                   mov        eax, dword ptr [ebp - 4]
00472ba1 8b8844330000             mov        ecx, dword ptr [eax + 0x3344]
00472ba7 c1e902                   shr        ecx, 2
00472baa 83e101                   and        ecx, 1
00472bad 741f                     je         0x472bce
00472baf 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00472bb2 81c148330000             add        ecx, 0x3348
00472bb8 e8d3d3f9ff               call       0x40ff90
00472bbd 83f81e                   cmp        eax, 0x1e
00472bc0 7d02                     jge        0x472bc4
00472bc2 eb0a                     jmp        0x472bce
00472bc4 8b55fc                   mov        edx, dword ptr [ebp - 4]
00472bc7 c6821f33000000           mov        byte ptr [edx + 0x331f], 0
00472bce 8b45fc                   mov        eax, dword ptr [ebp - 4]
00472bd1 8b8844330000             mov        ecx, dword ptr [eax + 0x3344]
00472bd7 83e101                   and        ecx, 1
00472bda 7448                     je         0x472c24
00472bdc b9404d5c00               mov        ecx, 0x5c4d40
00472be1 e89ab30600               call       0x4ddf80
00472be6 b9404d5c00               mov        ecx, 0x5c4d40
00472beb e8d0ae0600               call       0x4ddac0
00472bf0 6a08                     push       8
00472bf2 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00472bf5 e8861c0000               call       0x474880
00472bfa 6a09                     push       9
00472bfc 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00472bff e87c1c0000               call       0x474880
00472c04 6a0a                     push       0xa
00472c06 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00472c09 e8721c0000               call       0x474880
00472c0e 6a0b                     push       0xb
00472c10 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00472c13 e8681c0000               call       0x474880
00472c18 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00472c1e e89d29fdff               call       0x4455c0
00472c23 90                       nop        
00472c24 8b0d28005c00             mov        ecx, dword ptr [0x5c0028]
00472c2a e8f1240000               call       0x475120
00472c2f 90                       nop        
00472c30 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00472c33 81c148330000             add        ecx, 0x3348
00472c39 e852d3f9ff               call       0x40ff90
00472c3e 85c0                     test       eax, eax
00472c40 0f8e99000000             jle        0x472cdf
00472c46 8b55fc                   mov        edx, dword ptr [ebp - 4]
00472c49 81c248330000             add        edx, 0x3348
00472c4f 8955f8                   mov        dword ptr [ebp - 8], edx
00472c52 6a00                     push       0
00472c54 8b4df8                   mov        ecx, dword ptr [ebp - 8]
00472c57 e8c467fbff               call       0x429420
00472c5c 90                       nop        
00472c5d 8b45fc                   mov        eax, dword ptr [ebp - 4]
00472c60 0548330000               add        eax, 0x3348
00472c65 8945f4                   mov        dword ptr [ebp - 0xc], eax
00472c68 6a00                     push       0
00472c6a 8b4df4                   mov        ecx, dword ptr [ebp - 0xc]
00472c6d e81e09fbff               call       0x423590
00472c72 0fb6c8                   movzx      ecx, al
00472c75 85c9                     test       ecx, ecx
00472c77 7466                     je         0x472cdf
00472c79 8b55fc                   mov        edx, dword ptr [ebp - 4]
00472c7c c6821f330000ff           mov        byte ptr [edx + 0x331f], 0xff
00472c83 8b45fc                   mov        eax, dword ptr [ebp - 4]
00472c86 8b8844330000             mov        ecx, dword ptr [eax + 0x3344]
00472c8c d1e9                     shr        ecx, 1
00472c8e 83e101                   and        ecx, 1
00472c91 7415                     je         0x472ca8
00472c93 8b55fc                   mov        edx, dword ptr [ebp - 4]
00472c96 8b8244330000             mov        eax, dword ptr [edx + 0x3344]
00472c9c 83c808                   or         eax, 8
00472c9f 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00472ca2 898144330000             mov        dword ptr [ecx + 0x3344], eax
00472ca8 8b55fc                   mov        edx, dword ptr [ebp - 4]
00472cab c7821c330000ffffff00     mov        dword ptr [edx + 0x331c], 0xffffff
00472cb5 8b45fc                   mov        eax, dword ptr [ebp - 4]
00472cb8 8b8844330000             mov        ecx, dword ptr [eax + 0x3344]
00472cbe 83e1fd                   and        ecx, 0xfffffffd
00472cc1 8b55fc                   mov        edx, dword ptr [ebp - 4]
00472cc4 898a44330000             mov        dword ptr [edx + 0x3344], ecx
00472cca 8b45fc                   mov        eax, dword ptr [ebp - 4]
00472ccd 8b8844330000             mov        ecx, dword ptr [eax + 0x3344]
00472cd3 83e1fb                   and        ecx, 0xfffffffb
00472cd6 8b55fc                   mov        edx, dword ptr [ebp - 4]
00472cd9 898a44330000             mov        dword ptr [edx + 0x3344], ecx
00472cdf b9404d5c00               mov        ecx, 0x5c4d40
00472ce4 e897b20600               call       0x4ddf80
00472ce9 6a08                     push       8
00472ceb 6a17                     push       0x17
00472ced b9404d5c00               mov        ecx, 0x5c4d40
00472cf2 e8b9700600               call       0x4d9db0
00472cf7 b9404d5c00               mov        ecx, 0x5c4d40
00472cfc e85fad0600               call       0x4dda60
00472d01 b801000000               mov        eax, 1
00472d06 5f                       pop        edi
00472d07 5e                       pop        esi
00472d08 8be5                     mov        esp, ebp
00472d0a 5d                       pop        ebp
00472d0b c3                       ret        
00472d0c cc                       int3       
00472d0d cc                       int3       
00472d0e cc                       int3       
00472d0f cc                       int3       
