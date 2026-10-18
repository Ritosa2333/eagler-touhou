0041db40  55                                 push       ebp
0041db41  8b ec                              mov        ebp, esp
0041db43  51                                 push       ecx
0041db44  89 4d fc                           mov        dword ptr [ebp - 4], ecx
0041db47  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041db4a  c6 80 0c 0e 00 06 0b               mov        byte ptr [eax + 0x6000e0c], 0xb
0041db51  8b e5                              mov        esp, ebp
0041db53  5d                                 pop        ebp
0041db54  c3                                 ret        
