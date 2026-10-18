00438940 55                       push       ebp
00438941 8bec                     mov        ebp, esp
00438943 51                       push       ecx
00438944 894dfc                   mov        dword ptr [ebp - 4], ecx
00438947 8b45fc                   mov        eax, dword ptr [ebp - 4]
0043894a 8b4854                   mov        ecx, dword ptr [eax + 0x54]
0043894d 8b550c                   mov        edx, dword ptr [ebp + 0xc]
00438950 833c9100                 cmp        dword ptr [ecx + edx*4], 0
00438954 751b                     jne        0x438971
00438956 68e4050000               push       0x5e4
0043895b 6a00                     push       0
0043895d 8b4508                   mov        eax, dword ptr [ebp + 8]
00438960 50                       push       eax
00438961 e8fabd1000               call       0x544760
00438966 83c40c                   add        esp, 0xc
00438969 83c8ff                   or         eax, 0xffffffff
0043896c e9ab000000               jmp        0x438a1c
00438971 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00438974 e85710ffff               call       0x4299d0
00438979 8b4d08                   mov        ecx, dword ptr [ebp + 8]
0043897c e8af14ffff               call       0x429e30
00438981 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00438984 668b550c                 mov        dx, word ptr [ebp + 0xc]
00438988 66899140040000           mov        word ptr [ecx + 0x440], dx
0043898f 8b4508                   mov        eax, dword ptr [ebp + 8]
00438992 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
00438995 8b11                     mov        edx, dword ptr [ecx]
00438997 895018                   mov        dword ptr [eax + 0x18], edx
0043899a 8b4508                   mov        eax, dword ptr [ebp + 8]
0043899d 8b4dfc                   mov        ecx, dword ptr [ebp - 4]
004389a0 8b11                     mov        edx, dword ptr [ecx]
004389a2 89501c                   mov        dword ptr [eax + 0x1c], edx
004389a5 8b4508                   mov        eax, dword ptr [ebp + 8]
004389a8 8a88a0040000             mov        cl, byte ptr [eax + 0x4a0]
004389ae 80e13f                   and        cl, 0x3f
004389b1 8b5508                   mov        edx, dword ptr [ebp + 8]
004389b4 888aa0040000             mov        byte ptr [edx + 0x4a0], cl
004389ba 8b4508                   mov        eax, dword ptr [ebp + 8]
004389bd 8a88a0040000             mov        cl, byte ptr [eax + 0x4a0]
004389c3 80e1fc                   and        cl, 0xfc
004389c6 80c902                   or         cl, 2
004389c9 8b5508                   mov        edx, dword ptr [ebp + 8]
004389cc 888aa0040000             mov        byte ptr [edx + 0x4a0], cl
004389d2 8b4508                   mov        eax, dword ptr [ebp + 8]
004389d5 8b4d0c                   mov        ecx, dword ptr [ebp + 0xc]
004389d8 894824                   mov        dword ptr [eax + 0x24], ecx
004389db 8b5508                   mov        edx, dword ptr [ebp + 8]
004389de c7422800000000           mov        dword ptr [edx + 0x28], 0
004389e5 6a00                     push       0
004389e7 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004389ea 81c1d8040000             add        ecx, 0x4d8
004389f0 e82babfeff               call       0x423520
004389f5 6a00                     push       0
004389f7 8b4d08                   mov        ecx, dword ptr [ebp + 8]
004389fa 81c1c8040000             add        ecx, 0x4c8
00438a00 e81babfeff               call       0x423520
00438a05 8b4508                   mov        eax, dword ptr [ebp + 8]
00438a08 8a889a040000             mov        cl, byte ptr [eax + 0x49a]
00438a0e 80e1fe                   and        cl, 0xfe
00438a11 8b5508                   mov        edx, dword ptr [ebp + 8]
00438a14 888a9a040000             mov        byte ptr [edx + 0x49a], cl
00438a1a 33c0                     xor        eax, eax
00438a1c 8be5                     mov        esp, ebp
00438a1e 5d                       pop        ebp
00438a1f c20800                   ret        8
00438a22 cc                       int3       
00438a23 cc                       int3       
00438a24 cc                       int3       
00438a25 cc                       int3       
00438a26 cc                       int3       
00438a27 cc                       int3       
00438a28 cc                       int3       
00438a29 cc                       int3       
00438a2a cc                       int3       
00438a2b cc                       int3       
00438a2c cc                       int3       
00438a2d cc                       int3       
00438a2e cc                       int3       
00438a2f cc                       int3       
