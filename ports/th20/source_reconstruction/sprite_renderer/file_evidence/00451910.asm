00451910 55                       push       ebp
00451911 8bec                     mov        ebp, esp
00451913 81ecb4000000             sub        esp, 0xb4
00451919 a1c0255b00               mov        eax, dword ptr [0x5b25c0]
0045191e 33c5                     xor        eax, ebp
00451920 8945fc                   mov        dword ptr [ebp - 4], eax
00451923 56                       push       esi
00451924 898d4cffffff             mov        dword ptr [ebp - 0xb4], ecx
0045192a c7459800000000           mov        dword ptr [ebp - 0x68], 0
00451931 8b4508                   mov        eax, dword ptr [ebp + 8]
00451934 8b08                     mov        ecx, dword ptr [eax]
00451936 8b5148                   mov        edx, dword ptr [ecx + 0x48]
00451939 899558ffffff             mov        dword ptr [ebp - 0xa8], edx
0045193f 8d4598                   lea        eax, [ebp - 0x68]
00451942 50                       push       eax
00451943 6a00                     push       0
00451945 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00451948 51                       push       ecx
00451949 ff9558ffffff             call       dword ptr [ebp - 0xa8]
0045194f 90                       nop        
00451950 837d9800                 cmp        dword ptr [ebp - 0x68], 0
00451954 7505                     jne        0x45195b
00451956 e95d0a0000               jmp        0x4523b8
0045195b 8b5598                   mov        edx, dword ptr [ebp - 0x68]
0045195e 8b02                     mov        eax, dword ptr [edx]
00451960 8b4830                   mov        ecx, dword ptr [eax + 0x30]
00451963 898d54ffffff             mov        dword ptr [ebp - 0xac], ecx
00451969 8d559c                   lea        edx, [ebp - 0x64]
0045196c 52                       push       edx
0045196d 8b4598                   mov        eax, dword ptr [ebp - 0x68]
00451970 50                       push       eax
00451971 ff9554ffffff             call       dword ptr [ebp - 0xac]
00451977 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
0045197a 8b11                     mov        edx, dword ptr [ecx]
0045197c 8b4234                   mov        eax, dword ptr [edx + 0x34]
0045197f 898550ffffff             mov        dword ptr [ebp - 0xb0], eax
00451985 6a00                     push       0
00451987 6a00                     push       0
00451989 8d4d90                   lea        ecx, [ebp - 0x70]
0045198c 51                       push       ecx
0045198d 8b5598                   mov        edx, dword ptr [ebp - 0x68]
00451990 52                       push       edx
00451991 ff9550ffffff             call       dword ptr [ebp - 0xb0]
00451997 90                       nop        
00451998 8b459c                   mov        eax, dword ptr [ebp - 0x64]
0045199b 89855cffffff             mov        dword ptr [ebp - 0xa4], eax
004519a1 83bd5cffffff1d           cmp        dword ptr [ebp - 0xa4], 0x1d
004519a8 0f87ed090000             ja         0x45239b
004519ae 8b8d5cffffff             mov        ecx, dword ptr [ebp - 0xa4]
004519b4 0fb691e0234500           movzx      edx, byte ptr [ecx + 0x4523e0]
004519bb ff2495cc234500           jmp        dword ptr [edx*4 + 0x4523cc]
004519c2 33c0                     xor        eax, eax
004519c4 8945f0                   mov        dword ptr [ebp - 0x10], eax
004519c7 8945f4                   mov        dword ptr [ebp - 0xc], eax
004519ca 8945f8                   mov        dword ptr [ebp - 8], eax
004519cd c7857cffffff00000000     mov        dword ptr [ebp - 0x84], 0
004519d7 eb0f                     jmp        0x4519e8
004519d9 8b8d7cffffff             mov        ecx, dword ptr [ebp - 0x84]
004519df 83c101                   add        ecx, 1
004519e2 898d7cffffff             mov        dword ptr [ebp - 0x84], ecx
004519e8 8b957cffffff             mov        edx, dword ptr [ebp - 0x84]
004519ee 3b55b8                   cmp        edx, dword ptr [ebp - 0x48]
004519f1 0f83f3010000             jae        0x451bea
004519f7 8b4590                   mov        eax, dword ptr [ebp - 0x70]
004519fa 0faf857cffffff           imul       eax, dword ptr [ebp - 0x84]
00451a01 034594                   add        eax, dword ptr [ebp - 0x6c]
00451a04 894580                   mov        dword ptr [ebp - 0x80], eax
00451a07 c7856cffffff00000000     mov        dword ptr [ebp - 0x94], 0
00451a11 eb0f                     jmp        0x451a22
00451a13 8b8d6cffffff             mov        ecx, dword ptr [ebp - 0x94]
00451a19 83c101                   add        ecx, 1
00451a1c 898d6cffffff             mov        dword ptr [ebp - 0x94], ecx
00451a22 8b956cffffff             mov        edx, dword ptr [ebp - 0x94]
00451a28 3b55b4                   cmp        edx, dword ptr [ebp - 0x4c]
00451a2b 0f83b4010000             jae        0x451be5
00451a31 b804000000               mov        eax, 4
00451a36 6bc800                   imul       ecx, eax, 0
00451a39 8b5580                   mov        edx, dword ptr [ebp - 0x80]
00451a3c 0fb6440a03               movzx      eax, byte ptr [edx + ecx + 3]
00451a41 85c0                     test       eax, eax
00451a43 0f858e010000             jne        0x451bd7
00451a49 b904000000               mov        ecx, 4
00451a4e d1e1                     shl        ecx, 1
00451a50 c7440df000000000         mov        dword ptr [ebp + ecx - 0x10], 0
00451a58 ba04000000               mov        edx, 4
00451a5d c1e200                   shl        edx, 0
00451a60 c74415f000000000         mov        dword ptr [ebp + edx - 0x10], 0
00451a68 b804000000               mov        eax, 4
00451a6d 6bc800                   imul       ecx, eax, 0
00451a70 c7440df000000000         mov        dword ptr [ebp + ecx - 0x10], 0
00451a78 c745c800000000           mov        dword ptr [ebp - 0x38], 0
00451a7f 83bd6cffffff00           cmp        dword ptr [ebp - 0x94], 0
00451a86 761c                     jbe        0x451aa4
00451a88 8d55c8                   lea        edx, [ebp - 0x38]
00451a8b 52                       push       edx
00451a8c b804000000               mov        eax, 4
00451a91 6bc8ff                   imul       ecx, eax, -1
00451a94 034d80                   add        ecx, dword ptr [ebp - 0x80]
00451a97 51                       push       ecx
00451a98 8d55f0                   lea        edx, [ebp - 0x10]
00451a9b 52                       push       edx
00451a9c e8dffdffff               call       0x451880
00451aa1 83c40c                   add        esp, 0xc
00451aa4 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
00451aa7 83e801                   sub        eax, 1
00451aaa 39856cffffff             cmp        dword ptr [ebp - 0x94], eax
00451ab0 731c                     jae        0x451ace
00451ab2 8d4dc8                   lea        ecx, [ebp - 0x38]
00451ab5 51                       push       ecx
00451ab6 ba04000000               mov        edx, 4
00451abb c1e200                   shl        edx, 0
00451abe 035580                   add        edx, dword ptr [ebp - 0x80]
00451ac1 52                       push       edx
00451ac2 8d45f0                   lea        eax, [ebp - 0x10]
00451ac5 50                       push       eax
00451ac6 e8b5fdffff               call       0x451880
00451acb 83c40c                   add        esp, 0xc
00451ace 83bd7cffffff00           cmp        dword ptr [ebp - 0x84], 0
00451ad5 7625                     jbe        0x451afc
00451ad7 8d4dc8                   lea        ecx, [ebp - 0x38]
00451ada 51                       push       ecx
00451adb 8b4590                   mov        eax, dword ptr [ebp - 0x70]
00451ade f7d8                     neg        eax
00451ae0 99                       cdq        
00451ae1 83e203                   and        edx, 3
00451ae4 03c2                     add        eax, edx
00451ae6 c1f802                   sar        eax, 2
00451ae9 8b5580                   mov        edx, dword ptr [ebp - 0x80]
00451aec 8d0482                   lea        eax, [edx + eax*4]
00451aef 50                       push       eax
00451af0 8d4df0                   lea        ecx, [ebp - 0x10]
00451af3 51                       push       ecx
00451af4 e887fdffff               call       0x451880
00451af9 83c40c                   add        esp, 0xc
00451afc 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
00451aff 83ea01                   sub        edx, 1
00451b02 39957cffffff             cmp        dword ptr [ebp - 0x84], edx
00451b08 7323                     jae        0x451b2d
00451b0a 8d45c8                   lea        eax, [ebp - 0x38]
00451b0d 50                       push       eax
00451b0e 8b4590                   mov        eax, dword ptr [ebp - 0x70]
00451b11 99                       cdq        
00451b12 83e203                   and        edx, 3
00451b15 03c2                     add        eax, edx
00451b17 c1f802                   sar        eax, 2
00451b1a 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00451b1d 8d1481                   lea        edx, [ecx + eax*4]
00451b20 52                       push       edx
00451b21 8d45f0                   lea        eax, [ebp - 0x10]
00451b24 50                       push       eax
00451b25 e856fdffff               call       0x451880
00451b2a 83c40c                   add        esp, 0xc
00451b2d 837dc801                 cmp        dword ptr [ebp - 0x38], 1
00451b31 7655                     jbe        0x451b88
00451b33 b904000000               mov        ecx, 4
00451b38 6bd100                   imul       edx, ecx, 0
00451b3b 8b4415f0                 mov        eax, dword ptr [ebp + edx - 0x10]
00451b3f 33d2                     xor        edx, edx
00451b41 f775c8                   div        dword ptr [ebp - 0x38]
00451b44 b904000000               mov        ecx, 4
00451b49 6bd100                   imul       edx, ecx, 0
00451b4c 894415f0                 mov        dword ptr [ebp + edx - 0x10], eax
00451b50 b804000000               mov        eax, 4
00451b55 c1e000                   shl        eax, 0
00451b58 8b4405f0                 mov        eax, dword ptr [ebp + eax - 0x10]
00451b5c 33d2                     xor        edx, edx
00451b5e f775c8                   div        dword ptr [ebp - 0x38]
00451b61 b904000000               mov        ecx, 4
00451b66 c1e100                   shl        ecx, 0
00451b69 89440df0                 mov        dword ptr [ebp + ecx - 0x10], eax
00451b6d ba04000000               mov        edx, 4
00451b72 d1e2                     shl        edx, 1
00451b74 8b4415f0                 mov        eax, dword ptr [ebp + edx - 0x10]
00451b78 33d2                     xor        edx, edx
00451b7a f775c8                   div        dword ptr [ebp - 0x38]
00451b7d b904000000               mov        ecx, 4
00451b82 d1e1                     shl        ecx, 1
00451b84 89440df0                 mov        dword ptr [ebp + ecx - 0x10], eax
00451b88 ba04000000               mov        edx, 4
00451b8d 6bc200                   imul       eax, edx, 0
00451b90 b904000000               mov        ecx, 4
00451b95 6bd100                   imul       edx, ecx, 0
00451b98 8b4d80                   mov        ecx, dword ptr [ebp - 0x80]
00451b9b 8a4405f0                 mov        al, byte ptr [ebp + eax - 0x10]
00451b9f 88441102                 mov        byte ptr [ecx + edx + 2], al
00451ba3 b904000000               mov        ecx, 4
00451ba8 c1e100                   shl        ecx, 0
00451bab ba04000000               mov        edx, 4
00451bb0 6bc200                   imul       eax, edx, 0
00451bb3 8b5580                   mov        edx, dword ptr [ebp - 0x80]
00451bb6 8a4c0df0                 mov        cl, byte ptr [ebp + ecx - 0x10]
00451bba 884c0201                 mov        byte ptr [edx + eax + 1], cl
00451bbe ba04000000               mov        edx, 4
00451bc3 d1e2                     shl        edx, 1
00451bc5 b804000000               mov        eax, 4
00451bca 6bc800                   imul       ecx, eax, 0
00451bcd 8b4580                   mov        eax, dword ptr [ebp - 0x80]
00451bd0 8a5415f0                 mov        dl, byte ptr [ebp + edx - 0x10]
00451bd4 881408                   mov        byte ptr [eax + ecx], dl
00451bd7 8b4580                   mov        eax, dword ptr [ebp - 0x80]
00451bda 83c004                   add        eax, 4
00451bdd 894580                   mov        dword ptr [ebp - 0x80], eax
00451be0 e92efeffff               jmp        0x451a13
00451be5 e9effdffff               jmp        0x4519d9
00451bea e9ac070000               jmp        0x45239b
00451bef 33c9                     xor        ecx, ecx
00451bf1 894de4                   mov        dword ptr [ebp - 0x1c], ecx
00451bf4 894de8                   mov        dword ptr [ebp - 0x18], ecx
00451bf7 894dec                   mov        dword ptr [ebp - 0x14], ecx
00451bfa c78578ffffff00000000     mov        dword ptr [ebp - 0x88], 0
00451c04 eb0f                     jmp        0x451c15
00451c06 8b9578ffffff             mov        edx, dword ptr [ebp - 0x88]
00451c0c 83c201                   add        edx, 1
00451c0f 899578ffffff             mov        dword ptr [ebp - 0x88], edx
00451c15 8b8578ffffff             mov        eax, dword ptr [ebp - 0x88]
00451c1b 3b45b8                   cmp        eax, dword ptr [ebp - 0x48]
00451c1e 0f8355020000             jae        0x451e79
00451c24 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00451c27 0faf8d78ffffff           imul       ecx, dword ptr [ebp - 0x88]
00451c2e 034d94                   add        ecx, dword ptr [ebp - 0x6c]
00451c31 894d8c                   mov        dword ptr [ebp - 0x74], ecx
00451c34 c78568ffffff00000000     mov        dword ptr [ebp - 0x98], 0
00451c3e eb0f                     jmp        0x451c4f
00451c40 8b9568ffffff             mov        edx, dword ptr [ebp - 0x98]
00451c46 83c201                   add        edx, 1
00451c49 899568ffffff             mov        dword ptr [ebp - 0x98], edx
00451c4f 8b8568ffffff             mov        eax, dword ptr [ebp - 0x98]
00451c55 3b45b4                   cmp        eax, dword ptr [ebp - 0x4c]
00451c58 0f8316020000             jae        0x451e74
00451c5e b902000000               mov        ecx, 2
00451c63 6bd100                   imul       edx, ecx, 0
00451c66 8b458c                   mov        eax, dword ptr [ebp - 0x74]
00451c69 668b0c10                 mov        cx, word ptr [eax + edx]
00451c6d 66c1e90f                 shr        cx, 0xf
00451c71 6683e101                 and        cx, 1
00451c75 0fb7d1                   movzx      edx, cx
00451c78 85d2                     test       edx, edx
00451c7a 0f85e6010000             jne        0x451e66
00451c80 b804000000               mov        eax, 4
00451c85 d1e0                     shl        eax, 1
00451c87 c74405e400000000         mov        dword ptr [ebp + eax - 0x1c], 0
00451c8f b904000000               mov        ecx, 4
00451c94 c1e100                   shl        ecx, 0
00451c97 c7440de400000000         mov        dword ptr [ebp + ecx - 0x1c], 0
00451c9f ba04000000               mov        edx, 4
00451ca4 6bc200                   imul       eax, edx, 0
00451ca7 c74405e400000000         mov        dword ptr [ebp + eax - 0x1c], 0
00451caf c745c400000000           mov        dword ptr [ebp - 0x3c], 0
00451cb6 83bd68ffffff00           cmp        dword ptr [ebp - 0x98], 0
00451cbd 761c                     jbe        0x451cdb
00451cbf 8d4dc4                   lea        ecx, [ebp - 0x3c]
00451cc2 51                       push       ecx
00451cc3 ba02000000               mov        edx, 2
00451cc8 6bc2ff                   imul       eax, edx, -1
00451ccb 03458c                   add        eax, dword ptr [ebp - 0x74]
00451cce 50                       push       eax
00451ccf 8d4de4                   lea        ecx, [ebp - 0x1c]
00451cd2 51                       push       ecx
00451cd3 e868f9ffff               call       0x451640
00451cd8 83c40c                   add        esp, 0xc
00451cdb 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
00451cde 83ea01                   sub        edx, 1
00451ce1 399568ffffff             cmp        dword ptr [ebp - 0x98], edx
00451ce7 731c                     jae        0x451d05
00451ce9 8d45c4                   lea        eax, [ebp - 0x3c]
00451cec 50                       push       eax
00451ced b902000000               mov        ecx, 2
00451cf2 c1e100                   shl        ecx, 0
00451cf5 034d8c                   add        ecx, dword ptr [ebp - 0x74]
00451cf8 51                       push       ecx
00451cf9 8d55e4                   lea        edx, [ebp - 0x1c]
00451cfc 52                       push       edx
00451cfd e83ef9ffff               call       0x451640
00451d02 83c40c                   add        esp, 0xc
00451d05 83bd78ffffff00           cmp        dword ptr [ebp - 0x88], 0
00451d0c 7621                     jbe        0x451d2f
00451d0e 8d45c4                   lea        eax, [ebp - 0x3c]
00451d11 50                       push       eax
00451d12 8b4590                   mov        eax, dword ptr [ebp - 0x70]
00451d15 f7d8                     neg        eax
00451d17 99                       cdq        
00451d18 2bc2                     sub        eax, edx
00451d1a d1f8                     sar        eax, 1
00451d1c 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00451d1f 8d1441                   lea        edx, [ecx + eax*2]
00451d22 52                       push       edx
00451d23 8d45e4                   lea        eax, [ebp - 0x1c]
00451d26 50                       push       eax
00451d27 e814f9ffff               call       0x451640
00451d2c 83c40c                   add        esp, 0xc
00451d2f 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00451d32 83e901                   sub        ecx, 1
00451d35 398d78ffffff             cmp        dword ptr [ebp - 0x88], ecx
00451d3b 731f                     jae        0x451d5c
00451d3d 8d55c4                   lea        edx, [ebp - 0x3c]
00451d40 52                       push       edx
00451d41 8b4590                   mov        eax, dword ptr [ebp - 0x70]
00451d44 99                       cdq        
00451d45 2bc2                     sub        eax, edx
00451d47 d1f8                     sar        eax, 1
00451d49 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00451d4c 8d1441                   lea        edx, [ecx + eax*2]
00451d4f 52                       push       edx
00451d50 8d45e4                   lea        eax, [ebp - 0x1c]
00451d53 50                       push       eax
00451d54 e8e7f8ffff               call       0x451640
00451d59 83c40c                   add        esp, 0xc
00451d5c 837dc401                 cmp        dword ptr [ebp - 0x3c], 1
00451d60 7655                     jbe        0x451db7
00451d62 b904000000               mov        ecx, 4
00451d67 6bd100                   imul       edx, ecx, 0
00451d6a 8b4415e4                 mov        eax, dword ptr [ebp + edx - 0x1c]
00451d6e 33d2                     xor        edx, edx
00451d70 f775c4                   div        dword ptr [ebp - 0x3c]
00451d73 b904000000               mov        ecx, 4
00451d78 6bd100                   imul       edx, ecx, 0
00451d7b 894415e4                 mov        dword ptr [ebp + edx - 0x1c], eax
00451d7f b804000000               mov        eax, 4
00451d84 c1e000                   shl        eax, 0
00451d87 8b4405e4                 mov        eax, dword ptr [ebp + eax - 0x1c]
00451d8b 33d2                     xor        edx, edx
00451d8d f775c4                   div        dword ptr [ebp - 0x3c]
00451d90 b904000000               mov        ecx, 4
00451d95 c1e100                   shl        ecx, 0
00451d98 89440de4                 mov        dword ptr [ebp + ecx - 0x1c], eax
00451d9c ba04000000               mov        edx, 4
00451da1 d1e2                     shl        edx, 1
00451da3 8b4415e4                 mov        eax, dword ptr [ebp + edx - 0x1c]
00451da7 33d2                     xor        edx, edx
00451da9 f775c4                   div        dword ptr [ebp - 0x3c]
00451dac b904000000               mov        ecx, 4
00451db1 d1e1                     shl        ecx, 1
00451db3 89440de4                 mov        dword ptr [ebp + ecx - 0x1c], eax
00451db7 ba04000000               mov        edx, 4
00451dbc 6bc200                   imul       eax, edx, 0
00451dbf 660fb64c05e4             movzx      cx, byte ptr [ebp + eax - 0x1c]
00451dc5 ba02000000               mov        edx, 2
00451dca 6bc200                   imul       eax, edx, 0
00451dcd 6683e11f                 and        cx, 0x1f
00451dd1 66c1e10a                 shl        cx, 0xa
00451dd5 baff830000               mov        edx, 0x83ff
00451dda 8b758c                   mov        esi, dword ptr [ebp - 0x74]
00451ddd 66231406                 and        dx, word ptr [esi + eax]
00451de1 660bd1                   or         dx, cx
00451de4 b802000000               mov        eax, 2
00451de9 6bc800                   imul       ecx, eax, 0
00451dec 8b458c                   mov        eax, dword ptr [ebp - 0x74]
00451def 66891408                 mov        word ptr [eax + ecx], dx
00451df3 b904000000               mov        ecx, 4
00451df8 c1e100                   shl        ecx, 0
00451dfb 660fb6540de4             movzx      dx, byte ptr [ebp + ecx - 0x1c]
00451e01 b802000000               mov        eax, 2
00451e06 6bc800                   imul       ecx, eax, 0
00451e09 6683e21f                 and        dx, 0x1f
00451e0d 66c1e205                 shl        dx, 5
00451e11 b81ffc0000               mov        eax, 0xfc1f
00451e16 8b758c                   mov        esi, dword ptr [ebp - 0x74]
00451e19 6623040e                 and        ax, word ptr [esi + ecx]
00451e1d 660bc2                   or         ax, dx
00451e20 b902000000               mov        ecx, 2
00451e25 6bd100                   imul       edx, ecx, 0
00451e28 8b4d8c                   mov        ecx, dword ptr [ebp - 0x74]
00451e2b 66890411                 mov        word ptr [ecx + edx], ax
00451e2f ba04000000               mov        edx, 4
00451e34 d1e2                     shl        edx, 1
00451e36 660fb64415e4             movzx      ax, byte ptr [ebp + edx - 0x1c]
00451e3c b902000000               mov        ecx, 2
00451e41 6bd100                   imul       edx, ecx, 0
00451e44 6683e01f                 and        ax, 0x1f
00451e48 b9e0ff0000               mov        ecx, 0xffe0
00451e4d 8b758c                   mov        esi, dword ptr [ebp - 0x74]
00451e50 66230c16                 and        cx, word ptr [esi + edx]
00451e54 660bc8                   or         cx, ax
00451e57 ba02000000               mov        edx, 2
00451e5c 6bc200                   imul       eax, edx, 0
00451e5f 8b558c                   mov        edx, dword ptr [ebp - 0x74]
00451e62 66890c02                 mov        word ptr [edx + eax], cx
00451e66 8b458c                   mov        eax, dword ptr [ebp - 0x74]
00451e69 83c002                   add        eax, 2
00451e6c 89458c                   mov        dword ptr [ebp - 0x74], eax
00451e6f e9ccfdffff               jmp        0x451c40
00451e74 e98dfdffff               jmp        0x451c06
00451e79 e91d050000               jmp        0x45239b
00451e7e 33c9                     xor        ecx, ecx
00451e80 894dd8                   mov        dword ptr [ebp - 0x28], ecx
00451e83 894ddc                   mov        dword ptr [ebp - 0x24], ecx
00451e86 894de0                   mov        dword ptr [ebp - 0x20], ecx
00451e89 c78574ffffff00000000     mov        dword ptr [ebp - 0x8c], 0
00451e93 eb0f                     jmp        0x451ea4
00451e95 8b9574ffffff             mov        edx, dword ptr [ebp - 0x8c]
00451e9b 83c201                   add        edx, 1
00451e9e 899574ffffff             mov        dword ptr [ebp - 0x8c], edx
00451ea4 8b8574ffffff             mov        eax, dword ptr [ebp - 0x8c]
00451eaa 3b45b8                   cmp        eax, dword ptr [ebp - 0x48]
00451ead 0f8355020000             jae        0x452108
00451eb3 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00451eb6 0faf8d74ffffff           imul       ecx, dword ptr [ebp - 0x8c]
00451ebd 034d94                   add        ecx, dword ptr [ebp - 0x6c]
00451ec0 894d88                   mov        dword ptr [ebp - 0x78], ecx
00451ec3 c78564ffffff00000000     mov        dword ptr [ebp - 0x9c], 0
00451ecd eb0f                     jmp        0x451ede
00451ecf 8b9564ffffff             mov        edx, dword ptr [ebp - 0x9c]
00451ed5 83c201                   add        edx, 1
00451ed8 899564ffffff             mov        dword ptr [ebp - 0x9c], edx
00451ede 8b8564ffffff             mov        eax, dword ptr [ebp - 0x9c]
00451ee4 3b45b4                   cmp        eax, dword ptr [ebp - 0x4c]
00451ee7 0f8316020000             jae        0x452103
00451eed b902000000               mov        ecx, 2
00451ef2 6bd100                   imul       edx, ecx, 0
00451ef5 8b4588                   mov        eax, dword ptr [ebp - 0x78]
00451ef8 668b0c10                 mov        cx, word ptr [eax + edx]
00451efc 66c1e90c                 shr        cx, 0xc
00451f00 6683e10f                 and        cx, 0xf
00451f04 0fb7d1                   movzx      edx, cx
00451f07 85d2                     test       edx, edx
00451f09 0f85e6010000             jne        0x4520f5
00451f0f b804000000               mov        eax, 4
00451f14 d1e0                     shl        eax, 1
00451f16 c74405d800000000         mov        dword ptr [ebp + eax - 0x28], 0
00451f1e b904000000               mov        ecx, 4
00451f23 c1e100                   shl        ecx, 0
00451f26 c7440dd800000000         mov        dword ptr [ebp + ecx - 0x28], 0
00451f2e ba04000000               mov        edx, 4
00451f33 6bc200                   imul       eax, edx, 0
00451f36 c74405d800000000         mov        dword ptr [ebp + eax - 0x28], 0
00451f3e c745c000000000           mov        dword ptr [ebp - 0x40], 0
00451f45 83bd64ffffff00           cmp        dword ptr [ebp - 0x9c], 0
00451f4c 761c                     jbe        0x451f6a
00451f4e 8d4dc0                   lea        ecx, [ebp - 0x40]
00451f51 51                       push       ecx
00451f52 ba02000000               mov        edx, 2
00451f57 6bc2ff                   imul       eax, edx, -1
00451f5a 034588                   add        eax, dword ptr [ebp - 0x78]
00451f5d 50                       push       eax
00451f5e 8d4dd8                   lea        ecx, [ebp - 0x28]
00451f61 51                       push       ecx
00451f62 e899f7ffff               call       0x451700
00451f67 83c40c                   add        esp, 0xc
00451f6a 8b55b4                   mov        edx, dword ptr [ebp - 0x4c]
00451f6d 83ea01                   sub        edx, 1
00451f70 399564ffffff             cmp        dword ptr [ebp - 0x9c], edx
00451f76 731c                     jae        0x451f94
00451f78 8d45c0                   lea        eax, [ebp - 0x40]
00451f7b 50                       push       eax
00451f7c b902000000               mov        ecx, 2
00451f81 c1e100                   shl        ecx, 0
00451f84 034d88                   add        ecx, dword ptr [ebp - 0x78]
00451f87 51                       push       ecx
00451f88 8d55d8                   lea        edx, [ebp - 0x28]
00451f8b 52                       push       edx
00451f8c e86ff7ffff               call       0x451700
00451f91 83c40c                   add        esp, 0xc
00451f94 83bd74ffffff00           cmp        dword ptr [ebp - 0x8c], 0
00451f9b 7621                     jbe        0x451fbe
00451f9d 8d45c0                   lea        eax, [ebp - 0x40]
00451fa0 50                       push       eax
00451fa1 8b4590                   mov        eax, dword ptr [ebp - 0x70]
00451fa4 f7d8                     neg        eax
00451fa6 99                       cdq        
00451fa7 2bc2                     sub        eax, edx
00451fa9 d1f8                     sar        eax, 1
00451fab 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00451fae 8d1441                   lea        edx, [ecx + eax*2]
00451fb1 52                       push       edx
00451fb2 8d45d8                   lea        eax, [ebp - 0x28]
00451fb5 50                       push       eax
00451fb6 e845f7ffff               call       0x451700
00451fbb 83c40c                   add        esp, 0xc
00451fbe 8b4db8                   mov        ecx, dword ptr [ebp - 0x48]
00451fc1 83e901                   sub        ecx, 1
00451fc4 398d74ffffff             cmp        dword ptr [ebp - 0x8c], ecx
00451fca 731f                     jae        0x451feb
00451fcc 8d55c0                   lea        edx, [ebp - 0x40]
00451fcf 52                       push       edx
00451fd0 8b4590                   mov        eax, dword ptr [ebp - 0x70]
00451fd3 99                       cdq        
00451fd4 2bc2                     sub        eax, edx
00451fd6 d1f8                     sar        eax, 1
00451fd8 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
00451fdb 8d1441                   lea        edx, [ecx + eax*2]
00451fde 52                       push       edx
00451fdf 8d45d8                   lea        eax, [ebp - 0x28]
00451fe2 50                       push       eax
00451fe3 e818f7ffff               call       0x451700
00451fe8 83c40c                   add        esp, 0xc
00451feb 837dc001                 cmp        dword ptr [ebp - 0x40], 1
00451fef 7655                     jbe        0x452046
00451ff1 b904000000               mov        ecx, 4
00451ff6 6bd100                   imul       edx, ecx, 0
00451ff9 8b4415d8                 mov        eax, dword ptr [ebp + edx - 0x28]
00451ffd 33d2                     xor        edx, edx
00451fff f775c0                   div        dword ptr [ebp - 0x40]
00452002 b904000000               mov        ecx, 4
00452007 6bd100                   imul       edx, ecx, 0
0045200a 894415d8                 mov        dword ptr [ebp + edx - 0x28], eax
0045200e b804000000               mov        eax, 4
00452013 c1e000                   shl        eax, 0
00452016 8b4405d8                 mov        eax, dword ptr [ebp + eax - 0x28]
0045201a 33d2                     xor        edx, edx
0045201c f775c0                   div        dword ptr [ebp - 0x40]
0045201f b904000000               mov        ecx, 4
00452024 c1e100                   shl        ecx, 0
00452027 89440dd8                 mov        dword ptr [ebp + ecx - 0x28], eax
0045202b ba04000000               mov        edx, 4
00452030 d1e2                     shl        edx, 1
00452032 8b4415d8                 mov        eax, dword ptr [ebp + edx - 0x28]
00452036 33d2                     xor        edx, edx
00452038 f775c0                   div        dword ptr [ebp - 0x40]
0045203b b904000000               mov        ecx, 4
00452040 d1e1                     shl        ecx, 1
00452042 89440dd8                 mov        dword ptr [ebp + ecx - 0x28], eax
00452046 ba04000000               mov        edx, 4
0045204b 6bc200                   imul       eax, edx, 0
0045204e 660fb64c05d8             movzx      cx, byte ptr [ebp + eax - 0x28]
00452054 ba02000000               mov        edx, 2
00452059 6bc200                   imul       eax, edx, 0
0045205c 6683e10f                 and        cx, 0xf
00452060 66c1e108                 shl        cx, 8
00452064 bafff00000               mov        edx, 0xf0ff
00452069 8b7588                   mov        esi, dword ptr [ebp - 0x78]
0045206c 66231406                 and        dx, word ptr [esi + eax]
00452070 660bd1                   or         dx, cx
00452073 b802000000               mov        eax, 2
00452078 6bc800                   imul       ecx, eax, 0
0045207b 8b4588                   mov        eax, dword ptr [ebp - 0x78]
0045207e 66891408                 mov        word ptr [eax + ecx], dx
00452082 b904000000               mov        ecx, 4
00452087 c1e100                   shl        ecx, 0
0045208a 660fb6540dd8             movzx      dx, byte ptr [ebp + ecx - 0x28]
00452090 b802000000               mov        eax, 2
00452095 6bc800                   imul       ecx, eax, 0
00452098 6683e20f                 and        dx, 0xf
0045209c 66c1e204                 shl        dx, 4
004520a0 b80fff0000               mov        eax, 0xff0f
004520a5 8b7588                   mov        esi, dword ptr [ebp - 0x78]
004520a8 6623040e                 and        ax, word ptr [esi + ecx]
004520ac 660bc2                   or         ax, dx
004520af b902000000               mov        ecx, 2
004520b4 6bd100                   imul       edx, ecx, 0
004520b7 8b4d88                   mov        ecx, dword ptr [ebp - 0x78]
004520ba 66890411                 mov        word ptr [ecx + edx], ax
004520be ba04000000               mov        edx, 4
004520c3 d1e2                     shl        edx, 1
004520c5 660fb64415d8             movzx      ax, byte ptr [ebp + edx - 0x28]
004520cb b902000000               mov        ecx, 2
004520d0 6bd100                   imul       edx, ecx, 0
004520d3 6683e00f                 and        ax, 0xf
004520d7 b9f0ff0000               mov        ecx, 0xfff0
004520dc 8b7588                   mov        esi, dword ptr [ebp - 0x78]
004520df 66230c16                 and        cx, word ptr [esi + edx]
004520e3 660bc8                   or         cx, ax
004520e6 ba02000000               mov        edx, 2
004520eb 6bc200                   imul       eax, edx, 0
004520ee 8b5588                   mov        edx, dword ptr [ebp - 0x78]
004520f1 66890c02                 mov        word ptr [edx + eax], cx
004520f5 8b4588                   mov        eax, dword ptr [ebp - 0x78]
004520f8 83c002                   add        eax, 2
004520fb 894588                   mov        dword ptr [ebp - 0x78], eax
004520fe e9ccfdffff               jmp        0x451ecf
00452103 e98dfdffff               jmp        0x451e95
00452108 e98e020000               jmp        0x45239b
0045210d 33c9                     xor        ecx, ecx
0045210f 894dcc                   mov        dword ptr [ebp - 0x34], ecx
00452112 894dd0                   mov        dword ptr [ebp - 0x30], ecx
00452115 894dd4                   mov        dword ptr [ebp - 0x2c], ecx
00452118 c78570ffffff00000000     mov        dword ptr [ebp - 0x90], 0
00452122 eb0f                     jmp        0x452133
00452124 8b9570ffffff             mov        edx, dword ptr [ebp - 0x90]
0045212a 83c201                   add        edx, 1
0045212d 899570ffffff             mov        dword ptr [ebp - 0x90], edx
00452133 8b8570ffffff             mov        eax, dword ptr [ebp - 0x90]
00452139 3b45b8                   cmp        eax, dword ptr [ebp - 0x48]
0045213c 0f8359020000             jae        0x45239b
00452142 8b4d90                   mov        ecx, dword ptr [ebp - 0x70]
00452145 0faf8d70ffffff           imul       ecx, dword ptr [ebp - 0x90]
0045214c 034d94                   add        ecx, dword ptr [ebp - 0x6c]
0045214f 894d84                   mov        dword ptr [ebp - 0x7c], ecx
00452152 c78560ffffff00000000     mov        dword ptr [ebp - 0xa0], 0
0045215c eb0f                     jmp        0x45216d
0045215e 8b9560ffffff             mov        edx, dword ptr [ebp - 0xa0]
00452164 83c201                   add        edx, 1
00452167 899560ffffff             mov        dword ptr [ebp - 0xa0], edx
0045216d 8b8560ffffff             mov        eax, dword ptr [ebp - 0xa0]
00452173 3b45b4                   cmp        eax, dword ptr [ebp - 0x4c]
00452176 0f831a020000             jae        0x452396
0045217c b902000000               mov        ecx, 2
00452181 6bd100                   imul       edx, ecx, 0
00452184 8b4584                   mov        eax, dword ptr [ebp - 0x7c]
00452187 668b0c10                 mov        cx, word ptr [eax + edx]
0045218b 66c1e908                 shr        cx, 8
0045218f baff000000               mov        edx, 0xff
00452194 6623ca                   and        cx, dx
00452197 0fb7c1                   movzx      eax, cx
0045219a 85c0                     test       eax, eax
0045219c 0f85e6010000             jne        0x452388
004521a2 b904000000               mov        ecx, 4
004521a7 d1e1                     shl        ecx, 1
004521a9 c7440dcc00000000         mov        dword ptr [ebp + ecx - 0x34], 0
004521b1 ba04000000               mov        edx, 4
004521b6 c1e200                   shl        edx, 0
004521b9 c74415cc00000000         mov        dword ptr [ebp + edx - 0x34], 0
004521c1 b804000000               mov        eax, 4
004521c6 6bc800                   imul       ecx, eax, 0
004521c9 c7440dcc00000000         mov        dword ptr [ebp + ecx - 0x34], 0
004521d1 c745bc00000000           mov        dword ptr [ebp - 0x44], 0
004521d8 83bd60ffffff00           cmp        dword ptr [ebp - 0xa0], 0
004521df 761c                     jbe        0x4521fd
004521e1 8d55bc                   lea        edx, [ebp - 0x44]
004521e4 52                       push       edx
004521e5 b802000000               mov        eax, 2
004521ea 6bc8ff                   imul       ecx, eax, -1
004521ed 034d84                   add        ecx, dword ptr [ebp - 0x7c]
004521f0 51                       push       ecx
004521f1 8d55cc                   lea        edx, [ebp - 0x34]
004521f4 52                       push       edx
004521f5 e8c6f5ffff               call       0x4517c0
004521fa 83c40c                   add        esp, 0xc
004521fd 8b45b4                   mov        eax, dword ptr [ebp - 0x4c]
00452200 83e801                   sub        eax, 1
00452203 398560ffffff             cmp        dword ptr [ebp - 0xa0], eax
00452209 731c                     jae        0x452227
0045220b 8d4dbc                   lea        ecx, [ebp - 0x44]
0045220e 51                       push       ecx
0045220f ba02000000               mov        edx, 2
00452214 c1e200                   shl        edx, 0
00452217 035584                   add        edx, dword ptr [ebp - 0x7c]
0045221a 52                       push       edx
0045221b 8d45cc                   lea        eax, [ebp - 0x34]
0045221e 50                       push       eax
0045221f e89cf5ffff               call       0x4517c0
00452224 83c40c                   add        esp, 0xc
00452227 83bd70ffffff00           cmp        dword ptr [ebp - 0x90], 0
0045222e 7621                     jbe        0x452251
00452230 8d4dbc                   lea        ecx, [ebp - 0x44]
00452233 51                       push       ecx
00452234 8b4590                   mov        eax, dword ptr [ebp - 0x70]
00452237 f7d8                     neg        eax
00452239 99                       cdq        
0045223a 2bc2                     sub        eax, edx
0045223c d1f8                     sar        eax, 1
0045223e 8b5584                   mov        edx, dword ptr [ebp - 0x7c]
00452241 8d0442                   lea        eax, [edx + eax*2]
00452244 50                       push       eax
00452245 8d4dcc                   lea        ecx, [ebp - 0x34]
00452248 51                       push       ecx
00452249 e872f5ffff               call       0x4517c0
0045224e 83c40c                   add        esp, 0xc
00452251 8b55b8                   mov        edx, dword ptr [ebp - 0x48]
00452254 83ea01                   sub        edx, 1
00452257 399570ffffff             cmp        dword ptr [ebp - 0x90], edx
0045225d 731f                     jae        0x45227e
0045225f 8d45bc                   lea        eax, [ebp - 0x44]
00452262 50                       push       eax
00452263 8b4590                   mov        eax, dword ptr [ebp - 0x70]
00452266 99                       cdq        
00452267 2bc2                     sub        eax, edx
00452269 d1f8                     sar        eax, 1
0045226b 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
0045226e 8d1441                   lea        edx, [ecx + eax*2]
00452271 52                       push       edx
00452272 8d45cc                   lea        eax, [ebp - 0x34]
00452275 50                       push       eax
00452276 e845f5ffff               call       0x4517c0
0045227b 83c40c                   add        esp, 0xc
0045227e 837dbc01                 cmp        dword ptr [ebp - 0x44], 1
00452282 7655                     jbe        0x4522d9
00452284 b904000000               mov        ecx, 4
00452289 6bd100                   imul       edx, ecx, 0
0045228c 8b4415cc                 mov        eax, dword ptr [ebp + edx - 0x34]
00452290 33d2                     xor        edx, edx
00452292 f775bc                   div        dword ptr [ebp - 0x44]
00452295 b904000000               mov        ecx, 4
0045229a 6bd100                   imul       edx, ecx, 0
0045229d 894415cc                 mov        dword ptr [ebp + edx - 0x34], eax
004522a1 b804000000               mov        eax, 4
004522a6 c1e000                   shl        eax, 0
004522a9 8b4405cc                 mov        eax, dword ptr [ebp + eax - 0x34]
004522ad 33d2                     xor        edx, edx
004522af f775bc                   div        dword ptr [ebp - 0x44]
004522b2 b904000000               mov        ecx, 4
004522b7 c1e100                   shl        ecx, 0
004522ba 89440dcc                 mov        dword ptr [ebp + ecx - 0x34], eax
004522be ba04000000               mov        edx, 4
004522c3 d1e2                     shl        edx, 1
004522c5 8b4415cc                 mov        eax, dword ptr [ebp + edx - 0x34]
004522c9 33d2                     xor        edx, edx
004522cb f775bc                   div        dword ptr [ebp - 0x44]
004522ce b904000000               mov        ecx, 4
004522d3 d1e1                     shl        ecx, 1
004522d5 89440dcc                 mov        dword ptr [ebp + ecx - 0x34], eax
004522d9 ba04000000               mov        edx, 4
004522de 6bc200                   imul       eax, edx, 0
004522e1 660fb64c05cc             movzx      cx, byte ptr [ebp + eax - 0x34]
004522e7 ba02000000               mov        edx, 2
004522ec 6bc200                   imul       eax, edx, 0
004522ef 6683e107                 and        cx, 7
004522f3 66c1e105                 shl        cx, 5
004522f7 ba1fff0000               mov        edx, 0xff1f
004522fc 8b7584                   mov        esi, dword ptr [ebp - 0x7c]
004522ff 66231406                 and        dx, word ptr [esi + eax]
00452303 660bd1                   or         dx, cx
00452306 b802000000               mov        eax, 2
0045230b 6bc800                   imul       ecx, eax, 0
0045230e 8b4584                   mov        eax, dword ptr [ebp - 0x7c]
00452311 66891408                 mov        word ptr [eax + ecx], dx
00452315 b904000000               mov        ecx, 4
0045231a c1e100                   shl        ecx, 0
0045231d 660fb6540dcc             movzx      dx, byte ptr [ebp + ecx - 0x34]
00452323 b802000000               mov        eax, 2
00452328 6bc800                   imul       ecx, eax, 0
0045232b 6683e207                 and        dx, 7
0045232f 66c1e202                 shl        dx, 2
00452333 b8e3ff0000               mov        eax, 0xffe3
00452338 8b7584                   mov        esi, dword ptr [ebp - 0x7c]
0045233b 6623040e                 and        ax, word ptr [esi + ecx]
0045233f 660bc2                   or         ax, dx
00452342 b902000000               mov        ecx, 2
00452347 6bd100                   imul       edx, ecx, 0
0045234a 8b4d84                   mov        ecx, dword ptr [ebp - 0x7c]
0045234d 66890411                 mov        word ptr [ecx + edx], ax
00452351 ba04000000               mov        edx, 4
00452356 d1e2                     shl        edx, 1
00452358 660fb64415cc             movzx      ax, byte ptr [ebp + edx - 0x34]
0045235e b902000000               mov        ecx, 2
00452363 6bd100                   imul       edx, ecx, 0
00452366 6683e003                 and        ax, 3
0045236a b9fcff0000               mov        ecx, 0xfffc
0045236f 8b7584                   mov        esi, dword ptr [ebp - 0x7c]
00452372 66230c16                 and        cx, word ptr [esi + edx]
00452376 660bc8                   or         cx, ax
00452379 ba02000000               mov        edx, 2
0045237e 6bc200                   imul       eax, edx, 0
00452381 8b5584                   mov        edx, dword ptr [ebp - 0x7c]
00452384 66890c02                 mov        word ptr [edx + eax], cx
00452388 8b4584                   mov        eax, dword ptr [ebp - 0x7c]
0045238b 83c002                   add        eax, 2
0045238e 894584                   mov        dword ptr [ebp - 0x7c], eax
00452391 e9c8fdffff               jmp        0x45215e
00452396 e989fdffff               jmp        0x452124
0045239b 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
0045239e 8b11                     mov        edx, dword ptr [ecx]
004523a0 8b4598                   mov        eax, dword ptr [ebp - 0x68]
004523a3 50                       push       eax
004523a4 8b4a38                   mov        ecx, dword ptr [edx + 0x38]
004523a7 ffd1                     call       ecx
004523a9 8b5598                   mov        edx, dword ptr [ebp - 0x68]
004523ac 8b02                     mov        eax, dword ptr [edx]
004523ae 8b4d98                   mov        ecx, dword ptr [ebp - 0x68]
004523b1 51                       push       ecx
004523b2 8b5008                   mov        edx, dword ptr [eax + 8]
004523b5 ffd2                     call       edx
004523b7 90                       nop        
004523b8 5e                       pop        esi
004523b9 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004523bc 33cd                     xor        ecx, ebp
004523be e82b060f00               call       0x5429ee
004523c3 8be5                     mov        esp, ebp
004523c5 5d                       pop        ebp
004523c6 c20400                   ret        4
004523c9 0f1f00                   nop        dword ptr [eax]
004523cc c21945                   ret        0x4519
004523cf 00ef                     add        bh, ch
004523d1 1b4500                   sbb        eax, dword ptr [ebp]
004523d4 7e1e                     jle        0x4523f4
004523d6 45                       inc        ebp
004523d7 000d2145009b             add        byte ptr [0x9b004521], cl
004523dd 234500                   and        eax, dword ptr [ebp]
004523e0 000404                   add        byte ptr [esp + eax], al
004523e3 0404                     add        al, 4
004523e5 0404                     add        al, 4
004523e7 0404                     add        al, 4
004523e9 0404                     add        al, 4
004523eb 0404                     add        al, 4
004523ed 0404                     add        al, 4
004523ef 0404                     add        al, 4
004523f1 0404                     add        al, 4
004523f3 0404                     add        al, 4
004523f5 000404                   add        byte ptr [esp + eax], al
004523f8 0401                     add        al, 1
004523fa 020404                   add        al, byte ptr [esp + eax]
004523fd 03cc                     add        ecx, esp
004523ff cc                       int3       
