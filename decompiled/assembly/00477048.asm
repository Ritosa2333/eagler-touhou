
// block 00477048
00477048  mov        edi, edi
0047704a  push       ebp
0047704b  mov        ebp, esp
0047704d  sub        esp, 0x10
00477050  push       esi
00477051  mov        esi, dword ptr [ebp + 8]
00477054  push       edi
00477055  xor        edi, edi
00477057  mov        dword ptr [ebp - 4], edi
0047705a  cmp        esi, edi
0047705c  jne        0x47707c

// block 0047705e
0047705e  call       0x46e96f

// block 00477063
00477063  push       0x16
00477065  pop        esi
00477066  push       edi
00477067  push       edi
00477068  push       edi
00477069  push       edi
0047706a  push       edi
0047706b  mov        dword ptr [eax], esi
0047706d  call       0x470f2d

// block 00477072
00477072  add        esp, 0x14

// block 00477075
00477075  mov        eax, esi
00477077  jmp        0x477287

// block 0047707c
0047707c  push       0x24
0047707e  push       0xff
00477083  push       esi
00477084  call       0x477420

// block 00477089
00477089  mov        eax, dword ptr [ebp + 0xc]
0047708c  add        esp, 0xc
0047708f  cmp        eax, edi
00477091  je         0x47705e

// block 00477093
00477093  mov        ecx, dword ptr [eax]
00477095  mov        eax, dword ptr [eax + 4]
00477098  cmp        eax, -1
0047709b  mov        dword ptr [ebp - 0x10], ecx
0047709e  mov        dword ptr [ebp - 0xc], eax
004770a1  jg         0x4770b9

// block 004770a3
004770a3  jl         0x4770ad

// block 004770a5
004770a5  cmp        ecx, 0xffff5740
004770ab  jae        0x4770b9

// block 004770ad
004770ad  call       0x46e96f

// block 004770b2
004770b2  push       0x16
004770b4  pop        esi
004770b5  mov        dword ptr [eax], esi
004770b7  jmp        0x477075

// block 004770b9
004770b9  cmp        eax, 7
004770bc  jl         0x4770c8

// block 004770be
004770be  jg         0x4770ad

// block 004770c0
004770c0  cmp        ecx, 0x934126cf
004770c6  ja         0x4770ad

// block 004770c8
004770c8  push       ebx
004770c9  push       edi
004770ca  push       0x1e13380
004770cf  push       eax
004770d0  push       ecx
004770d1  call       0x4774a0

// block 004770d6
004770d6  mov        ecx, eax
004770d8  add        ecx, 0x46
004770db  lea        eax, [ecx + 0x12b]
004770e1  cdq        
004770e2  mov        ebx, 0x190
004770e7  idiv       ebx
004770e9  lea        edi, [ecx - 1]
004770ec  push       0x64
004770ee  mov        dword ptr [ebp - 8], edi
004770f1  mov        dword ptr [ebp + 8], ecx
004770f4  mov        ebx, eax
004770f6  mov        eax, edi
004770f8  cdq        
004770f9  pop        edi
004770fa  idiv       edi
004770fc  push       -1
004770fe  push       0xfffffe93
00477103  sub        ebx, eax
00477105  mov        eax, dword ptr [ebp - 8]
00477108  cdq        
00477109  and        edx, 3
0047710c  add        eax, edx
0047710e  sar        eax, 2
00477111  lea        eax, [ebx + eax - 0x11]
00477115  cdq        
00477116  mov        edi, eax
00477118  lea        eax, [ecx - 0x46]
0047711b  mov        ebx, edx
0047711d  cdq        
0047711e  push       edx
0047711f  push       eax
00477120  call       0x483220

// block 00477125
00477125  sub        eax, edi
00477127  sbb        edx, ebx
00477129  push       0
0047712b  mov        ebx, 0x15180
00477130  push       ebx
00477131  push       edx
00477132  push       eax
00477133  call       0x483220

// block 00477138
00477138  mov        edi, dword ptr [ebp - 0x10]
0047713b  add        edi, eax
0047713d  adc        dword ptr [ebp - 0xc], edx
00477140  cmp        dword ptr [ebp - 0xc], 0
00477144  jg         0x477193

// block 00477146
00477146  jl         0x47714c

// block 00477148
00477148  test       edi, edi
0047714a  jae        0x477193

// block 0047714c
0047714c  mov        eax, dword ptr [ebp - 8]
0047714f  add        edi, 0x1e13380
00477155  adc        dword ptr [ebp - 0xc], 0
00477159  mov        ecx, eax
0047715b  and        ecx, 0x80000003
00477161  mov        dword ptr [ebp + 8], eax
00477164  jns        0x47716b

// block 00477166
00477166  dec        ecx
00477167  or         ecx, 0xfffffffc
0047716a  inc        ecx

// block 0047716b
0047716b  jne        0x47717a

// block 0047716d
0047716d  push       0x64
0047716f  cdq        
00477170  pop        ecx
00477171  idiv       ecx
00477173  test       edx, edx
00477175  jne        0x47718b

// block 00477177
00477177  mov        eax, dword ptr [ebp + 8]

// block 0047717a
0047717a  add        eax, 0x76c
0047717f  cdq        
00477180  mov        ecx, 0x190
00477185  idiv       ecx
00477187  test       edx, edx
00477189  jne        0x4771cc

// block 0047718b
0047718b  add        edi, ebx
0047718d  adc        dword ptr [ebp - 0xc], 0
00477191  jmp        0x4771c5

// block 00477193
00477193  mov        eax, dword ptr [ebp + 8]
00477196  mov        ecx, eax
00477198  and        ecx, 0x80000003
0047719e  jns        0x4771a5

// block 004771a0
004771a0  dec        ecx
004771a1  or         ecx, 0xfffffffc
004771a4  inc        ecx

// block 004771a5
004771a5  jne        0x4771b1

// block 004771a7
004771a7  push       0x64
004771a9  cdq        
004771aa  pop        ecx
004771ab  idiv       ecx
004771ad  test       edx, edx
004771af  jne        0x4771c5

// block 004771b1
004771b1  mov        eax, dword ptr [ebp + 8]
004771b4  add        eax, 0x76c
004771b9  cdq        
004771ba  mov        ecx, 0x190
004771bf  idiv       ecx
004771c1  test       edx, edx
004771c3  jne        0x4771cc

// block 004771c5
004771c5  mov        dword ptr [ebp - 4], 1

// block 004771cc
004771cc  mov        eax, dword ptr [ebp + 8]
004771cf  push       0
004771d1  push       ebx
004771d2  push       dword ptr [ebp - 0xc]
004771d5  mov        dword ptr [esi + 0x14], eax
004771d8  push       edi
004771d9  call       0x4774a0

// block 004771de
004771de  push       -1
004771e0  mov        dword ptr [esi + 0x1c], eax
004771e3  cdq        
004771e4  push       0xfffeae80
004771e9  push       edx
004771ea  push       eax
004771eb  call       0x483220

// block 004771f0
004771f0  add        edi, eax
004771f2  adc        dword ptr [ebp - 0xc], edx
004771f5  cmp        dword ptr [ebp - 4], 0
004771f9  mov        edx, 0x4ae29c
004771fe  jne        0x477205

// block 00477200
00477200  mov        edx, 0x4ae2d0

// block 00477205
00477205  mov        eax, dword ptr [esi + 0x1c]
00477208  xor        ecx, ecx
0047720a  inc        ecx
0047720b  cmp        dword ptr [edx + 4], eax
0047720e  jge        0x47721d

// block 00477210
00477210  mov        ebx, eax

// block 00477212
00477212  inc        ecx
00477213  cmp        dword ptr [edx + ecx*4], ebx
00477216  jl         0x477212

// block 00477218
00477218  mov        ebx, 0x15180

// block 0047721d
0047721d  dec        ecx
0047721e  mov        dword ptr [esi + 0x10], ecx
00477221  sub        eax, dword ptr [edx + ecx*4]
00477224  push       0
00477226  mov        dword ptr [esi + 0xc], eax
00477229  mov        eax, dword ptr [ebp + 0xc]
0047722c  push       ebx
0047722d  push       dword ptr [eax + 4]
00477230  push       dword ptr [eax]
00477232  call       0x4774a0

// block 00477237
00477237  push       7
00477239  add        eax, 4
0047723c  pop        ecx
0047723d  cdq        
0047723e  idiv       ecx
00477240  xor        ebx, ebx
00477242  push       ebx
00477243  push       0xe10
00477248  push       dword ptr [ebp - 0xc]
0047724b  push       edi
0047724c  mov        dword ptr [esi + 0x18], edx
0047724f  call       0x4774a0

// block 00477254
00477254  push       -1
00477256  mov        dword ptr [esi + 8], eax
00477259  cdq        
0047725a  push       0xfffff1f0
0047725f  push       edx
00477260  push       eax
00477261  call       0x483220

// block 00477266
00477266  push       ebx
00477267  add        edi, eax
00477269  adc        dword ptr [ebp - 0xc], edx
0047726c  push       0x3c
0047726e  push       dword ptr [ebp - 0xc]
00477271  push       edi
00477272  call       0x4774a0

// block 00477277
00477277  mov        dword ptr [esi + 4], eax
0047727a  imul       eax, eax, 0x3c
0047727d  sub        edi, eax
0047727f  mov        dword ptr [esi + 0x20], ebx
00477282  mov        dword ptr [esi], edi
00477284  xor        eax, eax
00477286  pop        ebx

// block 00477287
00477287  pop        edi
00477288  pop        esi
00477289  leave      
0047728a  ret        
