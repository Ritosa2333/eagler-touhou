0041db60  55                                 push       ebp
0041db61  8b ec                              mov        ebp, esp
0041db63  51                                 push       ecx
0041db64  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041db67  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041db6a  c6 80 0d 0e 00 06 ff               mov        byte ptr [eax + 0x6000e0d], 0xff
0041db71  8b e5                              mov        esp, ebp
0041db73  5d                                 pop        ebp
0041db74  c3                                 ret        
