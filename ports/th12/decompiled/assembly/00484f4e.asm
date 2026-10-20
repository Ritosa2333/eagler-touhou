
// block 00484f4e
00484f4e  mov        edi, edi
00484f50  push       ebp
00484f51  mov        ebp, esp
00484f53  push       ecx
00484f54  push       ebx
00484f55  movsx      eax, al
00484f58  cmp        eax, 0x59
00484f5b  push       esi
00484f5c  push       edi
00484f5d  mov        ebx, ecx
00484f5f  mov        esi, edx
00484f61  jg         0x485124

// block 00484f67
00484f67  je         0x4850f2

// block 00484f6d
00484f6d  cmp        eax, 0x49
00484f70  jg         0x485041

// block 00484f76
00484f76  je         0x485014

// block 00484f7c
00484f7c  sub        eax, 4
00484f7f  je         0x48532e

// block 00484f85
00484f85  sub        eax, 9
00484f88  je         0x48532e

// block 00484f8e
00484f8e  sub        eax, 0x18
00484f91  je         0x485003

// block 00484f93
00484f93  sub        eax, 0x1c
00484f96  je         0x484fe1

// block 00484f98
00484f98  dec        eax
00484f99  je         0x484fbf

// block 00484f9b
00484f9b  sub        eax, 6
00484f9e  jne        0x48523e

// block 00484fa4
00484fa4  mov        eax, dword ptr [esi + 8]
00484fa7  xor        edi, edi
00484fa9  cmp        eax, edi
00484fab  jl         0x485226

// block 00484fb1
00484fb1  cmp        eax, 0x17

// block 00484fb4
00484fb4  jg         0x485226

// block 00484fba
00484fba  jmp        0x485195

// block 00484fbf
00484fbf  mov        esi, dword ptr [esi + 0x10]
00484fc2  xor        edi, edi
00484fc4  cmp        esi, edi
00484fc6  jl         0x485226

// block 00484fcc
00484fcc  cmp        esi, 0xb
00484fcf  jg         0x485226

// block 00484fd5
00484fd5  mov        edx, dword ptr [ebp + 0x10]
00484fd8  mov        edx, dword ptr [edx + esi*4 + 0x68]
00484fdc  jmp        0x485326

// block 00484fe1
00484fe1  mov        esi, dword ptr [esi + 0x18]
00484fe4  xor        edi, edi
00484fe6  cmp        esi, edi
00484fe8  jl         0x485226

// block 00484fee
00484fee  cmp        esi, 6
00484ff1  jg         0x485226

// block 00484ff7
00484ff7  mov        edx, dword ptr [ebp + 0x10]
00484ffa  mov        edx, dword ptr [edx + esi*4 + 0x1c]
00484ffe  jmp        0x485326

// block 00485003
00485003  mov        eax, dword ptr [ebx]
00485005  mov        byte ptr [eax], 0x25
00485008  mov        eax, dword ptr [ebp + 0xc]
0048500b  inc        dword ptr [ebx]
0048500d  dec        dword ptr [eax]
0048500f  jmp        0x48532e

// block 00485014
00485014  mov        eax, dword ptr [esi + 8]
00485017  xor        edi, edi
00485019  cmp        eax, edi
0048501b  jl         0x485226

// block 00485021
00485021  cmp        eax, 0x17
00485024  jg         0x485226

// block 0048502a
0048502a  cdq        
0048502b  push       0xc
0048502d  pop        ecx
0048502e  idiv       ecx
00485030  mov        eax, edx
00485032  cmp        eax, edi
00485034  jne        0x485195

// block 0048503a
0048503a  push       ecx
0048503b  pop        eax
0048503c  jmp        0x485195

// block 00485041
00485041  sub        eax, 0x4d
00485044  je         0x4850ed

// block 0048504a
0048504a  push       6
0048504c  pop        ecx
0048504d  sub        eax, ecx
0048504f  je         0x4850d9

// block 00485055
00485055  dec        eax
00485056  dec        eax
00485057  je         0x48508d

// block 00485059
00485059  dec        eax
0048505a  dec        eax
0048505b  je         0x48506f

// block 0048505d
0048505d  dec        eax
0048505e  jne        0x48523e

// block 00485064
00485064  push       dword ptr [ebp + 0x10]
00485067  push       dword ptr [ebp + 0xc]
0048506a  jmp        0x4851e2

// block 0048506f
0048506f  mov        eax, dword ptr [esi + 0x18]
00485072  xor        edi, edi
00485074  cmp        eax, edi
00485076  jl         0x485226

// block 0048507c
0048507c  cmp        eax, ecx
0048507e  jg         0x485226

// block 00485084
00485084  cmp        eax, edi
00485086  je         0x4850a4

// block 00485088
00485088  lea        ecx, [eax - 1]
0048508b  jmp        0x4850a4

// block 0048508d
0048508d  mov        eax, dword ptr [esi + 0x18]
00485090  xor        edi, edi
00485092  cmp        eax, edi
00485094  jl         0x485226

// block 0048509a
0048509a  cmp        eax, ecx
0048509c  jg         0x485226

// block 004850a2
004850a2  mov        ecx, eax

// block 004850a4
004850a4  mov        eax, dword ptr [esi + 0x1c]
004850a7  cmp        eax, edi
004850a9  jl         0x485226

// block 004850af
004850af  cmp        eax, 0x16d
004850b4  jg         0x485226

// block 004850ba
004850ba  cmp        eax, ecx
004850bc  jge        0x4850c5

// block 004850be
004850be  xor        eax, eax
004850c0  jmp        0x485195

// block 004850c5
004850c5  push       7
004850c7  cdq        
004850c8  pop        esi
004850c9  idiv       esi
004850cb  cmp        edx, ecx
004850cd  jl         0x485195

// block 004850d3
004850d3  inc        eax
004850d4  jmp        0x485195

// block 004850d9
004850d9  mov        eax, dword ptr [esi]

// block 004850db
004850db  xor        edi, edi
004850dd  cmp        eax, edi
004850df  jl         0x485226

// block 004850e5
004850e5  cmp        eax, 0x3b
004850e8  jmp        0x484fb4

// block 004850ed
004850ed  mov        eax, dword ptr [esi + 4]
004850f0  jmp        0x4850db

// block 004850f2
004850f2  mov        eax, dword ptr [esi + 0x14]
004850f5  cmp        eax, 0xfffff894
004850fa  jl         0x485103

// block 004850fc
004850fc  cmp        eax, 0x1fa3
00485101  jle        0x48510f

// block 00485103
00485103  call       0x46e96f

// block 00485108
00485108  xor        edi, edi
0048510a  jmp        0x48522b

// block 0048510f
0048510f  cdq        
00485110  push       0x64
00485112  pop        ecx
00485113  idiv       ecx
00485115  push       dword ptr [ebp + 0x14]
00485118  push       4
0048511a  add        eax, 0x13
0048511d  imul       eax, eax, 0x64
00485120  add        eax, edx
00485122  jmp        0x48519a

// block 00485124
00485124  cmp        eax, 0x6d
00485127  jg         0x485266

// block 0048512d
0048512d  je         0x485250

// block 00485133
00485133  sub        eax, 0x5a
00485136  je         0x48527d

// block 0048513c
0048513c  sub        eax, 7
0048513f  je         0x485218

// block 00485145
00485145  dec        eax
00485146  je         0x4851fe

// block 0048514c
0048514c  dec        eax
0048514d  je         0x4851ab

// block 0048514f
0048514f  dec        eax
00485150  je         0x48517e

// block 00485152
00485152  sub        eax, 6
00485155  jne        0x48523e

// block 0048515b
0048515b  mov        esi, dword ptr [esi + 0x1c]
0048515e  xor        edi, edi
00485160  cmp        esi, edi
00485162  jl         0x485226

// block 00485168
00485168  cmp        esi, 0x16d
0048516e  jg         0x485226

// block 00485174
00485174  push       dword ptr [ebp + 0x14]
00485177  lea        eax, [esi + 1]
0048517a  push       3
0048517c  jmp        0x48519a

// block 0048517e
0048517e  mov        esi, dword ptr [esi + 0xc]
00485181  cmp        esi, 1
00485184  jl         0x485103

// block 0048518a
0048518a  cmp        esi, 0x1f
0048518d  jg         0x485103

// block 00485193
00485193  mov        eax, esi

// block 00485195
00485195  push       dword ptr [ebp + 0x14]

// block 00485198
00485198  push       2

// block 0048519a
0048519a  pop        edx

// block 0048519b
0048519b  mov        ecx, dword ptr [ebp + 0xc]
0048519e  mov        edi, ebx
004851a0  call       0x484ed3

// block 004851a5
004851a5  pop        ecx
004851a6  jmp        0x48532e

// block 004851ab
004851ab  push       dword ptr [ebp + 0x10]
004851ae  xor        edi, edi
004851b0  push       dword ptr [ebp + 0xc]
004851b3  push       ebx
004851b4  push       esi
004851b5  cmp        dword ptr [ebp + 0x14], edi
004851b8  je         0x4851fb

// block 004851ba
004851ba  push       1

// block 004851bc
004851bc  push       dword ptr [ebp + 8]
004851bf  call       0x485336

// block 004851c4
004851c4  add        esp, 0x18
004851c7  test       eax, eax
004851c9  je         0x48523e

// block 004851cb
004851cb  mov        eax, dword ptr [ebp + 0xc]
004851ce  cmp        dword ptr [eax], edi
004851d0  je         0x48523e

// block 004851d2
004851d2  mov        eax, dword ptr [ebx]
004851d4  push       dword ptr [ebp + 0x10]
004851d7  mov        byte ptr [eax], 0x20
004851da  mov        eax, dword ptr [ebp + 0xc]
004851dd  inc        dword ptr [ebx]
004851df  dec        dword ptr [eax]
004851e1  push       eax

// block 004851e2
004851e2  push       ebx
004851e3  push       esi
004851e4  push       2

// block 004851e6
004851e6  push       dword ptr [ebp + 8]
004851e9  call       0x485336

// block 004851ee
004851ee  add        esp, 0x18
004851f1  test       eax, eax
004851f3  jne        0x48532e

// block 004851f9
004851f9  jmp        0x48523e

// block 004851fb
004851fb  push       edi
004851fc  jmp        0x4851bc

// block 004851fe
004851fe  mov        esi, dword ptr [esi + 0x10]
00485201  xor        edi, edi
00485203  cmp        esi, edi
00485205  jl         0x485226

// block 00485207
00485207  cmp        esi, 0xb
0048520a  jg         0x485226

// block 0048520c
0048520c  mov        edx, dword ptr [ebp + 0x10]
0048520f  mov        edx, dword ptr [edx + esi*4 + 0x38]
00485213  jmp        0x485326

// block 00485218
00485218  mov        esi, dword ptr [esi + 0x18]
0048521b  xor        edi, edi
0048521d  cmp        esi, edi
0048521f  jl         0x485226

// block 00485221
00485221  cmp        esi, 6
00485224  jle        0x485245

// block 00485226
00485226  call       0x46e96f

// block 0048522b
0048522b  push       edi
0048522c  push       edi
0048522d  push       edi
0048522e  push       edi
0048522f  push       edi
00485230  mov        dword ptr [eax], 0x16
00485236  call       0x470f2d

// block 0048523b
0048523b  add        esp, 0x14

// block 0048523e
0048523e  xor        eax, eax
00485240  jmp        0x485331

// block 00485245
00485245  mov        edx, dword ptr [ebp + 0x10]
00485248  mov        edx, dword ptr [edx + esi*4]
0048524b  jmp        0x485326

// block 00485250
00485250  mov        esi, dword ptr [esi + 0x10]
00485253  xor        edi, edi
00485255  cmp        esi, edi
00485257  jl         0x485226

// block 00485259
00485259  cmp        esi, 0xb
0048525c  jg         0x485226

// block 0048525e
0048525e  lea        eax, [esi + 1]
00485261  jmp        0x485195

// block 00485266
00485266  sub        eax, 0x70
00485269  je         0x4852fa

// block 0048526f
0048526f  sub        eax, 7
00485272  je         0x4852d9

// block 00485274
00485274  dec        eax
00485275  je         0x4852bd

// block 00485277
00485277  dec        eax
00485278  je         0x4852a0

// block 0048527a
0048527a  dec        eax
0048527b  jne        0x48523e

// block 0048527d
0048527d  call       0x476f80

// block 00485282
00485282  call       0x477413

// block 00485287
00485287  mov        edx, eax
00485289  xor        eax, eax
0048528b  cmp        dword ptr [esi + 0x20], eax
0048528e  mov        ecx, ebx
00485290  setne      al
00485293  mov        ebx, eax
00485295  mov        edx, dword ptr [edx + ebx*4]
00485298  mov        dword ptr [ebp - 4], eax
0048529b  jmp        0x485326

// block 004852a0
004852a0  mov        eax, dword ptr [esi + 0x14]
004852a3  xor        edi, edi
004852a5  cmp        eax, edi
004852a7  jl         0x485226

// block 004852ad
004852ad  push       dword ptr [ebp + 0x14]
004852b0  cdq        
004852b1  push       0x64
004852b3  pop        ecx
004852b4  idiv       ecx
004852b6  mov        eax, edx
004852b8  jmp        0x485198

// block 004852bd
004852bd  push       dword ptr [ebp + 0x10]
004852c0  xor        edi, edi
004852c2  push       dword ptr [ebp + 0xc]
004852c5  push       ebx
004852c6  push       esi
004852c7  cmp        dword ptr [ebp + 0x14], edi
004852ca  je         0x4852d3

// block 004852cc
004852cc  push       1
004852ce  jmp        0x4851e6

// block 004852d3
004852d3  push       edi
004852d4  jmp        0x4851e6

// block 004852d9
004852d9  mov        eax, dword ptr [esi + 0x18]
004852dc  xor        edi, edi
004852de  cmp        eax, edi
004852e0  jl         0x485226

// block 004852e6
004852e6  cmp        eax, 6
004852e9  jg         0x485226

// block 004852ef
004852ef  push       dword ptr [ebp + 0x14]
004852f2  xor        edx, edx
004852f4  inc        edx
004852f5  jmp        0x48519b

// block 004852fa
004852fa  mov        esi, dword ptr [esi + 8]
004852fd  xor        edi, edi
004852ff  cmp        esi, edi
00485301  jl         0x485226

// block 00485307
00485307  cmp        esi, 0x17
0048530a  jg         0x485226

// block 00485310
00485310  cmp        esi, 0xb
00485313  mov        edx, dword ptr [ebp + 0x10]
00485316  jg         0x485320

// block 00485318
00485318  mov        edx, dword ptr [edx + 0x98]
0048531e  jmp        0x485326

// block 00485320
00485320  mov        edx, dword ptr [edx + 0x9c]

// block 00485326
00485326  mov        eax, dword ptr [ebp + 0xc]
00485329  call       0x484e7d

// block 0048532e
0048532e  xor        eax, eax
00485330  inc        eax

// block 00485331
00485331  pop        edi
00485332  pop        esi
00485333  pop        ebx
00485334  leave      
00485335  ret        
