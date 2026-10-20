
// block 004814b0
004814b0  mov        edi, edi
004814b2  push       ebp
004814b3  mov        ebp, esp
004814b5  sub        esp, 0x98
004814bb  push       ebx
004814bc  xor        ebx, ebx
004814be  push       esi
004814bf  mov        esi, dword ptr [ebp + 8]
004814c2  push       edi
004814c3  mov        byte ptr [esi + 4], bl
004814c6  and        dword ptr [esi + 4], 0xffff00ff
004814cd  mov        dword ptr [esi], ebx
004814cf  mov        byte ptr [ebp + 0xb], bl
004814d2  mov        edi, 0x49df1c
004814d7  jmp        0x4816c4

// block 004814dc
004814dc  mov        eax, dword ptr [0x4b4318]
004814e1  mov        al, byte ptr [eax]
004814e3  cmp        al, bl
004814e5  je         0x4816cd

// block 004814eb
004814eb  cmp        al, 0x40
004814ed  je         0x4816cd

// block 004814f3
004814f3  cmp        byte ptr [0x4b4330], bl
004814f9  je         0x481507

// block 004814fb
004814fb  cmp        byte ptr [0x4b4331], bl
00481501  je         0x481719

// block 00481507
00481507  cmp        dword ptr [esi], ebx
00481509  je         0x481543

// block 0048150b
0048150b  push       esi
0048150c  lea        eax, [ebp - 0x90]
00481512  push       edi
00481513  push       eax
00481514  call       0x47ec4a

// block 00481519
00481519  add        esp, 0xc
0048151c  push       eax
0048151d  mov        ecx, esi
0048151f  call       0x47dde0

// block 00481524
00481524  cmp        byte ptr [ebp + 0xb], bl
00481527  je         0x481543

// block 00481529
00481529  push       esi
0048152a  lea        eax, [ebp - 0x78]
0048152d  push       0x5b
0048152f  push       eax
00481530  call       0x47ec02

// block 00481535
00481535  add        esp, 0xc
00481538  push       eax
00481539  mov        ecx, esi
0048153b  call       0x47dde0

// block 00481540
00481540  mov        byte ptr [ebp + 0xb], bl

// block 00481543
00481543  mov        ecx, dword ptr [0x4b4318]
00481549  cmp        byte ptr [ecx], 0x3f
0048154c  jne        0x4816a1

// block 00481552
00481552  inc        ecx
00481553  mov        dword ptr [0x4b4318], ecx
00481559  movsx      eax, byte ptr [ecx]
0048155c  sub        eax, 0x24
0048155f  je         0x481690

// block 00481565
00481565  dec        eax
00481566  je         0x481651

// block 0048156c
0048156c  sub        eax, 0x1a
0048156f  je         0x4815d7

// block 00481571
00481571  dec        eax
00481572  dec        eax
00481573  je         0x481651

// block 00481579
00481579  sub        eax, 8
0048157c  push       esi
0048157d  je         0x481598

// block 0048157f
0048157f  lea        eax, [ebp - 0x88]
00481585  push       eax
00481586  lea        eax, [ebp - 0x98]
0048158c  push       eax
0048158d  call       0x47f32e

// block 00481592
00481592  pop        ecx
00481593  jmp        0x4816b5

// block 00481598
00481598  lea        eax, [ebp - 0x80]
0048159b  push       eax
0048159c  push       0x5d
0048159e  lea        eax, [ebp - 0x40]
004815a1  push       eax
004815a2  push       ebx
004815a3  lea        eax, [ebp - 0x70]
004815a6  inc        ecx
004815a7  push       1
004815a9  push       eax
004815aa  mov        dword ptr [0x4b4318], ecx
004815b0  call       0x48023a

// block 004815b5
004815b5  add        esp, 0xc
004815b8  mov        ecx, eax
004815ba  call       0x47ec6e

// block 004815bf
004815bf  mov        ecx, eax
004815c1  call       0x47e9ae

// block 004815c6
004815c6  push       eax
004815c7  mov        ecx, esi
004815c9  call       0x47dde0

// block 004815ce
004815ce  mov        byte ptr [ebp + 0xb], 1
004815d2  jmp        0x4816c4

// block 004815d7
004815d7  lea        eax, [ecx + 1]
004815da  cmp        byte ptr [eax], 0x5f
004815dd  jne        0x481625

// block 004815df
004815df  cmp        byte ptr [ecx + 2], 0x3f
004815e3  jne        0x481625

// block 004815e5
004815e5  mov        dword ptr [0x4b4318], eax
004815ea  push       esi
004815eb  lea        eax, [ebp - 0x50]
004815ee  push       eax
004815ef  push       ebx
004815f0  lea        eax, [ebp - 0x28]
004815f3  push       ebx
004815f4  push       eax
004815f5  call       0x47fb56

// block 004815fa
004815fa  add        esp, 0xc
004815fd  mov        ecx, eax
004815ff  call       0x47e9ae

// block 00481604
00481604  push       eax
00481605  mov        ecx, esi
00481607  call       0x47dde0

// block 0048160c
0048160c  mov        eax, dword ptr [0x4b4318]
00481611  cmp        byte ptr [eax], 0x40
00481614  jne        0x4816c4

// block 0048161a
0048161a  inc        dword ptr [0x4b4318]
00481620  jmp        0x4816c4

// block 00481625
00481625  push       esi
00481626  lea        eax, [ebp - 0x60]
00481629  push       eax
0048162a  push       0x27
0048162c  lea        eax, [ebp - 0x30]
0048162f  push       eax
00481630  lea        eax, [ebp - 0x38]
00481633  push       eax
00481634  call       0x481298

// block 00481639
00481639  push       eax
0048163a  lea        eax, [ebp - 0x48]
0048163d  push       0x60
0048163f  push       eax
00481640  call       0x47ec02

// block 00481645
00481645  add        esp, 0x10
00481648  mov        ecx, eax
0048164a  call       0x47ec6e

// block 0048164f
0048164f  jmp        0x4816b5

// block 00481651
00481651  push       0x40
00481653  push       0x4b4318
00481658  lea        ecx, [ebp - 8]
0048165b  call       0x47e5d3

// block 00481660
00481660  push       esi
00481661  lea        eax, [ebp - 0x58]
00481664  push       0x49dedc
00481669  push       eax
0048166a  call       0x47ec4a

// block 0048166f
0048166f  add        esp, 0xc
00481672  push       eax
00481673  mov        ecx, esi
00481675  call       0x47dde0

// block 0048167a
0048167a  mov        ecx, dword ptr [0x4b4310]
00481680  cmp        dword ptr [ecx], 9
00481683  je         0x4816c4

// block 00481685
00481685  lea        eax, [ebp - 8]
00481688  push       eax
00481689  call       0x47e32e

// block 0048168e
0048168e  jmp        0x4816c4

// block 00481690
00481690  lea        eax, [ebp - 0x68]
00481693  push       esi
00481694  dec        ecx
00481695  push       eax
00481696  mov        dword ptr [0x4b4318], ecx
0048169c  lea        eax, [ebp - 0x10]
0048169f  jmp        0x4816a9

// block 004816a1
004816a1  lea        eax, [ebp - 0x20]
004816a4  push       esi
004816a5  push       eax
004816a6  lea        eax, [ebp - 0x18]

// block 004816a9
004816a9  push       ebx
004816aa  push       1
004816ac  push       eax
004816ad  call       0x48023a

// block 004816b2
004816b2  add        esp, 0xc

// block 004816b5
004816b5  mov        ecx, eax
004816b7  call       0x47e9ae

// block 004816bc
004816bc  push       eax
004816bd  mov        ecx, esi
004816bf  call       0x47dde0

// block 004816c4
004816c4  cmp        byte ptr [esi + 4], bl
004816c7  je         0x4814dc

// block 004816cd
004816cd  mov        eax, dword ptr [0x4b4318]
004816d2  mov        al, byte ptr [eax]
004816d4  cmp        al, bl
004816d6  je         0x4816e0

// block 004816d8
004816d8  cmp        al, 0x40
004816da  je         0x481719

// block 004816dc
004816dc  push       2
004816de  jmp        0x4816e6

// block 004816e0
004816e0  cmp        dword ptr [esi], ebx
004816e2  jne        0x4816ef

// block 004816e4
004816e4  push       1

// block 004816e6
004816e6  mov        ecx, esi
004816e8  call       0x47e2f7

// block 004816ed
004816ed  jmp        0x481719

// block 004816ef
004816ef  push       esi
004816f0  lea        eax, [ebp - 0x18]
004816f3  push       eax
004816f4  push       edi
004816f5  lea        eax, [ebp - 0x20]
004816f8  push       eax
004816f9  push       1
004816fb  lea        ecx, [ebp - 0x10]
004816fe  call       0x47e1ce

// block 00481703
00481703  mov        ecx, eax
00481705  call       0x47ec92

// block 0048170a
0048170a  mov        ecx, eax
0048170c  call       0x47e9ae

// block 00481711
00481711  push       eax
00481712  mov        ecx, esi
00481714  call       0x47dde0

// block 00481719
00481719  pop        edi
0048171a  mov        eax, esi
0048171c  pop        esi
0048171d  pop        ebx
0048171e  leave      
0048171f  ret        
