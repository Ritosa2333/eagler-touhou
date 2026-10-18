0054a4e0 8bff                     mov        edi, edi
0054a4e2 55                       push       ebp
0054a4e3 8bec                     mov        ebp, esp
0054a4e5 53                       push       ebx
0054a4e6 8b5d08                   mov        ebx, dword ptr [ebp + 8]
0054a4e9 81e300030000             and        ebx, 0x300
0054a4ef 3b5d08                   cmp        ebx, dword ptr [ebp + 8]
0054a4f2 753e                     jne        0x54a532
0054a4f4 56                       push       esi
0054a4f5 57                       push       edi
0054a4f6 e8a5f50000               call       0x559aa0
0054a4fb 6800030000               push       0x300
0054a500 8bf8                     mov        edi, eax
0054a502 e829f50000               call       0x559a30
0054a507 8bf0                     mov        esi, eax
0054a509 53                       push       ebx
0054a50a f7d6                     not        esi
0054a50c e81ff50000               call       0x559a30
0054a511 23f7                     and        esi, edi
0054a513 0bf0                     or         esi, eax
0054a515 56                       push       esi
0054a516 e815f80000               call       0x559d30
0054a51b 83c40c                   add        esp, 0xc
0054a51e e8fdf40000               call       0x559a20
0054a523 33c9                     xor        ecx, ecx
0054a525 3b4508                   cmp        eax, dword ptr [ebp + 8]
0054a528 5f                       pop        edi
0054a529 0f95c1                   setne      cl
0054a52c 5e                       pop        esi
0054a52d 8bc1                     mov        eax, ecx
0054a52f 5b                       pop        ebx
0054a530 5d                       pop        ebp
0054a531 c3                       ret        
0054a532 b801000000               mov        eax, 1
0054a537 5b                       pop        ebx
0054a538 5d                       pop        ebp
0054a539 c3                       ret        
