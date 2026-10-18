00470920  55                                 push       ebp
00470921  8b ec                              mov        ebp, esp
00470923  51                                 push       ecx
00470924  8b 45 18                           mov        eax, dword ptr [ebp + 0x18]
00470927  03 45 20                           add        eax, dword ptr [ebp + 0x20]
0047092a  39 45 08                           cmp        dword ptr [ebp + 8], eax
0047092d  7f 2a                              jg         0x470959
0047092f  8b 4d 08                           mov        ecx, dword ptr [ebp + 8]
00470932  03 4d 10                           add        ecx, dword ptr [ebp + 0x10]
00470935  3b 4d 18                           cmp        ecx, dword ptr [ebp + 0x18]
00470938  7c 1f                              jl         0x470959
0047093a  8b 55 1c                           mov        edx, dword ptr [ebp + 0x1c]
0047093d  03 55 24                           add        edx, dword ptr [ebp + 0x24]
00470940  39 55 0c                           cmp        dword ptr [ebp + 0xc], edx
00470943  7f 14                              jg         0x470959
00470945  8b 45 0c                           mov        eax, dword ptr [ebp + 0xc]
00470948  03 45 14                           add        eax, dword ptr [ebp + 0x14]
0047094b  3b 45 1c                           cmp        eax, dword ptr [ebp + 0x1c]
0047094e  7c 09                              jl         0x470959
00470950  c7 45 fc 01 00 00 00               mov        dword ptr [ebp - 4], 1
00470957  eb 07                              jmp        0x470960
00470959  c7 45 fc 00 00 00 00               mov        dword ptr [ebp - 4], 0
00470960  0f b6 45 fc                        movzx      eax, byte ptr [ebp - 4]
00470964  8b e5                              mov        esp, ebp
00470966  5d                                 pop        ebp
00470967  c3                                 ret        
