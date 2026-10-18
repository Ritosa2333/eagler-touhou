; MSVC security cookie initialization
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
00543cd2  8b 0d c0 25 5b 00                  mov        ecx, dword ptr [0x5b25c0]
00543cd8  56                                 push       esi
00543cd9  57                                 push       edi
00543cda  bf 4e e6 40 bb                     mov        edi, 0xbb40e64e
00543cdf  be 00 00 ff ff                     mov        esi, 0xffff0000
00543ce4  3b cf                              cmp        ecx, edi
00543ce6  74 04                              je         0x543cec
00543ce8  85 ce                              test       esi, ecx
00543cea  75 26                              jne        0x543d12
00543cec  e8 94 ff ff ff                     call       0x543c85
00543cf1  8b c8                              mov        ecx, eax
00543cf3  3b cf                              cmp        ecx, edi
00543cf5  75 07                              jne        0x543cfe
00543cf7  b9 4f e6 40 bb                     mov        ecx, 0xbb40e64f
00543cfc  eb 0e                              jmp        0x543d0c
00543cfe  85 ce                              test       esi, ecx
00543d00  75 0a                              jne        0x543d0c
00543d02  0d 11 47 00 00                     or         eax, 0x4711
00543d07  c1 e0 10                           shl        eax, 0x10
00543d0a  0b c8                              or         ecx, eax
00543d0c  89 0d c0 25 5b 00                  mov        dword ptr [0x5b25c0], ecx
00543d12  f7 d1                              not        ecx
00543d14  5f                                 pop        edi
00543d15  89 0d 00 26 5b 00                  mov        dword ptr [0x5b2600], ecx
00543d1b  5e                                 pop        esi
00543d1c  c3                                 ret
