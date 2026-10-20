
// block 00430f10
00430f10  sub        esp, 0xc
00430f13  fldz       
00430f15  push       esi
00430f16  fst        dword ptr [esp + 4]
00430f1a  mov        dword ptr [0x4cece8], 1
00430f24  mov        eax, dword ptr [esp + 4]
00430f28  fst        dword ptr [esp + 8]
00430f2c  mov        ecx, dword ptr [esp + 8]
00430f30  fst        dword ptr [esp + 0xc]
00430f34  mov        edx, dword ptr [esp + 0xc]
00430f38  fst        dword ptr [esp + 4]
00430f3c  fst        dword ptr [esp + 8]
00430f40  mov        dword ptr [0x4cec04], eax
00430f45  mov        eax, dword ptr [esp + 4]
00430f49  fst        dword ptr [esp + 0xc]
00430f4d  fst        dword ptr [esp + 4]
00430f51  mov        dword ptr [0x4cec08], ecx
00430f57  mov        ecx, dword ptr [esp + 8]
00430f5b  fld1       
00430f5d  fst        dword ptr [esp + 8]
00430f61  mov        dword ptr [0x4cec0c], edx
00430f67  mov        edx, dword ptr [esp + 0xc]
00430f6b  fxch       st(1)
00430f6d  fst        dword ptr [esp + 0xc]
00430f71  mov        dword ptr [0x4cec10], eax
00430f76  fld        dword ptr [0x4a3e0c]
00430f7c  mov        eax, dword ptr [esp + 4]
00430f80  fst        dword ptr [0x4cec4c]
00430f86  mov        dword ptr [0x4cec14], ecx
00430f8c  mov        ecx, dword ptr [esp + 8]
00430f90  fxch       st(1)
00430f92  fst        dword ptr [0x4cece0]
00430f98  mov        dword ptr [0x4cec18], edx
00430f9e  mov        edx, dword ptr [esp + 0xc]
00430fa2  fxch       st(2)
00430fa4  fst        dword ptr [0x4cece4]
00430faa  mov        dword ptr [0x4cec1c], eax
00430faf  fxch       st(2)
00430fb1  mov        dword ptr [0x4cec20], ecx
00430fb7  fst        dword ptr [esp + 4]
00430fbb  mov        dword ptr [0x4cec24], edx
00430fc1  fst        dword ptr [esp + 8]
00430fc5  xor        eax, eax
00430fc7  mov        esi, dword ptr [esp + 4]
00430fcb  fst        dword ptr [esp + 0xc]
00430fcf  mov        dword ptr [0x4cec40], esi
00430fd5  fst        dword ptr [esp + 4]
00430fd9  mov        esi, dword ptr [esp + 8]
00430fdd  fst        dword ptr [esp + 8]
00430fe1  fld        dword ptr [0x4a3de4]
00430fe7  mov        dword ptr [0x4cec44], esi
00430fed  mov        esi, dword ptr [esp + 0xc]
00430ff1  fst        dword ptr [esp + 0xc]
00430ff5  mov        dword ptr [0x4cec48], esi
00430ffb  fxch       st(1)
00430ffd  mov        esi, dword ptr [esp + 4]
00431001  fst        dword ptr [esp + 4]
00431005  mov        dword ptr [0x4ceaec], esi
0043100b  mov        esi, dword ptr [esp + 8]
0043100f  fst        dword ptr [esp + 8]
00431013  mov        dword ptr [0x4ceaf0], esi
00431019  mov        esi, dword ptr [esp + 0xc]
0043101d  fst        dword ptr [esp + 0xc]
00431021  mov        dword ptr [0x4ceaf4], esi
00431027  mov        esi, dword ptr [esp + 4]
0043102b  fst        dword ptr [esp + 4]
0043102f  mov        dword ptr [0x4ceaf8], esi
00431035  mov        esi, dword ptr [esp + 8]
00431039  mov        dword ptr [0x4ceafc], esi
0043103f  mov        esi, dword ptr [esp + 0xc]
00431043  fst        dword ptr [esp + 0xc]
00431047  mov        dword ptr [0x4ceb00], esi
0043104d  fxch       st(3)
0043104f  mov        esi, dword ptr [esp + 4]
00431053  fst        dword ptr [esp + 8]
00431057  mov        edx, 0x280
0043105c  mov        ecx, 0x1e0
00431061  mov        dword ptr [0x4cecd0], eax
00431066  mov        dword ptr [0x4cecd4], eax
0043106b  mov        dword ptr [0x4cecd8], edx
00431071  mov        dword ptr [0x4cecdc], ecx
00431077  mov        dword ptr [0x4ceb04], esi
0043107d  mov        esi, dword ptr [esp + 8]
00431081  fxch       st(2)
00431083  fst        dword ptr [0x4ceb34]
00431089  mov        dword ptr [0x4ceb08], esi
0043108f  mov        esi, dword ptr [esp + 0xc]
00431093  fxch       st(3)
00431095  fst        dword ptr [0x4cebc8]

// block 0043109b
0043109b  mov        dword ptr [0x4ceb0c], esi
004310a1  fxch       st(2)
004310a3  mov        dword ptr [0x4cebb8], eax
004310a8  fst        dword ptr [0x4cebcc]
004310ae  mov        dword ptr [0x4cebbc], eax
004310b3  fxch       st(2)
004310b5  mov        dword ptr [0x4cebc0], edx
004310bb  fst        dword ptr [esp + 4]
004310bf  mov        esi, dword ptr [esp + 4]
004310c3  mov        dword ptr [0x4ceb28], esi
004310c9  fst        dword ptr [esp + 8]
004310cd  mov        esi, dword ptr [esp + 8]
004310d1  fst        dword ptr [esp + 0xc]
004310d5  mov        dword ptr [0x4ceb2c], esi
004310db  fst        dword ptr [esp + 4]
004310df  mov        esi, dword ptr [esp + 0xc]
004310e3  fst        dword ptr [esp + 8]
004310e7  mov        dword ptr [0x4ceb30], esi
004310ed  fxch       st(1)
004310ef  mov        esi, dword ptr [esp + 4]
004310f3  fstp       dword ptr [esp + 0xc]
004310f7  mov        dword ptr [0x4ced1c], esi
004310fd  mov        esi, dword ptr [esp + 8]
00431101  fst        dword ptr [esp + 4]
00431105  mov        dword ptr [0x4ced20], esi
0043110b  fst        dword ptr [esp + 8]
0043110f  mov        esi, dword ptr [esp + 0xc]
00431113  fst        dword ptr [esp + 0xc]
00431117  mov        dword ptr [0x4ced24], esi
0043111d  mov        esi, dword ptr [esp + 4]
00431121  fst        dword ptr [esp + 4]
00431125  mov        dword ptr [0x4ced28], esi
0043112b  mov        esi, dword ptr [esp + 8]
0043112f  mov        dword ptr [0x4ced2c], esi
00431135  mov        esi, dword ptr [esp + 0xc]
00431139  fst        dword ptr [esp + 0xc]
0043113d  fxch       st(1)
0043113f  mov        dword ptr [0x4ced30], esi
00431145  mov        esi, dword ptr [esp + 4]
00431149  fst        dword ptr [esp + 8]
0043114d  fxch       st(2)
0043114f  mov        dword ptr [0x4ced34], esi
00431155  mov        esi, dword ptr [esp + 8]
00431159  fstp       dword ptr [0x4ced64]
0043115f  mov        dword ptr [0x4ced38], esi
00431165  mov        esi, dword ptr [esp + 0xc]
00431169  fst        dword ptr [0x4cedf8]
0043116f  fxch       st(1)
00431171  mov        dword ptr [0x4cebc4], ecx
00431177  fstp       dword ptr [0x4cedfc]
0043117d  mov        dword ptr [0x4cebd0], eax
00431182  mov        dword ptr [0x4cede8], eax
00431187  fst        dword ptr [esp + 4]
0043118b  mov        dword ptr [0x4cedec], eax
00431190  fst        dword ptr [esp + 8]
00431194  mov        dword ptr [0x4cedf0], edx
0043119a  fstp       dword ptr [esp + 0xc]
0043119e  mov        edx, dword ptr [esp + 0xc]
004311a2  mov        dword ptr [0x4cedf4], ecx
004311a8  mov        ecx, dword ptr [esp + 8]
004311ac  mov        dword ptr [0x4cee00], eax
004311b1  mov        eax, dword ptr [esp + 4]
004311b5  mov        dword ptr [0x4ced3c], esi
004311bb  mov        dword ptr [0x4ced58], eax
004311c0  mov        dword ptr [0x4ced5c], ecx
004311c6  mov        dword ptr [0x4ced60], edx
004311cc  pop        esi
004311cd  add        esp, 0xc
004311d0  ret        
