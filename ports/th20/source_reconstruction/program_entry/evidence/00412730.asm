; Direct3D device getter
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
00412730  55                                 push       ebp
00412731  8b ec                              mov        ebp, esp
00412733  51                                 push       ecx
00412734  89 4d fc                           mov        dword ptr [ebp - 4], ecx
00412737  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
0041273a  8b 40 08                           mov        eax, dword ptr [eax + 8]
0041273d  8b e5                              mov        esp, ebp
0041273f  5d                                 pop        ebp
00412740  c3                                 ret
