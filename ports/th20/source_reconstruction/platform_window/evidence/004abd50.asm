004abd50  55                                 push       ebp
004abd51  8b ec                              mov        ebp, esp
004abd53  83 ec 18                           sub        esp, 0x18
004abd56  89 4d fc                           mov        dword ptr [ebp - 4], ecx
004abd59  b9 58 67 5b 00                     mov        ecx, 0x5b6758
004abd5e  e8 ad 0d f7 ff                     call       0x41cb10
004abd63  dd 5d f0                           fstp       qword ptr [ebp - 0x10]
004abd66  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004abd69  f2 0f 10 40 18                     movsd      xmm0, qword ptr [eax + 0x18]
004abd6e  66 0f 2f 45 f0                     comisd     xmm0, xmmword ptr [ebp - 0x10]
004abd73  76 0d                              jbe        0x4abd82
004abd75  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004abd78  f2 0f 10 45 f0                     movsd      xmm0, qword ptr [ebp - 0x10]
004abd7d  f2 0f 11 41 18                     movsd      qword ptr [ecx + 0x18], xmm0
004abd82  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004abd85  f2 0f 10 45 f0                     movsd      xmm0, qword ptr [ebp - 0x10]
004abd8a  f2 0f 5c 42 18                     subsd      xmm0, qword ptr [edx + 0x18]
004abd8f  66 0f 2f 05 68 c4 56 00            comisd     xmm0, xmmword ptr [0x56c468]
004abd97  0f 82 40 01 00 00                  jb         0x4abedd
004abd9d  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004abda0  f2 0f 10 45 f0                     movsd      xmm0, qword ptr [ebp - 0x10]
004abda5  f2 0f 5c 40 18                     subsd      xmm0, qword ptr [eax + 0x18]
004abdaa  f2 0f 11 45 e8                     movsd      qword ptr [ebp - 0x18], xmm0
004abdaf  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004abdb2  f2 0f 10 41 18                     movsd      xmm0, qword ptr [ecx + 0x18]
004abdb7  f2 0f 58 45 e8                     addsd      xmm0, qword ptr [ebp - 0x18]
004abdbc  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004abdbf  f2 0f 11 42 18                     movsd      qword ptr [edx + 0x18], xmm0
004abdc4  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004abdc7  8b 48 24                           mov        ecx, dword ptr [eax + 0x24]
004abdca  89 4d f8                           mov        dword ptr [ebp - 8], ecx
004abdcd  f2 0f 2a 45 f8                     cvtsi2sd   xmm0, dword ptr [ebp - 8]
004abdd2  8b 55 f8                           mov        edx, dword ptr [ebp - 8]
004abdd5  c1 ea 1f                           shr        edx, 0x1f
004abdd8  f2 0f 58 04 d5 d0 cd 56 00         addsd      xmm0, qword ptr [edx*8 + 0x56cdd0]
004abde1  f2 0f 5e 45 e8                     divsd      xmm0, qword ptr [ebp - 0x18]
004abde6  f2 0f 5a c0                        cvtsd2ss   xmm0, xmm0
004abdea  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004abded  f3 0f 11 40 38                     movss      dword ptr [eax + 0x38], xmm0
004abdf2  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004abdf5  f3 0f 10 41 38                     movss      xmm0, dword ptr [ecx + 0x38]
004abdfa  0f 2f 05 18 06 57 00               comiss     xmm0, dword ptr [0x570618]
004abe01  76 11                              jbe        0x4abe14
004abe03  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004abe06  8b 42 20                           mov        eax, dword ptr [edx + 0x20]
004abe09  83 c0 01                           add        eax, 1
004abe0c  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004abe0f  89 41 20                           mov        dword ptr [ecx + 0x20], eax
004abe12  eb 0a                              jmp        0x4abe1e
004abe14  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004abe17  c7 42 20 00 00 00 00               mov        dword ptr [edx + 0x20], 0
004abe1e  83 3d 28 a8 5b 00 00               cmp        dword ptr [0x5ba828], 0
004abe25  0f 84 82 00 00 00                  je         0x4abead
004abe2b  a1 28 a8 5b 00                     mov        eax, dword ptr [0x5ba828]
004abe30  8b 88 e8 00 00 00                  mov        ecx, dword ptr [eax + 0xe8]
004abe36  c1 e9 04                           shr        ecx, 4
004abe39  83 e1 01                           and        ecx, 1
004abe3c  75 6f                              jne        0x4abead
004abe3e  8b 15 28 a8 5b 00                  mov        edx, dword ptr [0x5ba828]
004abe44  8b 82 e8 00 00 00                  mov        eax, dword ptr [edx + 0xe8]
004abe4a  c1 e8 02                           shr        eax, 2
004abe4d  83 e0 01                           and        eax, 1
004abe50  75 5b                              jne        0x4abead
004abe52  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004abe55  f2 0f 10 41 30                     movsd      xmm0, qword ptr [ecx + 0x30]
004abe5a  f2 0f 58 05 70 c4 56 00            addsd      xmm0, qword ptr [0x56c470]
004abe62  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004abe65  f2 0f 11 42 30                     movsd      qword ptr [edx + 0x30], xmm0
004abe6a  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004abe6d  f3 0f 10 40 38                     movss      xmm0, dword ptr [eax + 0x38]
004abe72  0f 2f 05 14 06 57 00               comiss     xmm0, dword ptr [0x570614]
004abe79  76 1a                              jbe        0x4abe95
004abe7b  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004abe7e  f2 0f 10 41 28                     movsd      xmm0, qword ptr [ecx + 0x28]
004abe83  f2 0f 58 05 70 c4 56 00            addsd      xmm0, qword ptr [0x56c470]
004abe8b  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004abe8e  f2 0f 11 42 28                     movsd      qword ptr [edx + 0x28], xmm0
004abe93  eb 18                              jmp        0x4abead
004abe95  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004abe98  f3 0f 5a 40 38                     cvtss2sd   xmm0, dword ptr [eax + 0x38]
004abe9d  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004abea0  f2 0f 58 41 28                     addsd      xmm0, qword ptr [ecx + 0x28]
004abea5  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004abea8  f2 0f 11 42 28                     movsd      qword ptr [edx + 0x28], xmm0
004abead  83 3d 28 a8 5b 00 00               cmp        dword ptr [0x5ba828], 0
004abeb4  74 1d                              je         0x4abed3
004abeb6  a1 28 a8 5b 00                     mov        eax, dword ptr [0x5ba828]
004abebb  8b 88 e8 00 00 00                  mov        ecx, dword ptr [eax + 0xe8]
004abec1  81 e1 ff fe ff ff                  and        ecx, 0xfffffeff
004abec7  8b 15 28 a8 5b 00                  mov        edx, dword ptr [0x5ba828]
004abecd  89 8a e8 00 00 00                  mov        dword ptr [edx + 0xe8], ecx
004abed3  8b 45 fc                           mov        eax, dword ptr [ebp - 4]
004abed6  c7 40 24 00 00 00 00               mov        dword ptr [eax + 0x24], 0
004abedd  0f b6 0d 84 4f 5c 00               movzx      ecx, byte ptr [0x5c4f84]
004abee4  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004abee7  8b 42 24                           mov        eax, dword ptr [edx + 0x24]
004abeea  8d 4c 01 01                        lea        ecx, [ecx + eax + 1]
004abeee  8b 55 fc                           mov        edx, dword ptr [ebp - 4]
004abef1  89 4a 24                           mov        dword ptr [edx + 0x24], ecx
004abef4  8b 4d fc                           mov        ecx, dword ptr [ebp - 4]
004abef7  e8 e4 01 00 00                     call       0x4ac0e0
004abefc  b8 01 00 00 00                     mov        eax, 1
004abf01  8b e5                              mov        esp, ebp
004abf03  5d                                 pop        ebp
004abf04  c3                                 ret        
