
// block 0047b07b
0047b07b  push       0x54
0047b07d  push       0x4aae38
0047b082  call       0x46fcec

// block 0047b087
0047b087  xor        edi, edi
0047b089  mov        dword ptr [ebp - 4], edi
0047b08c  lea        eax, [ebp - 0x64]
0047b08f  push       eax
0047b090  call       dword ptr [0x4981bc]

// block 0047b096
0047b096  mov        dword ptr [ebp - 4], 0xfffffffe
0047b09d  push       0x40
0047b09f  push       0x20
0047b0a1  pop        esi
0047b0a2  push       esi
0047b0a3  call       0x477813

// block 0047b0a8
0047b0a8  pop        ecx
0047b0a9  pop        ecx
0047b0aa  cmp        eax, edi
0047b0ac  je         0x47b2c6

// block 0047b0b2
0047b0b2  mov        dword ptr [0x4d6320], eax
0047b0b7  mov        dword ptr [0x4d6308], esi
0047b0bd  lea        ecx, [eax + 0x800]
0047b0c3  jmp        0x47b0f5

// block 0047b0c5
0047b0c5  mov        byte ptr [eax + 4], 0
0047b0c9  or         dword ptr [eax], 0xffffffff
0047b0cc  mov        byte ptr [eax + 5], 0xa
0047b0d0  mov        dword ptr [eax + 8], edi
0047b0d3  mov        byte ptr [eax + 0x24], 0
0047b0d7  mov        byte ptr [eax + 0x25], 0xa
0047b0db  mov        byte ptr [eax + 0x26], 0xa
0047b0df  mov        dword ptr [eax + 0x38], edi
0047b0e2  mov        byte ptr [eax + 0x34], 0
0047b0e6  add        eax, 0x40
0047b0e9  mov        ecx, dword ptr [0x4d6320]
0047b0ef  add        ecx, 0x800

// block 0047b0f5
0047b0f5  cmp        eax, ecx
0047b0f7  jb         0x47b0c5

// block 0047b0f9
0047b0f9  cmp        word ptr [ebp - 0x32], di
0047b0fd  je         0x47b20d

// block 0047b103
0047b103  mov        eax, dword ptr [ebp - 0x30]
0047b106  cmp        eax, edi
0047b108  je         0x47b20d

// block 0047b10e
0047b10e  mov        edi, dword ptr [eax]
0047b110  lea        ebx, [eax + 4]
0047b113  lea        eax, [ebx + edi]
0047b116  mov        dword ptr [ebp - 0x1c], eax
0047b119  mov        esi, 0x800
0047b11e  cmp        edi, esi
0047b120  jl         0x47b124

// block 0047b122
0047b122  mov        edi, esi

// block 0047b124
0047b124  mov        dword ptr [ebp - 0x20], 1
0047b12b  jmp        0x47b188

// block 0047b12d
0047b12d  push       0x40
0047b12f  push       0x20
0047b131  call       0x477813

// block 0047b136
0047b136  pop        ecx
0047b137  pop        ecx
0047b138  test       eax, eax
0047b13a  je         0x47b192

// block 0047b13c
0047b13c  mov        ecx, dword ptr [ebp - 0x20]
0047b13f  lea        ecx, [ecx*4 + 0x4d6320]
0047b146  mov        dword ptr [ecx], eax
0047b148  add        dword ptr [0x4d6308], 0x20
0047b14f  lea        edx, [eax + 0x800]
0047b155  jmp        0x47b181

// block 0047b157
0047b157  mov        byte ptr [eax + 4], 0
0047b15b  or         dword ptr [eax], 0xffffffff
0047b15e  mov        byte ptr [eax + 5], 0xa
0047b162  and        dword ptr [eax + 8], 0
0047b166  and        byte ptr [eax + 0x24], 0x80
0047b16a  mov        byte ptr [eax + 0x25], 0xa
0047b16e  mov        byte ptr [eax + 0x26], 0xa
0047b172  and        dword ptr [eax + 0x38], 0
0047b176  mov        byte ptr [eax + 0x34], 0
0047b17a  add        eax, 0x40
0047b17d  mov        edx, dword ptr [ecx]
0047b17f  add        edx, esi

// block 0047b181
0047b181  cmp        eax, edx
0047b183  jb         0x47b157

// block 0047b185
0047b185  inc        dword ptr [ebp - 0x20]

// block 0047b188
0047b188  cmp        dword ptr [0x4d6308], edi
0047b18e  jl         0x47b12d

// block 0047b190
0047b190  jmp        0x47b198

// block 0047b192
0047b192  mov        edi, dword ptr [0x4d6308]

// block 0047b198
0047b198  and        dword ptr [ebp - 0x20], 0
0047b19c  test       edi, edi
0047b19e  jle        0x47b20d

// block 0047b1a0
0047b1a0  mov        eax, dword ptr [ebp - 0x1c]
0047b1a3  mov        ecx, dword ptr [eax]
0047b1a5  cmp        ecx, -1
0047b1a8  je         0x47b200

// block 0047b1aa
0047b1aa  cmp        ecx, -2
0047b1ad  je         0x47b200

// block 0047b1af
0047b1af  mov        al, byte ptr [ebx]
0047b1b1  test       al, 1
0047b1b3  je         0x47b200

// block 0047b1b5
0047b1b5  test       al, 8
0047b1b7  jne        0x47b1c4

// block 0047b1b9
0047b1b9  push       ecx
0047b1ba  call       dword ptr [0x498114]

// block 0047b1c0
0047b1c0  test       eax, eax
0047b1c2  je         0x47b200

// block 0047b1c4
0047b1c4  mov        esi, dword ptr [ebp - 0x20]
0047b1c7  mov        eax, esi
0047b1c9  sar        eax, 5
0047b1cc  and        esi, 0x1f
0047b1cf  shl        esi, 6
0047b1d2  add        esi, dword ptr [eax*4 + 0x4d6320]
0047b1d9  mov        eax, dword ptr [ebp - 0x1c]
0047b1dc  mov        eax, dword ptr [eax]
0047b1de  mov        dword ptr [esi], eax
0047b1e0  mov        al, byte ptr [ebx]
0047b1e2  mov        byte ptr [esi + 4], al
0047b1e5  push       0xfa0
0047b1ea  lea        eax, [esi + 0xc]
0047b1ed  push       eax
0047b1ee  call       0x47b416

// block 0047b1f3
0047b1f3  pop        ecx
0047b1f4  pop        ecx
0047b1f5  test       eax, eax
0047b1f7  je         0x47b2c6

// block 0047b1fd
0047b1fd  inc        dword ptr [esi + 8]

// block 0047b200
0047b200  inc        dword ptr [ebp - 0x20]
0047b203  inc        ebx
0047b204  add        dword ptr [ebp - 0x1c], 4
0047b208  cmp        dword ptr [ebp - 0x20], edi
0047b20b  jl         0x47b1a0

// block 0047b20d
0047b20d  xor        ebx, ebx

// block 0047b20f
0047b20f  mov        esi, ebx
0047b211  shl        esi, 6
0047b214  add        esi, dword ptr [0x4d6320]
0047b21a  mov        eax, dword ptr [esi]
0047b21c  cmp        eax, -1
0047b21f  je         0x47b22c

// block 0047b221
0047b221  cmp        eax, -2
0047b224  je         0x47b22c

// block 0047b226
0047b226  or         byte ptr [esi + 4], 0x80
0047b22a  jmp        0x47b29e

// block 0047b22c
0047b22c  mov        byte ptr [esi + 4], 0x81
0047b230  test       ebx, ebx
0047b232  jne        0x47b239

// block 0047b234
0047b234  push       -0xa
0047b236  pop        eax
0047b237  jmp        0x47b243

// block 0047b239
0047b239  mov        eax, ebx
0047b23b  dec        eax
0047b23c  neg        eax
0047b23e  sbb        eax, eax
0047b240  add        eax, -0xb

// block 0047b243
0047b243  push       eax
0047b244  call       dword ptr [0x498168]

// block 0047b24a
0047b24a  mov        edi, eax
0047b24c  cmp        edi, -1
0047b24f  je         0x47b294

// block 0047b251
0047b251  test       edi, edi
0047b253  je         0x47b294

// block 0047b255
0047b255  push       edi
0047b256  call       dword ptr [0x498114]

// block 0047b25c
0047b25c  test       eax, eax
0047b25e  je         0x47b294

// block 0047b260
0047b260  mov        dword ptr [esi], edi
0047b262  and        eax, 0xff
0047b267  cmp        eax, 2
0047b26a  jne        0x47b272

// block 0047b26c
0047b26c  or         byte ptr [esi + 4], 0x40
0047b270  jmp        0x47b27b

// block 0047b272
0047b272  cmp        eax, 3
0047b275  jne        0x47b27b

// block 0047b277
0047b277  or         byte ptr [esi + 4], 8

// block 0047b27b
0047b27b  push       0xfa0
0047b280  lea        eax, [esi + 0xc]
0047b283  push       eax
0047b284  call       0x47b416

// block 0047b289
0047b289  pop        ecx
0047b28a  pop        ecx
0047b28b  test       eax, eax
0047b28d  je         0x47b2c6

// block 0047b28f
0047b28f  inc        dword ptr [esi + 8]
0047b292  jmp        0x47b29e

// block 0047b294
0047b294  or         byte ptr [esi + 4], 0x40
0047b298  mov        dword ptr [esi], 0xfffffffe

// block 0047b29e
0047b29e  inc        ebx
0047b29f  cmp        ebx, 3
0047b2a2  jl         0x47b20f

// block 0047b2a8
0047b2a8  push       dword ptr [0x4d6308]
0047b2ae  call       dword ptr [0x498118]

// block 0047b2b4
0047b2b4  xor        eax, eax
0047b2b6  jmp        0x47b2c9

// block 0047b2c6
0047b2c6  or         eax, 0xffffffff

// block 0047b2c9
0047b2c9  call       0x46fd31

// block 0047b2ce
0047b2ce  ret        
