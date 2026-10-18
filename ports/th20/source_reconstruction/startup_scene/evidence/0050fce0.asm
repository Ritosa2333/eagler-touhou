0050fce0  55                                 push       ebp
0050fce1  8b ec                              mov        ebp, esp
0050fce3  68 70 37 57 00                     push       0x573770 ; string='D:\\cygwin\\home\\zun\\prog\\th20\\src\\game\\score.cpp:118 ScoreInf'
0050fce8  8b 0d 94 88 5b 00                  mov        ecx, dword ptr [0x5b8894]
0050fcee  e8 cd b0 ff ff                     call       0x50adc0
0050fcf3  a3 08 61 5c 00                     mov        dword ptr [0x5c6108], eax
0050fcf8  5d                                 pop        ebp
0050fcf9  c3                                 ret        
