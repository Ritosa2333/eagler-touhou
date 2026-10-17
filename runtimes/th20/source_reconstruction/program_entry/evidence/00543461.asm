; MSVC CRT startup and invocation of WinMain
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
00543461  6a 14                              push       0x14
00543463  68 d8 c0 5a 00                     push       0x5ac0d8
00543468  e8 d3 07 00 00                     call       0x543c40
0054346d  6a 01                              push       1
0054346f  e8 9b f3 ff ff                     call       0x54280f
00543474  59                                 pop        ecx
00543475  84 c0                              test       al, al
00543477  0f 84 4d 01 00 00                  je         0x5435ca
0054347d  32 db                              xor        bl, bl
0054347f  88 5d e7                           mov        byte ptr [ebp - 0x19], bl
00543482  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
00543489  e8 4f f3 ff ff                     call       0x5427dd
0054348e  88 45 dc                           mov        byte ptr [ebp - 0x24], al
00543491  a1 44 4f 5e 00                     mov        eax, dword ptr [0x5e4f44]
00543496  33 c9                              xor        ecx, ecx
00543498  41                                 inc        ecx
00543499  3b c1                              cmp        eax, ecx
0054349b  0f 84 29 01 00 00                  je         0x5435ca
005434a1  85 c0                              test       eax, eax
005434a3  75 49                              jne        0x5434ee
005434a5  89 0d 44 4f 5e 00                  mov        dword ptr [0x5e4f44], ecx
005434ab  68 3c c4 56 00                     push       0x56c43c
005434b0  68 18 c4 56 00                     push       0x56c418
005434b5  e8 6f 25 01 00                     call       0x555a29
005434ba  59                                 pop        ecx
005434bb  59                                 pop        ecx
005434bc  85 c0                              test       eax, eax
005434be  74 11                              je         0x5434d1
005434c0  c7 45 fc fe ff ff ff               mov        dword ptr [ebp - 4], 0xfffffffe
005434c7  b8 ff 00 00 00                     mov        eax, 0xff
005434cc  e9 e9 00 00 00                     jmp        0x5435ba
005434d1  68 14 c4 56 00                     push       0x56c414
005434d6  68 84 c3 56 00                     push       0x56c384
005434db  e8 1e 25 01 00                     call       0x5559fe
005434e0  59                                 pop        ecx
005434e1  59                                 pop        ecx
005434e2  c7 05 44 4f 5e 00 02 00 00 00      mov        dword ptr [0x5e4f44], 2
005434ec  eb 05                              jmp        0x5434f3
005434ee  8a d9                              mov        bl, cl
005434f0  88 5d e7                           mov        byte ptr [ebp - 0x19], bl
005434f3  ff 75 dc                           push       dword ptr [ebp - 0x24]
005434f6  e8 6b f4 ff ff                     call       0x542966
005434fb  59                                 pop        ecx
005434fc  e8 7b 08 00 00                     call       0x543d7c
00543501  8b f0                              mov        esi, eax
00543503  33 ff                              xor        edi, edi
00543505  39 3e                              cmp        dword ptr [esi], edi
00543507  74 1b                              je         0x543524
00543509  56                                 push       esi
0054350a  e8 c0 f3 ff ff                     call       0x5428cf
0054350f  59                                 pop        ecx
00543510  84 c0                              test       al, al
00543512  74 10                              je         0x543524
00543514  8b 36                              mov        esi, dword ptr [esi]
00543516  57                                 push       edi
00543517  6a 02                              push       2
00543519  57                                 push       edi
0054351a  8b ce                              mov        ecx, esi
0054351c  ff 15 7c c3 56 00                  call       dword ptr [0x56c37c]
00543522  ff d6                              call       esi
00543524  e8 59 08 00 00                     call       0x543d82
00543529  8b f0                              mov        esi, eax
0054352b  39 3e                              cmp        dword ptr [esi], edi
0054352d  74 13                              je         0x543542
0054352f  56                                 push       esi
00543530  e8 9a f3 ff ff                     call       0x5428cf
00543535  59                                 pop        ecx
00543536  84 c0                              test       al, al
00543538  74 08                              je         0x543542
0054353a  ff 36                              push       dword ptr [esi]
0054353c  e8 c2 f8 00 00                     call       0x552e03
00543541  59                                 pop        ecx
00543542  e8 07 06 00 00                     call       0x543b4e
00543547  0f b7 f0                           movzx      esi, ax
0054354a  e8 57 24 01 00                     call       0x5559a6
0054354f  56                                 push       esi
00543550  50                                 push       eax
00543551  57                                 push       edi
00543552  68 00 00 40 00                     push       0x400000
00543557  e8 74 b2 ed ff                     call       0x41e7d0
0054355c  8b f0                              mov        esi, eax
0054355e  e8 23 06 00 00                     call       0x543b86
00543563  84 c0                              test       al, al
00543565  74 6a                              je         0x5435d1
00543567  84 db                              test       bl, bl
00543569  75 05                              jne        0x543570
0054356b  e8 6e f8 00 00                     call       0x552dde
00543570  57                                 push       edi
00543571  6a 01                              push       1
00543573  e8 0b f4 ff ff                     call       0x542983
00543578  59                                 pop        ecx
00543579  59                                 pop        ecx
0054357a  c7 45 fc fe ff ff ff               mov        dword ptr [ebp - 4], 0xfffffffe
00543581  8b c6                              mov        eax, esi
00543583  eb 35                              jmp        0x5435ba
005435ba  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
005435bd  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
005435c4  59                                 pop        ecx
005435c5  5f                                 pop        edi
005435c6  5e                                 pop        esi
005435c7  5b                                 pop        ebx
005435c8  c9                                 leave
005435c9  c3                                 ret
005435ca  6a 07                              push       7
005435cc  e8 68 04 00 00                     call       0x543a39
005435d1  56                                 push       esi
005435d2  e8 52 f8 00 00                     call       0x552e29
005435d7  ff 75 e0                           push       dword ptr [ebp - 0x20]
005435da  e8 0e f8 00 00                     call       0x552ded
