004bd460  55                                 push       ebp
004bd461  8b ec                              mov        ebp, esp
004bd463  51                                 push       ecx
004bd464  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004bd467  83 7d 08 00                        cmp        dword ptr [ebp + 8], 0
004bd46b  7c 12                              jl         0x4bd47f
004bd46d  83 7d 08 02                        cmp        dword ptr [ebp + 8], 2
004bd471  7d 0c                              jge        0x4bd47f
004bd473  83 7d 0c 00                        cmp        dword ptr [ebp + 0xc], 0
004bd477  7c 06                              jl         0x4bd47f
004bd479  83 7d 0c 09                        cmp        dword ptr [ebp + 0xc], 9
004bd47d  7c 04                              jl         0x4bd483
004bd47f  33 c0                              xor        eax, eax
004bd481  eb 17                              jmp        0x4bd49a
004bd483  69 45 08 28 52 04 00               imul       eax, dword ptr [ebp + 8], 0x45228
004bd48a  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004bd48d  8d 44 01 10                        lea        eax, [ecx + eax + 0x10]
004bd491  69 55 0c e8 7a 00 00               imul       edx, dword ptr [ebp + 0xc], 0x7ae8
004bd498  03 c2                              add        eax, edx
004bd49a  8b e5                              mov        esp, ebp
004bd49c  5d                                 pop        ebp
004bd49d  c2 08 00                           ret        8
