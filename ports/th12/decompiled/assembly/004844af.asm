
// block 004844af
004844af  mov        edi, edi
004844b1  push       ebp
004844b2  mov        ebp, esp
004844b4  sub        esp, 0x4c
004844b7  mov        eax, dword ptr [0x4ad138]
004844bc  xor        eax, ebp
004844be  mov        dword ptr [ebp - 4], eax
004844c1  push       ebx
004844c2  xor        ebx, ebx
004844c4  push       esi
004844c5  mov        esi, dword ptr [ebp + 8]
004844c8  push       edi
004844c9  mov        dword ptr [ebp - 0x2c], ebx
004844cc  mov        dword ptr [ebp - 0x1c], ebx
004844cf  mov        dword ptr [ebp - 0x20], ebx
004844d2  mov        dword ptr [ebp - 0x28], ebx
004844d5  mov        dword ptr [ebp - 0x24], ebx
004844d8  mov        dword ptr [ebp - 0x4c], esi
004844db  mov        dword ptr [ebp - 0x48], ebx
004844de  cmp        dword ptr [esi + 0x14], ebx
004844e1  je         0x4847fd

// block 004844e7
004844e7  lea        eax, [esi + 4]
004844ea  cmp        dword ptr [eax], ebx
004844ec  jne        0x48450e

// block 004844ee
004844ee  push       eax
004844ef  movzx      eax, word ptr [esi + 0x30]
004844f3  push       0x1004
004844f8  push       eax
004844f9  lea        eax, [ebp - 0x4c]
004844fc  push       ebx
004844fd  push       eax
004844fe  call       0x47a12b

// block 00484503
00484503  add        esp, 0x14
00484506  test       eax, eax
00484508  jne        0x4847d5

// block 0048450e
0048450e  push       4
00484510  call       0x4777ce

// block 00484515
00484515  push       2
00484517  mov        edi, 0x180
0048451c  push       edi
0048451d  mov        dword ptr [ebp - 0x2c], eax
00484520  call       0x477813

// block 00484525
00484525  push       1
00484527  push       edi
00484528  mov        dword ptr [ebp - 0x1c], eax
0048452b  call       0x477813

// block 00484530
00484530  push       1
00484532  push       edi
00484533  mov        dword ptr [ebp - 0x20], eax
00484536  call       0x477813

// block 0048453b
0048453b  push       1
0048453d  push       0x101
00484542  mov        dword ptr [ebp - 0x28], eax
00484545  call       0x477813

// block 0048454a
0048454a  add        esp, 0x24
0048454d  mov        dword ptr [ebp - 0x24], eax
00484550  cmp        dword ptr [ebp - 0x2c], ebx
00484553  je         0x4847d5

// block 00484559
00484559  cmp        dword ptr [ebp - 0x1c], ebx
0048455c  je         0x4847d5

// block 00484562
00484562  cmp        eax, ebx
00484564  je         0x4847d5

// block 0048456a
0048456a  cmp        dword ptr [ebp - 0x20], ebx
0048456d  je         0x4847d5

// block 00484573
00484573  cmp        dword ptr [ebp - 0x28], ebx
00484576  je         0x4847d5

// block 0048457c
0048457c  mov        eax, dword ptr [ebp - 0x2c]
0048457f  mov        dword ptr [eax], ebx
00484581  xor        eax, eax

// block 00484583
00484583  mov        ecx, dword ptr [ebp - 0x24]
00484586  mov        byte ptr [eax + ecx], al
00484589  inc        eax
0048458a  cmp        eax, 0x100
0048458f  jl         0x484583

// block 00484591
00484591  lea        eax, [ebp - 0x18]
00484594  push       eax
00484595  push       dword ptr [esi + 4]
00484598  call       dword ptr [0x498164]

// block 0048459e
0048459e  test       eax, eax
004845a0  je         0x4847d5

// block 004845a6
004845a6  cmp        dword ptr [ebp - 0x18], 5
004845aa  ja         0x4847d5

// block 004845b0
004845b0  movzx      eax, word ptr [ebp - 0x18]
004845b4  cmp        eax, 1
004845b7  mov        dword ptr [ebp - 0x30], eax
004845ba  jle        0x4845e9

// block 004845bc
004845bc  cmp        byte ptr [ebp - 0x12], bl
004845bf  je         0x4845e9

// block 004845c1
004845c1  lea        eax, [ebp - 0x11]

// block 004845c4
004845c4  mov        cl, byte ptr [eax]
004845c6  cmp        cl, bl
004845c8  je         0x4845e9

// block 004845ca
004845ca  movzx      edi, byte ptr [eax - 1]
004845ce  movzx      ecx, cl
004845d1  jmp        0x4845de

// block 004845d3
004845d3  mov        ecx, dword ptr [ebp - 0x24]
004845d6  mov        byte ptr [edi + ecx], 0x20
004845da  movzx      ecx, byte ptr [eax]
004845dd  inc        edi

// block 004845de
004845de  cmp        edi, ecx
004845e0  jle        0x4845d3

// block 004845e2
004845e2  inc        eax
004845e3  inc        eax
004845e4  cmp        byte ptr [eax - 1], bl
004845e7  jne        0x4845c4

// block 004845e9
004845e9  mov        eax, dword ptr [ebp - 0x1c]
004845ec  push       ebx
004845ed  push       ebx
004845ee  push       dword ptr [esi + 4]
004845f1  add        eax, 0x100
004845f6  push       eax
004845f7  push       0x100
004845fc  push       dword ptr [ebp - 0x24]
004845ff  mov        dword ptr [ebp - 0x40], eax
00484602  push       1
00484604  push       ebx
00484605  call       0x48384c

// block 0048460a
0048460a  add        esp, 0x20
0048460d  test       eax, eax
0048460f  je         0x4847d5

// block 00484615
00484615  mov        ecx, dword ptr [ebp - 0x20]
00484618  mov        eax, dword ptr [ebp - 0x24]
0048461b  push       ebx
0048461c  push       dword ptr [esi + 4]
0048461f  mov        edi, 0xff
00484624  push       edi
00484625  add        ecx, 0x81
0048462b  push       ecx
0048462c  push       edi
0048462d  inc        eax
0048462e  push       eax
0048462f  push       0x100
00484634  push       dword ptr [esi + 0x14]
00484637  push       ebx
00484638  call       0x48364d

// block 0048463d
0048463d  add        esp, 0x24
00484640  test       eax, eax
00484642  je         0x4847d5

// block 00484648
00484648  mov        eax, dword ptr [ebp - 0x28]
0048464b  push       ebx
0048464c  push       dword ptr [esi + 4]
0048464f  add        eax, 0x81
00484654  push       edi
00484655  push       eax
00484656  mov        eax, dword ptr [ebp - 0x24]
00484659  push       edi
0048465a  inc        eax
0048465b  push       eax
0048465c  push       0x200
00484661  push       dword ptr [esi + 0x14]
00484664  push       ebx
00484665  call       0x48364d

// block 0048466a
0048466a  add        esp, 0x24
0048466d  test       eax, eax
0048466f  je         0x4847d5

// block 00484675
00484675  mov        eax, dword ptr [ebp - 0x1c]
00484678  mov        edi, dword ptr [ebp - 0x20]
0048467b  lea        ecx, [eax + 0xfe]
00484681  xor        edx, edx
00484683  cmp        dword ptr [ebp - 0x30], 1
00484687  mov        word ptr [ecx], dx
0048468a  mov        edx, dword ptr [ebp - 0x28]
0048468d  mov        dword ptr [ebp - 0x3c], ecx
00484690  lea        ecx, [edi + 0x80]
00484696  mov        byte ptr [edi + 0x7f], bl
00484699  mov        byte ptr [edx + 0x7f], bl
0048469c  mov        byte ptr [ecx], bl
0048469e  mov        dword ptr [ebp - 0x44], ecx
004846a1  lea        ecx, [edx + 0x80]
004846a7  mov        dword ptr [ebp - 0x38], ecx
004846aa  mov        byte ptr [ecx], bl
004846ac  jle        0x484701

// block 004846ae
004846ae  cmp        byte ptr [ebp - 0x12], bl
004846b1  je         0x484701

// block 004846b3
004846b3  lea        ecx, [ebp - 0x11]
004846b6  mov        dword ptr [ebp - 0x1c], ecx

// block 004846b9
004846b9  mov        dl, byte ptr [ecx]
004846bb  cmp        dl, bl
004846bd  je         0x484701

// block 004846bf
004846bf  movzx      ecx, byte ptr [ecx - 1]
004846c3  movzx      edx, dl
004846c6  cmp        ecx, edx
004846c8  mov        dword ptr [ebp - 0x20], ecx
004846cb  jg         0x4846f4

// block 004846cd
004846cd  lea        ecx, [eax + ecx*2 + 0x100]
004846d4  jmp        0x4846d9

// block 004846d6
004846d6  mov        ecx, dword ptr [ebp - 0x34]

// block 004846d9
004846d9  inc        dword ptr [ebp - 0x20]
004846dc  mov        edx, 0x8000
004846e1  mov        word ptr [ecx], dx
004846e4  inc        ecx
004846e5  inc        ecx
004846e6  mov        dword ptr [ebp - 0x34], ecx
004846e9  mov        ecx, dword ptr [ebp - 0x1c]
004846ec  movzx      ecx, byte ptr [ecx]
004846ef  cmp        dword ptr [ebp - 0x20], ecx
004846f2  jle        0x4846d6

// block 004846f4
004846f4  mov        ecx, dword ptr [ebp - 0x1c]
004846f7  inc        ecx
004846f8  inc        ecx
004846f9  mov        dword ptr [ebp - 0x1c], ecx
004846fc  cmp        byte ptr [ecx - 1], bl
004846ff  jne        0x4846b9

// block 00484701
00484701  push       0xfe
00484706  lea        ecx, [eax + 0x200]
0048470c  push       ecx
0048470d  push       eax
0048470e  call       0x47a330

// block 00484713
00484713  push       0x7f
00484715  lea        eax, [edi + 0x100]
0048471b  push       eax
0048471c  push       edi
0048471d  call       0x47a330

// block 00484722
00484722  mov        eax, dword ptr [ebp - 0x28]
00484725  push       0x7f
00484727  lea        ecx, [eax + 0x100]
0048472d  push       ecx
0048472e  push       eax
0048472f  call       0x47a330

// block 00484734
00484734  mov        eax, dword ptr [esi + 0xc0]
0048473a  add        esp, 0x24
0048473d  cmp        eax, ebx
0048473f  je         0x48478c

// block 00484741
00484741  push       eax
00484742  call       dword ptr [0x49815c]

// block 00484748
00484748  test       eax, eax
0048474a  jne        0x48478c

// block 0048474c
0048474c  mov        eax, dword ptr [esi + 0xc4]
00484752  sub        eax, 0xfe
00484757  push       eax
00484758  call       0x46c941

// block 0048475d
0048475d  mov        eax, dword ptr [esi + 0xcc]
00484763  mov        edi, 0x80
00484768  sub        eax, edi
0048476a  push       eax
0048476b  call       0x46c941

// block 00484770
00484770  mov        eax, dword ptr [esi + 0xd0]
00484776  sub        eax, edi
00484778  push       eax
00484779  call       0x46c941

// block 0048477e
0048477e  push       dword ptr [esi + 0xc0]
00484784  call       0x46c941

// block 00484789
00484789  add        esp, 0x10

// block 0048478c
0048478c  mov        eax, dword ptr [ebp - 0x2c]
0048478f  mov        dword ptr [eax], 1
00484795  mov        dword ptr [esi + 0xc0], eax
0048479b  mov        eax, dword ptr [ebp - 0x40]
0048479e  mov        dword ptr [esi + 0xc8], eax
004847a4  mov        eax, dword ptr [ebp - 0x3c]
004847a7  mov        dword ptr [esi + 0xc4], eax
004847ad  mov        eax, dword ptr [ebp - 0x44]
004847b0  mov        dword ptr [esi + 0xcc], eax
004847b6  mov        eax, dword ptr [ebp - 0x38]
004847b9  mov        dword ptr [esi + 0xd0], eax
004847bf  mov        eax, dword ptr [ebp - 0x30]
004847c2  mov        dword ptr [esi + 0xac], eax

// block 004847c8
004847c8  push       dword ptr [ebp - 0x24]
004847cb  call       0x46c941

// block 004847d0
004847d0  pop        ecx
004847d1  mov        eax, ebx
004847d3  jmp        0x484842

// block 004847d5
004847d5  push       dword ptr [ebp - 0x2c]
004847d8  call       0x46c941

// block 004847dd
004847dd  push       dword ptr [ebp - 0x1c]
004847e0  call       0x46c941

// block 004847e5
004847e5  push       dword ptr [ebp - 0x20]
004847e8  call       0x46c941

// block 004847ed
004847ed  push       dword ptr [ebp - 0x28]
004847f0  call       0x46c941

// block 004847f5
004847f5  xor        ebx, ebx
004847f7  add        esp, 0x10
004847fa  inc        ebx
004847fb  jmp        0x4847c8

// block 004847fd
004847fd  lea        edi, [esi + 0xc0]
00484803  mov        eax, dword ptr [edi]
00484805  cmp        eax, ebx
00484807  je         0x484810

// block 00484809
00484809  push       eax
0048480a  call       dword ptr [0x49815c]

// block 00484810
00484810  mov        dword ptr [edi], ebx
00484812  mov        dword ptr [esi + 0xc4], ebx
00484818  mov        dword ptr [esi + 0xc8], 0x49e2d0
00484822  mov        dword ptr [esi + 0xcc], 0x49e758
0048482c  mov        dword ptr [esi + 0xd0], 0x49e8d8
00484836  mov        dword ptr [esi + 0xac], 1
00484840  xor        eax, eax

// block 00484842
00484842  mov        ecx, dword ptr [ebp - 4]
00484845  pop        edi
00484846  pop        esi
00484847  xor        ecx, ebp
00484849  pop        ebx
0048484a  call       0x46c932

// block 0048484f
0048484f  leave      
00484850  ret        
