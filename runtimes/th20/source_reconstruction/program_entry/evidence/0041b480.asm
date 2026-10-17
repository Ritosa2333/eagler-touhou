; SetForegroundWindow wrapper
; source SHA-256 a274b45fe6ec53511718bb328c2ff169a74e67f95d1b0c74d97d348b955a0897
0041b480  55                                 push       ebp
0041b481  8b ec                              mov        ebp, esp
0041b483  8b 45 08                           mov        eax, dword ptr [ebp + 8]
0041b486  50                                 push       eax
0041b487  ff 15 50 c2 56 00                  call       dword ptr [0x56c250] ; USER32.dll!SetForegroundWindow
0041b48d  90                                 nop
0041b48e  5d                                 pop        ebp
0041b48f  c3                                 ret
