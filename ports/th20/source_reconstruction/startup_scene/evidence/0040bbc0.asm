0040bbc0  55                                 push       ebp
0040bbc1  8b ec                              mov        ebp, esp
0040bbc3  51                                 push       ecx
0040bbc4  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0040bbc7  6b 45 08 30                        imul       eax, dword ptr [ebp + 8], 0x30
0040bbcb  03 45 fc                           add        eax, dword ptr [ebp - 4]
0040bbce  8b e5                              mov        esp, ebp
0040bbd0  5d                                 pop        ebp
0040bbd1  c2 04 00                           ret        4
