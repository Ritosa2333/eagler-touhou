00511900 55                       push       ebp
00511901 8bec                     mov        ebp, esp
00511903 6a00                     push       0
00511905 8b4d08                   mov        ecx, dword ptr [ebp + 8]
00511908 e883070000               call       0x512090
0051190d 90                       nop        
0051190e 5d                       pop        ebp
0051190f c3                       ret        
