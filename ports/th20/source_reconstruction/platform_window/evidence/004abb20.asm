004abb20  55                                 push       ebp
004abb21  8b ec                              mov        ebp, esp
004abb23  83 ec 0c                           sub        esp, 0xc
004abb26  c6 45 ff 01                        mov        byte ptr [ebp - 1], 1
004abb2a  c6 45 fe 00                        mov        byte ptr [ebp - 2], 0
004abb2e  8b 4d 0c                           mov        ecx, dword ptr [ebp + 0xc]
004abb31  e8 5a 05 fa ff                     call       0x44c090
004abb36  6a 00                              push       0
004abb38  68 80 96 98 00                     push       0x989680
004abb3d  52                                 push       edx
004abb3e  50                                 push       eax
004abb3f  e8 0c 75 09 00                     call       0x543050
004abb44  89 45 f4                           mov        dword ptr [ebp - 0xc], eax
004abb47  89 55 f8                           mov        dword ptr [ebp - 8], edx
004abb4a  8d 45 f4                           lea        eax, [ebp - 0xc]
004abb4d  50                                 push       eax
004abb4e  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
004abb51  e8 4a b2 f9 ff                     call       0x446da0
004abb56  90                                 nop        
004abb57  8b 45 08                           mov        eax, dword ptr [ebp + 8]
004abb5a  8b e5                              mov        esp, ebp
004abb5c  5d                                 pop        ebp
004abb5d  c3                                 ret        
