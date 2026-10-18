00470680  55                                 push       ebp
00470681  8b ec                              mov        ebp, esp
00470683  6a ff                              push       -1
00470685  68 b5 8e 56 00                     push       0x568eb5
0047068a  64 a1 00 00 00 00                  mov        eax, dword ptr fs:[0]
00470690  50                                 push       eax
00470691  83 ec 38                           sub        esp, 0x38
00470694  a1 c0 25 5b 00                     mov        eax, dword ptr [0x5b25c0]
00470699  33 c5                              xor        eax, ebp
0047069b  89 45 f0                           mov        dword ptr [ebp - 0x10], eax
0047069e  56                                 push       esi
0047069f  50                                 push       eax
004706a0  8d 45 f4                           lea        eax, [ebp - 0xc]
004706a3  64 a3 00 00 00 00                  mov        dword ptr fs:[0], eax
004706a9  89 4d d4                           mov        dword ptr [ebp - 0x2c], ecx
004706ac  6a 12                              push       0x12
004706ae  b9 40 02 5c 00                     mov        ecx, 0x5c0240
004706b3  e8 58 bc f9 ff                     call       0x40c310
004706b8  50                                 push       eax
004706b9  8d 4d e4                           lea        ecx, [ebp - 0x1c]
004706bc  e8 6f b0 f9 ff                     call       0x40b730
004706c1  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
004706c8  c7 45 d8 00 00 00 00               mov        dword ptr [ebp - 0x28], 0
004706cf  c7 45 dc 00 00 00 00               mov        dword ptr [ebp - 0x24], 0
004706d6  8b 45 d4                           mov        eax, dword ptr [ebp - 0x2c]
004706d9  05 0c a2 01 00                     add        eax, 0x1a20c
004706de  89 45 d0                           mov        dword ptr [ebp - 0x30], eax
004706e1  6a 08                              push       8
004706e3  8d 4d e8                           lea        ecx, [ebp - 0x18]
004706e6  e8 95 b9 f9 ff                     call       0x40c080
004706eb  8d 4d e8                           lea        ecx, [ebp - 0x18]
004706ee  51                                 push       ecx
004706ef  8b 4d d0                           mov        ecx, dword ptr [ebp - 0x30]
004706f2  e8 89 1b fa ff                     call       0x412280
004706f7  c6 45 fc 01                        mov        byte ptr [ebp - 4], 1
004706fb  8b 4d d0                           mov        ecx, dword ptr [ebp - 0x30]
004706fe  e8 3d 1e fa ff                     call       0x412540
00470703  89 45 cc                           mov        dword ptr [ebp - 0x34], eax
00470706  eb 09                              jmp        0x470711
00470708  8d 4d e8                           lea        ecx, [ebp - 0x18]
0047070b  e8 20 15 fa ff                     call       0x411c30
00470710  90                                 nop        
00470711  8b 55 cc                           mov        edx, dword ptr [ebp - 0x34]
00470714  52                                 push       edx
00470715  8d 4d e8                           lea        ecx, [ebp - 0x18]
00470718  e8 93 14 fa ff                     call       0x411bb0
0047071d  0f b6 c0                           movzx      eax, al
00470720  85 c0                              test       eax, eax
00470722  0f 84 65 01 00 00                  je         0x47088d
00470728  8d 4d e8                           lea        ecx, [ebp - 0x18]
0047072b  e8 d0 bb f9 ff                     call       0x40c300
00470730  89 45 e0                           mov        dword ptr [ebp - 0x20], eax
00470733  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
00470736  e8 25 ea f9 ff                     call       0x40f160
0047073b  8b 08                              mov        ecx, dword ptr [eax]
0047073d  8b 91 48 06 00 00                  mov        edx, dword ptr [ecx + 0x648]
00470743  83 ea 01                           sub        edx, 1
00470746  89 55 c8                           mov        dword ptr [ebp - 0x38], edx
00470749  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
0047074c  e8 0f ea f9 ff                     call       0x40f160
00470751  8b 00                              mov        eax, dword ptr [eax]
00470753  8b 88 44 06 00 00                  mov        ecx, dword ptr [eax + 0x644]
00470759  83 e9 01                           sub        ecx, 1
0047075c  89 4d c4                           mov        dword ptr [ebp - 0x3c], ecx
0047075f  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
00470762  e8 f9 e9 f9 ff                     call       0x40f160
00470767  8b 10                              mov        edx, dword ptr [eax]
00470769  8b 82 40 06 00 00                  mov        eax, dword ptr [edx + 0x640]
0047076f  89 45 c0                           mov        dword ptr [ebp - 0x40], eax
00470772  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
00470775  e8 e6 e9 f9 ff                     call       0x40f160
0047077a  8b 08                              mov        ecx, dword ptr [eax]
0047077c  8b 91 3c 06 00 00                  mov        edx, dword ptr [ecx + 0x63c]
00470782  89 55 bc                           mov        dword ptr [ebp - 0x44], edx
00470785  8b 45 c8                           mov        eax, dword ptr [ebp - 0x38]
00470788  50                                 push       eax
00470789  8b 4d c4                           mov        ecx, dword ptr [ebp - 0x3c]
0047078c  51                                 push       ecx
0047078d  8b 55 c0                           mov        edx, dword ptr [ebp - 0x40]
00470790  52                                 push       edx
00470791  8b 45 bc                           mov        eax, dword ptr [ebp - 0x44]
00470794  50                                 push       eax
00470795  8b 4d 10                           mov        ecx, dword ptr [ebp + 0x10]
00470798  51                                 push       ecx
00470799  8b 55 0c                           mov        edx, dword ptr [ebp + 0xc]
0047079c  52                                 push       edx
0047079d  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
004707a0  50                                 push       eax
004707a1  8b 4d d8                           mov        ecx, dword ptr [ebp - 0x28]
004707a4  51                                 push       ecx
004707a5  e8 76 01 00 00                     call       0x470920
004707aa  83 c4 20                           add        esp, 0x20
004707ad  0f b6 d0                           movzx      edx, al
004707b0  85 d2                              test       edx, edx
004707b2  0f 84 d0 00 00 00                  je         0x470888
004707b8  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
004707bb  e8 a0 e9 f9 ff                     call       0x40f160
004707c0  8b 30                              mov        esi, dword ptr [eax]
004707c2  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
004707c5  e8 96 e9 f9 ff                     call       0x40f160
004707ca  8b 00                              mov        eax, dword ptr [eax]
004707cc  8b 8e 3c 06 00 00                  mov        ecx, dword ptr [esi + 0x63c]
004707d2  03 88 44 06 00 00                  add        ecx, dword ptr [eax + 0x644]
004707d8  03 4d 0c                           add        ecx, dword ptr [ebp + 0xc]
004707db  8b 55 d4                           mov        edx, dword ptr [ebp - 0x2c]
004707de  3b 8a 00 a2 01 00                  cmp        ecx, dword ptr [edx + 0x1a200]
004707e4  7d 25                              jge        0x47080b
004707e6  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
004707e9  e8 72 e9 f9 ff                     call       0x40f160
004707ee  8b 30                              mov        esi, dword ptr [eax]
004707f0  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
004707f3  e8 68 e9 f9 ff                     call       0x40f160
004707f8  8b 00                              mov        eax, dword ptr [eax]
004707fa  8b 8e 3c 06 00 00                  mov        ecx, dword ptr [esi + 0x63c]
00470800  03 88 44 06 00 00                  add        ecx, dword ptr [eax + 0x644]
00470806  89 4d d8                           mov        dword ptr [ebp - 0x28], ecx
00470809  eb 6b                              jmp        0x470876
0047080b  c7 45 d8 00 00 00 00               mov        dword ptr [ebp - 0x28], 0
00470812  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
00470815  e8 46 e9 f9 ff                     call       0x40f160
0047081a  8b 30                              mov        esi, dword ptr [eax]
0047081c  8b 4d e0                           mov        ecx, dword ptr [ebp - 0x20]
0047081f  e8 3c e9 f9 ff                     call       0x40f160
00470824  8b 10                              mov        edx, dword ptr [eax]
00470826  8b 86 40 06 00 00                  mov        eax, dword ptr [esi + 0x640]
0047082c  8b 8a 48 06 00 00                  mov        ecx, dword ptr [edx + 0x648]
00470832  8d 54 08 01                        lea        edx, [eax + ecx + 1]
00470836  89 55 dc                           mov        dword ptr [ebp - 0x24], edx
00470839  8b 45 dc                           mov        eax, dword ptr [ebp - 0x24]
0047083c  03 45 10                           add        eax, dword ptr [ebp + 0x10]
0047083f  8b 4d d4                           mov        ecx, dword ptr [ebp - 0x2c]
00470842  3b 81 04 a2 01 00                  cmp        eax, dword ptr [ecx + 0x1a204]
00470848  7e 2c                              jle        0x470876
0047084a  6a ff                              push       -1
0047084c  6a ff                              push       -1
0047084e  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00470851  e8 aa d5 f9 ff                     call       0x40de00
00470856  c6 45 fc 00                        mov        byte ptr [ebp - 4], 0
0047085a  8d 4d e8                           lea        ecx, [ebp - 0x18]
0047085d  e8 9e 12 fa ff                     call       0x411b00
00470862  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
00470869  8d 4d e4                           lea        ecx, [ebp - 0x1c]
0047086c  e8 8f b0 f9 ff                     call       0x40b900
00470871  8b 45 08                           mov        eax, dword ptr [ebp + 8]
00470874  eb 46                              jmp        0x4708bc
00470876  c6 45 fc 00                        mov        byte ptr [ebp - 4], 0
0047087a  8d 4d e8                           lea        ecx, [ebp - 0x18]
0047087d  e8 7e 12 fa ff                     call       0x411b00
00470882  90                                 nop        
00470883  e9 4e fe ff ff                     jmp        0x4706d6
00470888  e9 7b fe ff ff                     jmp        0x470708
0047088d  c6 45 fc 00                        mov        byte ptr [ebp - 4], 0
00470891  8d 4d e8                           lea        ecx, [ebp - 0x18]
00470894  e8 67 12 fa ff                     call       0x411b00
00470899  90                                 nop        
0047089a  8b 55 dc                           mov        edx, dword ptr [ebp - 0x24]
0047089d  52                                 push       edx
0047089e  8b 45 d8                           mov        eax, dword ptr [ebp - 0x28]
004708a1  50                                 push       eax
004708a2  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004708a5  e8 56 d5 f9 ff                     call       0x40de00
004708aa  c7 45 fc ff ff ff ff               mov        dword ptr [ebp - 4], 0xffffffff
004708b1  8d 4d e4                           lea        ecx, [ebp - 0x1c]
004708b4  e8 47 b0 f9 ff                     call       0x40b900
004708b9  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004708bc  8b 4d f4                           mov        ecx, dword ptr [ebp - 0xc]
004708bf  64 89 0d 00 00 00 00               mov        dword ptr fs:[0], ecx
004708c6  59                                 pop        ecx
004708c7  5e                                 pop        esi
004708c8  8b 4d f0                           mov        ecx, dword ptr [ebp - 0x10]
004708cb  33 cd                              xor        ecx, ebp
004708cd  e8 1c 21 0d 00                     call       0x5429ee
004708d2  8b e5                              mov        esp, ebp
004708d4  5d                                 pop        ebp
004708d5  c2 0c 00                           ret        0xc
