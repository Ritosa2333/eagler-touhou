004ac160  55                                 push       ebp
004ac161  8b ec                              mov        ebp, esp
004ac163  51                                 push       ecx
004ac164  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004ac167  32 c0                              xor        al, al
004ac169  8b e5                              mov        esp, ebp
004ac16b  5d                                 pop        ebp
004ac16c  c3                                 ret        
