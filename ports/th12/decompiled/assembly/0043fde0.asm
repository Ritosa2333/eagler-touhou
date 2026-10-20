
// block 0043fde0
0043fde0  push       ebp
0043fde1  mov        ebp, dword ptr [esp + 8]
0043fde5  mov        eax, dword ptr [ebp + 0x24]
0043fde8  cmp        eax, 4
0043fdeb  ja         0x441164

// block 0043fdf1
0043fdf1  push       ebx
0043fdf2  push       esi
0043fdf3  push       edi
0043fdf4  jmp        dword ptr [eax*4 + 0x441170]

// block 0043fdfb
0043fdfb  mov        dword ptr [ebp + 0x30], 8
0043fe02  call       0x43f180

// block 0043fe07
0043fe07  test       eax, eax
0043fe09  jne        0x43fe23

// block 0043fe0b
0043fe0b  mov        ecx, dword ptr [ebp + 0xfc]
0043fe11  mov        eax, 1
0043fe16  mov        dword ptr [ebp + ecx*4 + 0xb8], eax
0043fe1d  add        dword ptr [ebp + 0xfc], eax

// block 0043fe23
0043fe23  test       byte ptr [0x4b0ce0], 0x10
0043fe2a  je         0x43fe56

// block 0043fe2c
0043fe2c  mov        eax, dword ptr [ebp + 0x30]
0043fe2f  test       eax, eax
0043fe31  je         0x43fe48

// block 0043fe33
0043fe33  cmp        eax, 2
0043fe36  jg         0x43fe3e

// block 0043fe38
0043fe38  dec        eax
0043fe39  mov        dword ptr [ebp + 0x28], eax
0043fe3c  jmp        0x43fe4f

// block 0043fe3e
0043fe3e  mov        eax, 2
0043fe43  mov        dword ptr [ebp + 0x28], eax
0043fe46  jmp        0x43fe4f

// block 0043fe48
0043fe48  mov        dword ptr [ebp + 0x28], 2

// block 0043fe4f
0043fe4f  and        dword ptr [0x4b0ce0], 0xffffffef

// block 0043fe56
0043fe56  mov        ecx, 1
0043fe5b  mov        eax, ebp
0043fe5d  call       0x43ef40

// block 0043fe62
0043fe62  test       byte ptr [ebp + 0x5c18], 2
0043fe69  je         0x43fe8d

// block 0043fe6b
0043fe6b  mov        esi, 0x63
0043fe70  mov        edi, ebp
0043fe72  call       0x43efa0

// block 0043fe77
0043fe77  mov        esi, 0x6a
0043fe7c  call       0x43efa0

// block 0043fe81
0043fe81  and        dword ptr [ebp + 0x5c18], 0xfffffffd
0043fe88  jmp        0x43ff17

// block 0043fe8d
0043fe8d  mov        edx, dword ptr [ebp + 0x454]
0043fe93  push       edx
0043fe94  mov        edx, dword ptr [0x4ce8cc]
0043fe9a  call       0x461920

// block 0043fe9f
0043fe9f  test       eax, eax
0043fea1  jne        0x43febc

// block 0043fea3
0043fea3  lea        esi, [eax + 0x63]
0043fea6  mov        edi, ebp
0043fea8  call       0x43efa0

// block 0043fead
0043fead  mov        eax, dword ptr [ebp + 0x454]
0043feb3  push       eax
0043feb4  lea        ebx, [esi - 0x61]
0043feb7  call       0x4619e0

// block 0043febc
0043febc  cmp        dword ptr [ebp + 0x20], 3
0043fec0  je         0x43fed3

// block 0043fec2
0043fec2  mov        ecx, dword ptr [ebp + 0x454]
0043fec8  push       ecx
0043fec9  mov        ebx, 2
0043fece  call       0x4619e0

// block 0043fed3
0043fed3  mov        edx, dword ptr [ebp + 0x470]
0043fed9  mov        esi, dword ptr [0x4ce8cc]
0043fedf  push       edx
0043fee0  mov        edx, esi
0043fee2  call       0x461920

// block 0043fee7
0043fee7  test       eax, eax
0043fee9  jne        0x43ff07

// block 0043feeb
0043feeb  mov        eax, dword ptr [ebp + 0x474]
0043fef1  push       eax
0043fef2  mov        edx, esi
0043fef4  call       0x461920

// block 0043fef9
0043fef9  test       eax, eax
0043fefb  jne        0x43ff07

// block 0043fefd
0043fefd  lea        esi, [eax + 0x6b]
0043ff00  mov        edi, ebp
0043ff02  call       0x43efa0

// block 0043ff07
0043ff07  push       0xb4
0043ff0c  lea        eax, [ebp + 0x2b4]
0043ff12  call       0x4067e0

// block 0043ff17
0043ff17  cmp        dword ptr [ebp + 0x2b8], 0xb4
0043ff21  jne        0x4402d7

// block 0043ff27
0043ff27  mov        edx, dword ptr [ebp + 0x14]
0043ff2a  xor        edi, edi
0043ff2c  push       edi
0043ff2d  push       edi
0043ff2e  lea        ecx, [esp + 0x1c]
0043ff32  push       ecx
0043ff33  push       edx
0043ff34  lea        eax, [edi + 0x17]
0043ff37  xor        ecx, ecx
0043ff39  call       0x4615a0

// block 0043ff3e
0043ff3e  mov        eax, dword ptr [esp + 0x14]
0043ff42  mov        edx, dword ptr [0x4ce8cc]
0043ff48  mov        dword ptr [ebp + 0x2c8], eax
0043ff4e  mov        ecx, dword ptr [ebp + 0x720]
0043ff54  push       ecx
0043ff55  call       0x461920

// block 0043ff5a
0043ff5a  cmp        eax, edi
0043ff5c  jne        0x43ff83

// block 0043ff5e
0043ff5e  push       edi
0043ff5f  push       edi
0043ff60  lea        edx, [esp + 0x1c]
0043ff64  mov        dword ptr [ebp + 0x720], edi
0043ff6a  mov        eax, dword ptr [ebp + 0x18]
0043ff6d  push       edx
0043ff6e  push       eax
0043ff6f  lea        eax, [edi + 0x17]
0043ff72  xor        ecx, ecx
0043ff74  call       0x4615a0

// block 0043ff79
0043ff79  mov        ecx, dword ptr [esp + 0x14]
0043ff7d  mov        dword ptr [ebp + 0x720], ecx

// block 0043ff83
0043ff83  cmp        dword ptr [ebp + 0x28], edi
0043ff86  jle        0x440034

// block 0043ff8c
0043ff8c  xor        ebx, ebx
0043ff8e  mov        edi, edi

// block 0043ff90
0043ff90  mov        edx, dword ptr [ebp + 0x2c8]
0043ff96  push       edx
0043ff97  mov        edx, dword ptr [0x4ce8cc]
0043ff9d  call       0x461920

// block 0043ffa2
0043ffa2  cmp        eax, ebx
0043ffa4  jne        0x43ffac

// block 0043ffa6
0043ffa6  mov        dword ptr [ebp + 0x2c8], ebx

// block 0043ffac
0043ffac  lea        edx, [edi + 3]
0043ffaf  mov        esi, eax
0043ffb1  call       0x462020

// block 0043ffb6
0043ffb6  mov        esi, eax
0043ffb8  mov        eax, dword ptr [esi + 0x494]
0043ffbe  cmp        eax, ebx
0043ffc0  je         0x43ffcb

// block 0043ffc2
0043ffc2  mov        edx, 0x1e
0043ffc7  mov        ecx, esi
0043ffc9  call       eax

// block 0043ffcb
0043ffcb  mov        eax, 0x1e
0043ffd0  push       esi
0043ffd1  mov        word ptr [esi + 0x3c4], ax
0043ffd8  call       0x455630

// block 0043ffdd
0043ffdd  mov        ecx, dword ptr [ebp + 0x2c8]
0043ffe3  mov        edx, dword ptr [0x4ce8cc]
0043ffe9  push       ecx
0043ffea  call       0x461920

// block 0043ffef
0043ffef  cmp        eax, ebx
0043fff1  jne        0x43fff9

// block 0043fff3
0043fff3  mov        dword ptr [ebp + 0x2c8], ebx

// block 0043fff9
0043fff9  lea        edx, [edi + 0xb]
0043fffc  mov        esi, eax
0043fffe  call       0x462020

// block 00440003
00440003  mov        esi, eax
00440005  mov        eax, dword ptr [esi + 0x494]
0044000b  cmp        eax, ebx
0044000d  je         0x440018

// block 0044000f
0044000f  mov        edx, 0x1e
00440014  mov        ecx, esi
00440016  call       eax

// block 00440018
00440018  mov        edx, 0x1e
0044001d  push       esi
0044001e  mov        word ptr [esi + 0x3c4], dx
00440025  call       0x455630

// block 0044002a
0044002a  inc        edi
0044002b  cmp        edi, dword ptr [ebp + 0x28]
0044002e  jl         0x43ff90

// block 00440034
00440034  inc        edi
00440035  cmp        edi, 8
00440038  jge        0x4401b6

// block 0044003e
0044003e  add        edi, 0xb

// block 00440041
00440041  mov        ecx, dword ptr [ebp + 0x2c8]
00440047  mov        edx, dword ptr [0x4ce8cc]
0044004d  lea        esi, [edi - 8]
00440050  test       ecx, ecx
00440052  jne        0x440058

// block 00440054
00440054  xor        eax, eax
00440056  jmp        0x440099

// block 00440058
00440058  mov        eax, dword ptr [edx + 0x8856b8]
0044005e  test       eax, eax
00440060  je         0x44006f

// block 00440062
00440062  mov        ebx, dword ptr [eax]
00440064  cmp        dword ptr [ebx], ecx
00440066  je         0x44008f

// block 00440068
00440068  mov        eax, dword ptr [eax + 4]
0044006b  test       eax, eax
0044006d  jne        0x440062

// block 0044006f
0044006f  mov        eax, dword ptr [edx + 0x8856c0]
00440075  test       eax, eax
00440077  je         0x440054

// block 00440079
00440079  lea        esp, [esp]

// block 00440080
00440080  mov        edx, dword ptr [eax]
00440082  cmp        dword ptr [edx], ecx
00440084  je         0x440093

// block 00440086
00440086  mov        eax, dword ptr [eax + 4]
00440089  test       eax, eax
0044008b  jne        0x440080

// block 0044008d
0044008d  jmp        0x440099

// block 0044008f
0044008f  mov        eax, ebx
00440091  jmp        0x440095

// block 00440093
00440093  mov        eax, edx

// block 00440095
00440095  test       eax, eax
00440097  jne        0x4400a3

// block 00440099
00440099  mov        dword ptr [ebp + 0x2c8], 0

// block 004400a3
004400a3  lea        ecx, [eax + 0x10]
004400a6  test       ecx, ecx
004400a8  je         0x4400cd

// block 004400aa
004400aa  lea        ebx, [ebx]

// block 004400b0
004400b0  mov        edx, dword ptr [ecx]
004400b2  movsx      ebx, word ptr [edx + 0x3ea]
004400b9  cmp        ebx, esi
004400bb  je         0x440102

// block 004400bd
004400bd  cmp        esi, -1
004400c0  jne        0x4400c6

// block 004400c2
004400c2  cmp        edx, eax
004400c4  jne        0x440102

// block 004400c6
004400c6  mov        ecx, dword ptr [ecx + 4]
004400c9  test       ecx, ecx
004400cb  jne        0x4400b0

// block 004400cd
004400cd  xor        esi, esi

// block 004400cf
004400cf  mov        eax, dword ptr [esi + 0x494]
004400d5  test       eax, eax
004400d7  je         0x4400e2

// block 004400d9
004400d9  mov        edx, 0x1f
004400de  mov        ecx, esi
004400e0  call       eax

// block 004400e2
004400e2  mov        eax, 0x1f
004400e7  push       esi
004400e8  mov        word ptr [esi + 0x3c4], ax
004400ef  call       0x455630

// block 004400f4
004400f4  mov        ecx, dword ptr [ebp + 0x2c8]
004400fa  test       ecx, ecx
004400fc  jne        0x440106

// block 004400fe
004400fe  xor        eax, eax
00440100  jmp        0x440149

// block 00440102
00440102  mov        esi, edx
00440104  jmp        0x4400cf

// block 00440106
00440106  mov        edx, dword ptr [0x4ce8cc]
0044010c  mov        eax, dword ptr [edx + 0x8856b8]
00440112  test       eax, eax
00440114  je         0x440123

// block 00440116
00440116  mov        esi, dword ptr [eax]
00440118  cmp        dword ptr [esi], ecx
0044011a  je         0x44013f

// block 0044011c
0044011c  mov        eax, dword ptr [eax + 4]
0044011f  test       eax, eax
00440121  jne        0x440116

// block 00440123
00440123  mov        eax, dword ptr [edx + 0x8856c0]
00440129  test       eax, eax
0044012b  je         0x4400fe

// block 0044012d
0044012d  lea        ecx, [ecx]

// block 00440130
00440130  mov        edx, dword ptr [eax]
00440132  cmp        dword ptr [edx], ecx
00440134  je         0x440143

// block 00440136
00440136  mov        eax, dword ptr [eax + 4]
00440139  test       eax, eax
0044013b  jne        0x440130

// block 0044013d
0044013d  jmp        0x440149

// block 0044013f
0044013f  mov        eax, esi
00440141  jmp        0x440145

// block 00440143
00440143  mov        eax, edx

// block 00440145
00440145  test       eax, eax
00440147  jne        0x440153

// block 00440149
00440149  mov        dword ptr [ebp + 0x2c8], 0

// block 00440153
00440153  lea        ecx, [eax + 0x10]
00440156  test       ecx, ecx
00440158  je         0x440185

// block 0044015a
0044015a  lea        ebx, [ebx]

// block 00440160
00440160  mov        edx, dword ptr [ecx]
00440162  movsx      esi, word ptr [edx + 0x3ea]
00440169  cmp        esi, edi
0044016b  je         0x440381

// block 00440171
00440171  cmp        edi, -1
00440174  jne        0x44017e

// block 00440176
00440176  cmp        edx, eax
00440178  jne        0x440381

// block 0044017e
0044017e  mov        ecx, dword ptr [ecx + 4]
00440181  test       ecx, ecx
00440183  jne        0x440160

// block 00440185
00440185  xor        esi, esi

// block 00440187
00440187  mov        eax, dword ptr [esi + 0x494]
0044018d  test       eax, eax
0044018f  je         0x44019a

// block 00440191
00440191  mov        edx, 0x1f
00440196  mov        ecx, esi
00440198  call       eax

// block 0044019a
0044019a  mov        eax, 0x1f
0044019f  push       esi
004401a0  mov        word ptr [esi + 0x3c4], ax
004401a7  call       0x455630

// block 004401ac
004401ac  inc        edi
004401ad  cmp        edi, 0x13
004401b0  jl         0x440041

// block 004401b6
004401b6  mov        eax, dword ptr [0x4b451c]
004401bb  test       byte ptr [eax + 0x1e9d0], 0x10
004401c2  jne        0x4402d7

// block 004401c8
004401c8  test       byte ptr [eax + 0x1e9d1], 0x10
004401cf  jne        0x4402d7

// block 004401d5
004401d5  test       byte ptr [eax + 0x1e9d2], 0x10
004401dc  jne        0x4402d7

// block 004401e2
004401e2  test       byte ptr [eax + 0x1e9d3], 0x10
004401e9  jne        0x4402d7

// block 004401ef
004401ef  test       byte ptr [eax + 0x1e9d4], 0x10
004401f6  jne        0x4402d7

// block 004401fc
004401fc  test       byte ptr [eax + 0x1e9d5], 0x10
00440203  jne        0x4402d7

// block 00440209
00440209  mov        ecx, dword ptr [ebp + 0x2c8]
0044020f  mov        edx, dword ptr [0x4ce8cc]
00440215  push       ecx
00440216  call       0x461920

// block 0044021b
0044021b  xor        edi, edi
0044021d  cmp        eax, edi
0044021f  jne        0x440227

// block 00440221
00440221  mov        dword ptr [ebp + 0x2c8], edi

// block 00440227
00440227  add        eax, 0x10
0044022a  cmp        eax, edi
0044022c  je         0x440256

// block 0044022e
0044022e  mov        ecx, 4
00440233  jmp        0x440240

// block 00440240
00440240  mov        edx, dword ptr [eax]
00440242  cmp        word ptr [edx + 0x3ea], cx
00440249  je         0x440388

// block 0044024f
0044024f  mov        eax, dword ptr [eax + 4]
00440252  cmp        eax, edi
00440254  jne        0x440240

// block 00440256
00440256  xor        esi, esi

// block 00440258
00440258  mov        eax, dword ptr [esi + 0x494]
0044025e  cmp        eax, edi
00440260  je         0x44026b

// block 00440262
00440262  mov        edx, 0x1d
00440267  mov        ecx, esi
00440269  call       eax

// block 0044026b
0044026b  mov        edx, dword ptr [0x4ce8cc]
00440271  mov        eax, 0x1d
00440276  mov        word ptr [esi + 0x3c4], ax
0044027d  mov        ecx, dword ptr [ebp + 0x2c8]
00440283  push       ecx
00440284  call       0x461920

// block 00440289
00440289  cmp        eax, edi
0044028b  jne        0x440293

// block 0044028d
0044028d  mov        dword ptr [ebp + 0x2c8], edi

// block 00440293
00440293  add        eax, 0x10
00440296  cmp        eax, edi
00440298  je         0x4402b6

// block 0044029a
0044029a  mov        ecx, 0xc
0044029f  nop        

// block 004402a0
004402a0  mov        edx, dword ptr [eax]
004402a2  cmp        word ptr [edx + 0x3ea], cx
004402a9  je         0x44038f

// block 004402af
004402af  mov        eax, dword ptr [eax + 4]
004402b2  cmp        eax, edi
004402b4  jne        0x4402a0

// block 004402b6
004402b6  xor        esi, esi

// block 004402b8
004402b8  mov        eax, dword ptr [esi + 0x494]
004402be  cmp        eax, edi
004402c0  je         0x4402cb

// block 004402c2
004402c2  mov        edx, 0x1d
004402c7  mov        ecx, esi
004402c9  call       eax

// block 004402cb
004402cb  mov        eax, 0x1d
004402d0  mov        word ptr [esi + 0x3c4], ax

// block 004402d7
004402d7  cmp        dword ptr [ebp + 0x2b8], 0xbe
004402e1  jle        0x441161

// block 004402e7
004402e7  fldz       
004402e9  mov        dword ptr [ebp + 0x24], 2
004402f0  mov        eax, dword ptr [ebp + 0x2c4]
004402f6  test       al, 1
004402f8  jne        0x440327

// block 004402fa
004402fa  or         eax, 1
004402fd  fst        dword ptr [ebp + 0x2bc]
00440303  mov        dword ptr [ebp + 0x2b8], 0
0044030d  mov        dword ptr [ebp + 0x2b4], 0xfff0bdc1
00440317  mov        dword ptr [ebp + 0x2c0], 0x4b2ed0
00440321  mov        dword ptr [ebp + 0x2c4], eax

// block 00440327
00440327  xor        edi, edi
00440329  fstp       dword ptr [ebp + 0x2bc]
0044032f  mov        dword ptr [ebp + 0x2b8], edi
00440335  mov        dword ptr [ebp + 0x2b4], 0xffffffff
0044033f  mov        ecx, dword ptr [ebp + 0x2c8]
00440345  push       ecx
00440346  lea        ebx, [edi + 3]
00440349  call       0x4619e0

// block 0044034e
0044034e  movzx      ebx, word ptr [ebp + 0x28]
00440352  mov        edx, dword ptr [ebp + 0x2c8]
00440358  add        bx, 0x11
0044035c  push       edx
0044035d  call       0x461970

// block 00440362
00440362  cmp        dword ptr [ebp + 0x28], edi
00440365  jle        0x4404ef

// block 0044036b
0044036b  jmp        0x440370

// block 00440370
00440370  mov        ecx, dword ptr [ebp + 0x2c8]
00440376  lea        esi, [edi + 3]
00440379  test       ecx, ecx
0044037b  jne        0x440396

// block 0044037d
0044037d  xor        eax, eax
0044037f  jmp        0x4403d9

// block 00440381
00440381  mov        esi, edx
00440383  jmp        0x440187

// block 00440388
00440388  mov        esi, edx
0044038a  jmp        0x440258

// block 0044038f
0044038f  mov        esi, edx
00440391  jmp        0x4402b8

// block 00440396
00440396  mov        edx, dword ptr [0x4ce8cc]
0044039c  mov        eax, dword ptr [edx + 0x8856b8]
004403a2  test       eax, eax
004403a4  je         0x4403b3

// block 004403a6
004403a6  mov        ebx, dword ptr [eax]
004403a8  cmp        dword ptr [ebx], ecx
004403aa  je         0x4403cf

// block 004403ac
004403ac  mov        eax, dword ptr [eax + 4]
004403af  test       eax, eax
004403b1  jne        0x4403a6

// block 004403b3
004403b3  mov        eax, dword ptr [edx + 0x8856c0]
004403b9  test       eax, eax
004403bb  je         0x44037d

// block 004403bd
004403bd  lea        ecx, [ecx]

// block 004403c0
004403c0  mov        edx, dword ptr [eax]
004403c2  cmp        dword ptr [edx], ecx
004403c4  je         0x4403d3

// block 004403c6
004403c6  mov        eax, dword ptr [eax + 4]
004403c9  test       eax, eax
004403cb  jne        0x4403c0

// block 004403cd
004403cd  jmp        0x4403d9

// block 004403cf
004403cf  mov        eax, ebx
004403d1  jmp        0x4403d5

// block 004403d3
004403d3  mov        eax, edx

// block 004403d5
004403d5  test       eax, eax
004403d7  jne        0x4403e3

// block 004403d9
004403d9  mov        dword ptr [ebp + 0x2c8], 0

// block 004403e3
004403e3  lea        ecx, [eax + 0x10]
004403e6  test       ecx, ecx
004403e8  je         0x44040d

// block 004403ea
004403ea  lea        ebx, [ebx]

// block 004403f0
004403f0  mov        edx, dword ptr [ecx]
004403f2  movsx      ebx, word ptr [edx + 0x3ea]
004403f9  cmp        ebx, esi
004403fb  je         0x440445

// block 004403fd
004403fd  cmp        esi, -1
00440400  jne        0x440406

// block 00440402
00440402  cmp        edx, eax
00440404  jne        0x440445

// block 00440406
00440406  mov        ecx, dword ptr [ecx + 4]
00440409  test       ecx, ecx
0044040b  jne        0x4403f0

// block 0044040d
0044040d  xor        esi, esi

// block 0044040f
0044040f  mov        eax, dword ptr [esi + 0x494]
00440415  test       eax, eax
00440417  je         0x440422

// block 00440419
00440419  mov        edx, 0x1e
0044041e  mov        ecx, esi
00440420  call       eax

// block 00440422
00440422  mov        eax, 0x1e
00440427  push       esi
00440428  mov        word ptr [esi + 0x3c4], ax
0044042f  call       0x455630

// block 00440434
00440434  mov        ecx, dword ptr [ebp + 0x2c8]
0044043a  lea        esi, [edi + 0xb]
0044043d  test       ecx, ecx
0044043f  jne        0x440449

// block 00440441
00440441  xor        eax, eax
00440443  jmp        0x440490

// block 00440445
00440445  mov        esi, edx
00440447  jmp        0x44040f

// block 00440449
00440449  mov        edx, dword ptr [0x4ce8cc]
0044044f  mov        eax, dword ptr [edx + 0x8856b8]
00440455  test       eax, eax
00440457  je         0x44046d

// block 00440459
00440459  lea        esp, [esp]

// block 00440460
00440460  mov        ebx, dword ptr [eax]
00440462  cmp        dword ptr [ebx], ecx
00440464  je         0x440486

// block 00440466
00440466  mov        eax, dword ptr [eax + 4]
00440469  test       eax, eax
0044046b  jne        0x440460

// block 0044046d
0044046d  mov        eax, dword ptr [edx + 0x8856c0]
00440473  test       eax, eax
00440475  je         0x440441

// block 00440477
00440477  mov        edx, dword ptr [eax]
00440479  cmp        dword ptr [edx], ecx
0044047b  je         0x44048a

// block 0044047d
0044047d  mov        eax, dword ptr [eax + 4]
00440480  test       eax, eax
00440482  jne        0x440477

// block 00440484
00440484  jmp        0x440490

// block 00440486
00440486  mov        eax, ebx
00440488  jmp        0x44048c

// block 0044048a
0044048a  mov        eax, edx

// block 0044048c
0044048c  test       eax, eax
0044048e  jne        0x44049a

// block 00440490
00440490  mov        dword ptr [ebp + 0x2c8], 0

// block 0044049a
0044049a  lea        ecx, [eax + 0x10]
0044049d  test       ecx, ecx
0044049f  je         0x4404be

// block 004404a1
004404a1  mov        edx, dword ptr [ecx]
004404a3  movsx      ebx, word ptr [edx + 0x3ea]
004404aa  cmp        ebx, esi
004404ac  je         0x440511

// block 004404ae
004404ae  cmp        esi, -1
004404b1  jne        0x4404b7

// block 004404b3
004404b3  cmp        edx, eax
004404b5  jne        0x440511

// block 004404b7
004404b7  mov        ecx, dword ptr [ecx + 4]
004404ba  test       ecx, ecx
004404bc  jne        0x4404a1

// block 004404be
004404be  xor        esi, esi

// block 004404c0
004404c0  mov        eax, dword ptr [esi + 0x494]
004404c6  test       eax, eax
004404c8  je         0x4404d3

// block 004404ca
004404ca  mov        edx, 0x1e
004404cf  mov        ecx, esi
004404d1  call       eax

// block 004404d3
004404d3  mov        eax, 0x1e
004404d8  push       esi
004404d9  mov        word ptr [esi + 0x3c4], ax
004404e0  call       0x455630

// block 004404e5
004404e5  inc        edi
004404e6  cmp        edi, dword ptr [ebp + 0x28]
004404e9  jl         0x440370

// block 004404ef
004404ef  inc        edi
004404f0  cmp        edi, 8
004404f3  jge        0x440676

// block 004404f9
004404f9  add        edi, 0xb
004404fc  lea        esp, [esp]

// block 00440500
00440500  mov        ecx, dword ptr [ebp + 0x2c8]
00440506  lea        esi, [edi - 8]
00440509  test       ecx, ecx
0044050b  jne        0x440515

// block 0044050d
0044050d  xor        eax, eax
0044050f  jmp        0x440559

// block 00440511
00440511  mov        esi, edx
00440513  jmp        0x4404c0

// block 00440515
00440515  mov        edx, dword ptr [0x4ce8cc]
0044051b  mov        eax, dword ptr [edx + 0x8856b8]
00440521  test       eax, eax
00440523  je         0x440532

// block 00440525
00440525  mov        ebx, dword ptr [eax]
00440527  cmp        dword ptr [ebx], ecx
00440529  je         0x44054f

// block 0044052b
0044052b  mov        eax, dword ptr [eax + 4]
0044052e  test       eax, eax
00440530  jne        0x440525

// block 00440532
00440532  mov        eax, dword ptr [edx + 0x8856c0]
00440538  test       eax, eax
0044053a  je         0x44050d

// block 0044053c
0044053c  lea        esp, [esp]

// block 00440540
00440540  mov        edx, dword ptr [eax]
00440542  cmp        dword ptr [edx], ecx
00440544  je         0x440553

// block 00440546
00440546  mov        eax, dword ptr [eax + 4]
00440549  test       eax, eax
0044054b  jne        0x440540

// block 0044054d
0044054d  jmp        0x440559

// block 0044054f
0044054f  mov        eax, ebx
00440551  jmp        0x440555

// block 00440553
00440553  mov        eax, edx

// block 00440555
00440555  test       eax, eax
00440557  jne        0x440563

// block 00440559
00440559  mov        dword ptr [ebp + 0x2c8], 0

// block 00440563
00440563  lea        ecx, [eax + 0x10]
00440566  test       ecx, ecx
00440568  je         0x44058d

// block 0044056a
0044056a  lea        ebx, [ebx]

// block 00440570
00440570  mov        edx, dword ptr [ecx]
00440572  movsx      ebx, word ptr [edx + 0x3ea]
00440579  cmp        ebx, esi
0044057b  je         0x4405c2

// block 0044057d
0044057d  cmp        esi, -1
00440580  jne        0x440586

// block 00440582
00440582  cmp        edx, eax
00440584  jne        0x4405c2

// block 00440586
00440586  mov        ecx, dword ptr [ecx + 4]
00440589  test       ecx, ecx
0044058b  jne        0x440570

// block 0044058d
0044058d  xor        esi, esi

// block 0044058f
0044058f  mov        eax, dword ptr [esi + 0x494]
00440595  test       eax, eax
00440597  je         0x4405a2

// block 00440599
00440599  mov        edx, 0x1f
0044059e  mov        ecx, esi
004405a0  call       eax

// block 004405a2
004405a2  mov        eax, 0x1f
004405a7  push       esi
004405a8  mov        word ptr [esi + 0x3c4], ax
004405af  call       0x455630

// block 004405b4
004405b4  mov        ecx, dword ptr [ebp + 0x2c8]
004405ba  test       ecx, ecx
004405bc  jne        0x4405c6

// block 004405be
004405be  xor        eax, eax
004405c0  jmp        0x440609

// block 004405c2
004405c2  mov        esi, edx
004405c4  jmp        0x44058f

// block 004405c6
004405c6  mov        edx, dword ptr [0x4ce8cc]
004405cc  mov        eax, dword ptr [edx + 0x8856b8]
004405d2  test       eax, eax
004405d4  je         0x4405e3

// block 004405d6
004405d6  mov        esi, dword ptr [eax]
004405d8  cmp        dword ptr [esi], ecx
004405da  je         0x4405ff

// block 004405dc
004405dc  mov        eax, dword ptr [eax + 4]
004405df  test       eax, eax
004405e1  jne        0x4405d6

// block 004405e3
004405e3  mov        eax, dword ptr [edx + 0x8856c0]
004405e9  test       eax, eax
004405eb  je         0x4405be

// block 004405ed
004405ed  lea        ecx, [ecx]

// block 004405f0
004405f0  mov        edx, dword ptr [eax]
004405f2  cmp        dword ptr [edx], ecx
004405f4  je         0x440603

// block 004405f6
004405f6  mov        eax, dword ptr [eax + 4]
004405f9  test       eax, eax
004405fb  jne        0x4405f0

// block 004405fd
004405fd  jmp        0x440609

// block 004405ff
004405ff  mov        eax, esi
00440601  jmp        0x440605

// block 00440603
00440603  mov        eax, edx

// block 00440605
00440605  test       eax, eax
00440607  jne        0x440613

// block 00440609
00440609  mov        dword ptr [ebp + 0x2c8], 0

// block 00440613
00440613  lea        ecx, [eax + 0x10]
00440616  test       ecx, ecx
00440618  je         0x440645

// block 0044061a
0044061a  lea        ebx, [ebx]

// block 00440620
00440620  mov        edx, dword ptr [ecx]
00440622  movsx      esi, word ptr [edx + 0x3ea]
00440629  cmp        esi, edi
0044062b  je         0x4406db

// block 00440631
00440631  cmp        edi, -1
00440634  jne        0x44063e

// block 00440636
00440636  cmp        edx, eax
00440638  jne        0x4406db

// block 0044063e
0044063e  mov        ecx, dword ptr [ecx + 4]
00440641  test       ecx, ecx
00440643  jne        0x440620

// block 00440645
00440645  xor        esi, esi

// block 00440647
00440647  mov        eax, dword ptr [esi + 0x494]
0044064d  test       eax, eax
0044064f  je         0x44065a

// block 00440651
00440651  mov        edx, 0x1f
00440656  mov        ecx, esi
00440658  call       eax

// block 0044065a
0044065a  mov        eax, 0x1f
0044065f  push       esi
00440660  mov        word ptr [esi + 0x3c4], ax
00440667  call       0x455630

// block 0044066c
0044066c  inc        edi
0044066d  cmp        edi, 0x13
00440670  jl         0x440500

// block 00440676
00440676  mov        eax, dword ptr [0x4b451c]
0044067b  mov        cl, 0x10
0044067d  test       byte ptr [eax + 0x1e9d0], cl
00440683  jne        0x441161

// block 00440689
00440689  test       byte ptr [eax + 0x1e9d1], cl
0044068f  jne        0x441161

// block 00440695
00440695  test       byte ptr [eax + 0x1e9d2], cl
0044069b  jne        0x441161

// block 004406a1
004406a1  test       byte ptr [eax + 0x1e9d3], cl
004406a7  jne        0x441161

// block 004406ad
004406ad  test       byte ptr [eax + 0x1e9d4], cl
004406b3  jne        0x441161

// block 004406b9
004406b9  test       byte ptr [eax + 0x1e9d5], cl
004406bf  jne        0x441161

// block 004406c5
004406c5  mov        ecx, dword ptr [ebp + 0x2c8]
004406cb  mov        edx, dword ptr [0x4ce8cc]
004406d1  xor        edi, edi
004406d3  cmp        ecx, edi
004406d5  jne        0x4406e2

// block 004406d7
004406d7  xor        eax, eax
004406d9  jmp        0x44071e

// block 004406db
004406db  mov        esi, edx
004406dd  jmp        0x440647

// block 004406e2
004406e2  mov        eax, dword ptr [edx + 0x8856b8]
004406e8  cmp        eax, edi
004406ea  je         0x4406fd

// block 004406ec
004406ec  lea        esp, [esp]

// block 004406f0
004406f0  mov        esi, dword ptr [eax]
004406f2  cmp        dword ptr [esi], ecx
004406f4  je         0x440718

// block 004406f6
004406f6  mov        eax, dword ptr [eax + 4]
004406f9  cmp        eax, edi
004406fb  jne        0x4406f0

// block 004406fd
004406fd  mov        eax, dword ptr [edx + 0x8856c0]
00440703  cmp        eax, edi
00440705  je         0x4406d7

// block 00440707
00440707  mov        esi, dword ptr [eax]
00440709  cmp        dword ptr [esi], ecx
0044070b  je         0x440718

// block 0044070d
0044070d  mov        eax, dword ptr [eax + 4]
00440710  cmp        eax, edi
00440712  jne        0x440707

// block 00440714
00440714  xor        eax, eax
00440716  jmp        0x44071e

// block 00440718
00440718  mov        eax, esi
0044071a  cmp        eax, edi
0044071c  jne        0x440724

// block 0044071e
0044071e  mov        dword ptr [ebp + 0x2c8], edi

// block 00440724
00440724  add        eax, 0x10
00440727  cmp        eax, edi
00440729  je         0x440742

// block 0044072b
0044072b  mov        ecx, 4

// block 00440730
00440730  mov        esi, dword ptr [eax]
00440732  cmp        word ptr [esi + 0x3ea], cx
00440739  je         0x440744

// block 0044073b
0044073b  mov        eax, dword ptr [eax + 4]
0044073e  cmp        eax, edi
00440740  jne        0x440730

// block 00440742
00440742  xor        esi, esi

// block 00440744
00440744  mov        eax, dword ptr [esi + 0x494]
0044074a  cmp        eax, edi
0044074c  je         0x44075d

// block 0044074e
0044074e  mov        edx, 0x1d
00440753  mov        ecx, esi
00440755  call       eax

// block 00440757
00440757  mov        edx, dword ptr [0x4ce8cc]

// block 0044075d
0044075d  mov        ecx, 0x1d
00440762  mov        word ptr [esi + 0x3c4], cx
00440769  mov        ecx, dword ptr [ebp + 0x2c8]
0044076f  cmp        ecx, edi
00440771  jne        0x440777

// block 00440773
00440773  xor        eax, eax
00440775  jmp        0x4407b3

// block 00440777
00440777  mov        eax, dword ptr [edx + 0x8856b8]
0044077d  cmp        eax, edi
0044077f  je         0x44078e

// block 00440781
00440781  mov        esi, dword ptr [eax]
00440783  cmp        dword ptr [esi], ecx
00440785  je         0x4407a9

// block 00440787
00440787  mov        eax, dword ptr [eax + 4]
0044078a  cmp        eax, edi
0044078c  jne        0x440781

// block 0044078e
0044078e  mov        eax, dword ptr [edx + 0x8856c0]
00440794  cmp        eax, edi
00440796  je         0x440773

// block 00440798
00440798  mov        edx, dword ptr [eax]
0044079a  cmp        dword ptr [edx], ecx
0044079c  je         0x4407ad

// block 0044079e
0044079e  mov        eax, dword ptr [eax + 4]
004407a1  cmp        eax, edi
004407a3  jne        0x440798

// block 004407a5
004407a5  xor        eax, eax
004407a7  jmp        0x4407b3

// block 004407a9
004407a9  mov        eax, esi
004407ab  jmp        0x4407af

// block 004407ad
004407ad  mov        eax, edx

// block 004407af
004407af  cmp        eax, edi
004407b1  jne        0x4407b9

// block 004407b3
004407b3  mov        dword ptr [ebp + 0x2c8], edi

// block 004407b9
004407b9  add        eax, 0x10
004407bc  cmp        eax, edi
004407be  je         0x4407d7

// block 004407c0
004407c0  mov        ecx, 0xc

// block 004407c5
004407c5  mov        edx, dword ptr [eax]
004407c7  cmp        word ptr [edx + 0x3ea], cx
004407ce  je         0x440804

// block 004407d0
004407d0  mov        eax, dword ptr [eax + 4]
004407d3  cmp        eax, edi
004407d5  jne        0x4407c5

// block 004407d7
004407d7  xor        esi, esi

// block 004407d9
004407d9  mov        eax, dword ptr [esi + 0x494]
004407df  cmp        eax, edi
004407e1  je         0x4407ec

// block 004407e3
004407e3  mov        edx, 0x1d
004407e8  mov        ecx, esi
004407ea  call       eax

// block 004407ec
004407ec  mov        eax, 0x1d
004407f1  pop        edi
004407f2  mov        word ptr [esi + 0x3c4], ax
004407f9  pop        esi
004407fa  pop        ebx
004407fb  mov        eax, 1
00440800  pop        ebp
00440801  ret        4

// block 00440804
00440804  mov        esi, edx
00440806  jmp        0x4407d9

// block 00440808
00440808  mov        ecx, dword ptr [ebp + 0x28]
0044080b  lea        esi, [ebp + 0x28]
0044080e  mov        al, 0x10
00440810  mov        dword ptr [esi + 4], ecx
00440813  test       byte ptr [0x4d48c4], al
00440819  jne        0x440823

// block 0044081b
0044081b  test       byte ptr [0x4d48c0], al
00440821  je         0x44082c

// block 00440823
00440823  push       -1
00440825  mov        eax, esi
00440827  call       0x464970

// block 0044082c
0044082c  mov        al, 0x20
0044082e  test       byte ptr [0x4d48c4], al
00440834  jne        0x44083e

// block 00440836
00440836  test       byte ptr [0x4d48c0], al
0044083c  je         0x440847

// block 0044083e
0044083e  push       1
00440840  mov        eax, esi
00440842  call       0x464970

// block 00440847
00440847  mov        edx, dword ptr [esi + 4]
0044084a  cmp        edx, dword ptr [esi]
0044084c  je         0x4409ff

// block 00440852
00440852  mov        edx, 0xa
00440857  call       0x453d90

// block 0044085c
0044085c  mov        eax, dword ptr [ebp + 0x2c8]
00440862  push       eax
00440863  mov        ebx, 3
00440868  call       0x4619e0

// block 0044086d
0044086d  movzx      ebx, word ptr [ebp + 0x28]
00440871  mov        ecx, dword ptr [ebp + 0x2c8]
00440877  add        bx, 7
0044087b  push       ecx
0044087c  call       0x461970

// block 00440881
00440881  xor        esi, esi
00440883  cmp        dword ptr [ebp + 0x28], esi
00440886  jle        0x4408b3

// block 00440888
00440888  jmp        0x440890

// block 00440890
00440890  lea        edx, [esi + 3]
00440893  push       edx
00440894  mov        ebx, 0x1e
00440899  xor        eax, eax
0044089b  mov        edi, ebp
0044089d  call       0x43f0a0

// block 004408a2
004408a2  lea        eax, [esi + 0xb]
004408a5  push       eax
004408a6  xor        eax, eax
004408a8  call       0x43f0a0

// block 004408ad
004408ad  inc        esi
004408ae  cmp        esi, dword ptr [ebp + 0x28]
004408b1  jl         0x440890

// block 004408b3
004408b3  inc        esi
004408b4  cmp        esi, 8
004408b7  jge        0x4409b6

// block 004408bd
004408bd  lea        edi, [esi + 0xb]

// block 004408c0
004408c0  mov        ecx, dword ptr [ebp + 0x2c8]
004408c6  mov        edx, dword ptr [0x4ce8cc]
004408cc  push       ecx
004408cd  lea        esi, [edi - 8]
004408d0  call       0x461920

// block 004408d5
004408d5  test       eax, eax
004408d7  jne        0x4408df

// block 004408d9
004408d9  mov        dword ptr [ebp + 0x2c8], eax

// block 004408df
004408df  lea        ecx, [eax + 0x10]
004408e2  test       ecx, ecx
004408e4  je         0x440915

// block 004408e6
004408e6  jmp        0x4408f0

// block 004408f0
004408f0  mov        edx, dword ptr [ecx]
004408f2  movsx      ebx, word ptr [edx + 0x3ea]
004408f9  cmp        ebx, esi
004408fb  je         0x440a35

// block 00440901
00440901  cmp        esi, -1
00440904  jne        0x44090e

// block 00440906
00440906  cmp        edx, eax
00440908  jne        0x440a35

// block 0044090e
0044090e  mov        ecx, dword ptr [ecx + 4]
00440911  test       ecx, ecx
00440913  jne        0x4408f0

// block 00440915
00440915  xor        esi, esi

// block 00440917
00440917  mov        eax, dword ptr [esi + 0x494]
0044091d  test       eax, eax
0044091f  je         0x44092a

// block 00440921
00440921  mov        edx, 0x1f
00440926  mov        ecx, esi
00440928  call       eax

// block 0044092a
0044092a  mov        edx, 0x1f
0044092f  push       esi
00440930  mov        word ptr [esi + 0x3c4], dx
00440937  call       0x455630

// block 0044093c
0044093c  mov        eax, dword ptr [ebp + 0x2c8]
00440942  mov        edx, dword ptr [0x4ce8cc]
00440948  push       eax
00440949  call       0x461920

// block 0044094e
0044094e  test       eax, eax
00440950  jne        0x440958

// block 00440952
00440952  mov        dword ptr [ebp + 0x2c8], eax

// block 00440958
00440958  lea        ecx, [eax + 0x10]
0044095b  test       ecx, ecx
0044095d  je         0x440985

// block 0044095f
0044095f  nop        

// block 00440960
00440960  mov        edx, dword ptr [ecx]
00440962  movsx      esi, word ptr [edx + 0x3ea]
00440969  cmp        esi, edi
0044096b  je         0x440a3c

// block 00440971
00440971  cmp        edi, -1
00440974  jne        0x44097e

// block 00440976
00440976  cmp        edx, eax
00440978  jne        0x440a3c

// block 0044097e
0044097e  mov        ecx, dword ptr [ecx + 4]
00440981  test       ecx, ecx
00440983  jne        0x440960

// block 00440985
00440985  xor        esi, esi

// block 00440987
00440987  mov        eax, dword ptr [esi + 0x494]
0044098d  test       eax, eax
0044098f  je         0x44099a

// block 00440991
00440991  mov        edx, 0x1f
00440996  mov        ecx, esi
00440998  call       eax

// block 0044099a
0044099a  mov        ecx, 0x1f
0044099f  push       esi
004409a0  mov        word ptr [esi + 0x3c4], cx
004409a7  call       0x455630

// block 004409ac
004409ac  inc        edi
004409ad  cmp        edi, 0x13
004409b0  jl         0x4408c0

// block 004409b6
004409b6  mov        eax, dword ptr [0x4b451c]
004409bb  mov        cl, 0x10
004409bd  test       byte ptr [eax + 0x1e9d0], cl
004409c3  jne        0x4409ff

// block 004409c5
004409c5  test       byte ptr [eax + 0x1e9d1], cl
004409cb  jne        0x4409ff

// block 004409cd
004409cd  test       byte ptr [eax + 0x1e9d2], cl
004409d3  jne        0x4409ff

// block 004409d5
004409d5  test       byte ptr [eax + 0x1e9d3], cl
004409db  jne        0x4409ff

// block 004409dd
004409dd  test       byte ptr [eax + 0x1e9d4], cl
004409e3  jne        0x4409ff

// block 004409e5
004409e5  test       byte ptr [eax + 0x1e9d5], cl
004409eb  jne        0x4409ff

// block 004409ed
004409ed  push       4
004409ef  mov        eax, ebp
004409f1  call       0x43f040

// block 004409f6
004409f6  push       0xc
004409f8  mov        eax, ebp
004409fa  call       0x43f040

// block 004409ff
004409ff  test       dword ptr [0x4d48c4], 0x102
00440a09  je         0x440e88

// block 00440a0f
00440a0f  cmp        dword ptr [ebp + 0x28], 7
00440a13  je         0x440f7d

// block 00440a19
00440a19  mov        edx, 9
00440a1e  call       0x453d90

// block 00440a23
00440a23  mov        eax, dword ptr [ebp + 0x30]
00440a26  test       eax, eax
00440a28  je         0x440a4d

// block 00440a2a
00440a2a  cmp        eax, 7
00440a2d  jg         0x440a43

// block 00440a2f
00440a2f  dec        eax
00440a30  mov        dword ptr [ebp + 0x28], eax
00440a33  jmp        0x440a54

// block 00440a35
00440a35  mov        esi, edx
00440a37  jmp        0x440917

// block 00440a3c
00440a3c  mov        esi, edx
00440a3e  jmp        0x440987

// block 00440a43
00440a43  mov        eax, 7
00440a48  mov        dword ptr [ebp + 0x28], eax
00440a4b  jmp        0x440a54

// block 00440a4d
00440a4d  mov        dword ptr [ebp + 0x28], 7

// block 00440a54
00440a54  mov        edx, dword ptr [ebp + 0x2c8]
00440a5a  push       edx
00440a5b  mov        ebx, 3
00440a60  call       0x4619e0

// block 00440a65
00440a65  movzx      ebx, word ptr [ebp + 0x28]
00440a69  mov        eax, dword ptr [ebp + 0x2c8]
00440a6f  add        bx, 7
00440a73  push       eax
00440a74  call       0x461970

// block 00440a79
00440a79  xor        edi, edi
00440a7b  cmp        dword ptr [ebp + 0x28], edi
00440a7e  jle        0x440b80

// block 00440a84
00440a84  jmp        0x440a90

// block 00440a90
00440a90  mov        ecx, dword ptr [ebp + 0x2c8]
00440a96  mov        edx, dword ptr [0x4ce8cc]
00440a9c  push       ecx
00440a9d  lea        esi, [edi + 3]
00440aa0  call       0x461920

// block 00440aa5
00440aa5  test       eax, eax
00440aa7  jne        0x440aaf

// block 00440aa9
00440aa9  mov        dword ptr [ebp + 0x2c8], eax

// block 00440aaf
00440aaf  lea        ecx, [eax + 0x10]
00440ab2  test       ecx, ecx
00440ab4  je         0x440ae5

// block 00440ab6
00440ab6  jmp        0x440ac0

// block 00440ac0
00440ac0  mov        edx, dword ptr [ecx]
00440ac2  movsx      ebx, word ptr [edx + 0x3ea]
00440ac9  cmp        ebx, esi
00440acb  je         0x440ba1

// block 00440ad1
00440ad1  cmp        esi, -1
00440ad4  jne        0x440ade

// block 00440ad6
00440ad6  cmp        edx, eax
00440ad8  jne        0x440ba1

// block 00440ade
00440ade  mov        ecx, dword ptr [ecx + 4]
00440ae1  test       ecx, ecx
00440ae3  jne        0x440ac0

// block 00440ae5
00440ae5  xor        esi, esi

// block 00440ae7
00440ae7  mov        eax, dword ptr [esi + 0x494]
00440aed  test       eax, eax
00440aef  je         0x440afa

// block 00440af1
00440af1  mov        edx, 0x1e
00440af6  mov        ecx, esi
00440af8  call       eax

// block 00440afa
00440afa  mov        edx, 0x1e
00440aff  push       esi
00440b00  mov        word ptr [esi + 0x3c4], dx
00440b07  call       0x455630

// block 00440b0c
00440b0c  mov        eax, dword ptr [ebp + 0x2c8]
00440b12  mov        edx, dword ptr [0x4ce8cc]
00440b18  push       eax
00440b19  lea        esi, [edi + 0xb]
00440b1c  call       0x461920

// block 00440b21
00440b21  test       eax, eax
00440b23  jne        0x440b2b

// block 00440b25
00440b25  mov        dword ptr [ebp + 0x2c8], eax

// block 00440b2b
00440b2b  lea        ecx, [eax + 0x10]
00440b2e  test       ecx, ecx
00440b30  je         0x440b4f

// block 00440b32
00440b32  mov        edx, dword ptr [ecx]
00440b34  movsx      ebx, word ptr [edx + 0x3ea]
00440b3b  cmp        ebx, esi
00440b3d  je         0x440ba8

// block 00440b3f
00440b3f  cmp        esi, -1
00440b42  jne        0x440b48

// block 00440b44
00440b44  cmp        edx, eax
00440b46  jne        0x440ba8

// block 00440b48
00440b48  mov        ecx, dword ptr [ecx + 4]
00440b4b  test       ecx, ecx
00440b4d  jne        0x440b32

// block 00440b4f
00440b4f  xor        esi, esi

// block 00440b51
00440b51  mov        eax, dword ptr [esi + 0x494]
00440b57  test       eax, eax
00440b59  je         0x440b64

// block 00440b5b
00440b5b  mov        edx, 0x1e
00440b60  mov        ecx, esi
00440b62  call       eax

// block 00440b64
00440b64  mov        ecx, 0x1e
00440b69  push       esi
00440b6a  mov        word ptr [esi + 0x3c4], cx
00440b71  call       0x455630

// block 00440b76
00440b76  inc        edi
00440b77  cmp        edi, dword ptr [ebp + 0x28]
00440b7a  jl         0x440a90

// block 00440b80
00440b80  inc        edi
00440b81  cmp        edi, 8
00440b84  jge        0x440d06

// block 00440b8a
00440b8a  add        edi, 0xb
00440b8d  lea        ecx, [ecx]

// block 00440b90
00440b90  mov        ecx, dword ptr [ebp + 0x2c8]
00440b96  lea        esi, [edi - 8]
00440b99  test       ecx, ecx
00440b9b  jne        0x440bac

// block 00440b9d
00440b9d  xor        eax, eax
00440b9f  jmp        0x440bf0

// block 00440ba1
00440ba1  mov        esi, edx
00440ba3  jmp        0x440ae7

// block 00440ba8
00440ba8  mov        esi, edx
00440baa  jmp        0x440b51

// block 00440bac
00440bac  mov        edx, dword ptr [0x4ce8cc]
00440bb2  mov        eax, dword ptr [edx + 0x8856b8]
00440bb8  test       eax, eax
00440bba  je         0x440bcd

// block 00440bbc
00440bbc  lea        esp, [esp]

// block 00440bc0
00440bc0  mov        ebx, dword ptr [eax]
00440bc2  cmp        dword ptr [ebx], ecx
00440bc4  je         0x440be6

// block 00440bc6
00440bc6  mov        eax, dword ptr [eax + 4]
00440bc9  test       eax, eax
00440bcb  jne        0x440bc0

// block 00440bcd
00440bcd  mov        eax, dword ptr [edx + 0x8856c0]
00440bd3  test       eax, eax
00440bd5  je         0x440b9d

// block 00440bd7
00440bd7  mov        edx, dword ptr [eax]
00440bd9  cmp        dword ptr [edx], ecx
00440bdb  je         0x440bea

// block 00440bdd
00440bdd  mov        eax, dword ptr [eax + 4]
00440be0  test       eax, eax
00440be2  jne        0x440bd7

// block 00440be4
00440be4  jmp        0x440bf0

// block 00440be6
00440be6  mov        eax, ebx
00440be8  jmp        0x440bec

// block 00440bea
00440bea  mov        eax, edx

// block 00440bec
00440bec  test       eax, eax
00440bee  jne        0x440bfa

// block 00440bf0
00440bf0  mov        dword ptr [ebp + 0x2c8], 0

// block 00440bfa
00440bfa  lea        ecx, [eax + 0x10]
00440bfd  test       ecx, ecx
00440bff  je         0x440c1e

// block 00440c01
00440c01  mov        edx, dword ptr [ecx]
00440c03  movsx      ebx, word ptr [edx + 0x3ea]
00440c0a  cmp        ebx, esi
00440c0c  je         0x440c53

// block 00440c0e
00440c0e  cmp        esi, -1
00440c11  jne        0x440c17

// block 00440c13
00440c13  cmp        edx, eax
00440c15  jne        0x440c53

// block 00440c17
00440c17  mov        ecx, dword ptr [ecx + 4]
00440c1a  test       ecx, ecx
00440c1c  jne        0x440c01

// block 00440c1e
00440c1e  xor        esi, esi

// block 00440c20
00440c20  mov        eax, dword ptr [esi + 0x494]
00440c26  test       eax, eax
00440c28  je         0x440c33

// block 00440c2a
00440c2a  mov        edx, 0x1f
00440c2f  mov        ecx, esi
00440c31  call       eax

// block 00440c33
00440c33  mov        eax, 0x1f
00440c38  push       esi
00440c39  mov        word ptr [esi + 0x3c4], ax
00440c40  call       0x455630

// block 00440c45
00440c45  mov        ecx, dword ptr [ebp + 0x2c8]
00440c4b  test       ecx, ecx
00440c4d  jne        0x440c57

// block 00440c4f
00440c4f  xor        eax, eax
00440c51  jmp        0x440c99

// block 00440c53
00440c53  mov        esi, edx
00440c55  jmp        0x440c20

// block 00440c57
00440c57  mov        edx, dword ptr [0x4ce8cc]
00440c5d  mov        eax, dword ptr [edx + 0x8856b8]
00440c63  test       eax, eax
00440c65  je         0x440c74

// block 00440c67
00440c67  mov        esi, dword ptr [eax]
00440c69  cmp        dword ptr [esi], ecx
00440c6b  je         0x440c8f

// block 00440c6d
00440c6d  mov        eax, dword ptr [eax + 4]
00440c70  test       eax, eax
00440c72  jne        0x440c67

// block 00440c74
00440c74  mov        eax, dword ptr [edx + 0x8856c0]
00440c7a  test       eax, eax
00440c7c  je         0x440c4f

// block 00440c7e
00440c7e  mov        edi, edi

// block 00440c80
00440c80  mov        edx, dword ptr [eax]
00440c82  cmp        dword ptr [edx], ecx
00440c84  je         0x440c93

// block 00440c86
00440c86  mov        eax, dword ptr [eax + 4]
00440c89  test       eax, eax
00440c8b  jne        0x440c80

// block 00440c8d
00440c8d  jmp        0x440c99

// block 00440c8f
00440c8f  mov        eax, esi
00440c91  jmp        0x440c95

// block 00440c93
00440c93  mov        eax, edx

// block 00440c95
00440c95  test       eax, eax
00440c97  jne        0x440ca3

// block 00440c99
00440c99  mov        dword ptr [ebp + 0x2c8], 0

// block 00440ca3
00440ca3  lea        ecx, [eax + 0x10]
00440ca6  test       ecx, ecx
00440ca8  je         0x440cd5

// block 00440caa
00440caa  lea        ebx, [ebx]

// block 00440cb0
00440cb0  mov        edx, dword ptr [ecx]
00440cb2  movsx      esi, word ptr [edx + 0x3ea]
00440cb9  cmp        esi, edi
00440cbb  je         0x440d6b

// block 00440cc1
00440cc1  cmp        edi, -1
00440cc4  jne        0x440cce

// block 00440cc6
00440cc6  cmp        edx, eax
00440cc8  jne        0x440d6b

// block 00440cce
00440cce  mov        ecx, dword ptr [ecx + 4]
00440cd1  test       ecx, ecx
00440cd3  jne        0x440cb0

// block 00440cd5
00440cd5  xor        esi, esi

// block 00440cd7
00440cd7  mov        eax, dword ptr [esi + 0x494]
00440cdd  test       eax, eax
00440cdf  je         0x440cea

// block 00440ce1
00440ce1  mov        edx, 0x1f
00440ce6  mov        ecx, esi
00440ce8  call       eax

// block 00440cea
00440cea  mov        eax, 0x1f
00440cef  push       esi
00440cf0  mov        word ptr [esi + 0x3c4], ax
00440cf7  call       0x455630

// block 00440cfc
00440cfc  inc        edi
00440cfd  cmp        edi, 0x13
00440d00  jl         0x440b90

// block 00440d06
00440d06  mov        eax, dword ptr [0x4b451c]
00440d0b  mov        cl, 0x10
00440d0d  test       byte ptr [eax + 0x1e9d0], cl
00440d13  jne        0x440e88

// block 00440d19
00440d19  test       byte ptr [eax + 0x1e9d1], cl
00440d1f  jne        0x440e88

// block 00440d25
00440d25  test       byte ptr [eax + 0x1e9d2], cl
00440d2b  jne        0x440e88

// block 00440d31
00440d31  test       byte ptr [eax + 0x1e9d3], cl
00440d37  jne        0x440e88

// block 00440d3d
00440d3d  test       byte ptr [eax + 0x1e9d4], cl
00440d43  jne        0x440e88

// block 00440d49
00440d49  test       byte ptr [eax + 0x1e9d5], cl
00440d4f  jne        0x440e88

// block 00440d55
00440d55  mov        ecx, dword ptr [ebp + 0x2c8]
00440d5b  mov        edx, dword ptr [0x4ce8cc]
00440d61  xor        edi, edi
00440d63  cmp        ecx, edi
00440d65  jne        0x440d72

// block 00440d67
00440d67  xor        eax, eax
00440d69  jmp        0x440dae

// block 00440d6b
00440d6b  mov        esi, edx
00440d6d  jmp        0x440cd7

// block 00440d72
00440d72  mov        eax, dword ptr [edx + 0x8856b8]
00440d78  cmp        eax, edi
00440d7a  je         0x440d8d

// block 00440d7c
00440d7c  lea        esp, [esp]

// block 00440d80
00440d80  mov        esi, dword ptr [eax]
00440d82  cmp        dword ptr [esi], ecx
00440d84  je         0x440da8

// block 00440d86
00440d86  mov        eax, dword ptr [eax + 4]
00440d89  cmp        eax, edi
00440d8b  jne        0x440d80

// block 00440d8d
00440d8d  mov        eax, dword ptr [edx + 0x8856c0]
00440d93  cmp        eax, edi
00440d95  je         0x440d67

// block 00440d97
00440d97  mov        esi, dword ptr [eax]
00440d99  cmp        dword ptr [esi], ecx
00440d9b  je         0x440da8

// block 00440d9d
00440d9d  mov        eax, dword ptr [eax + 4]
00440da0  cmp        eax, edi
00440da2  jne        0x440d97

// block 00440da4
00440da4  xor        eax, eax
00440da6  jmp        0x440dae

// block 00440da8
00440da8  mov        eax, esi
00440daa  cmp        eax, edi
00440dac  jne        0x440db4

// block 00440dae
00440dae  mov        dword ptr [ebp + 0x2c8], edi

// block 00440db4
00440db4  add        eax, 0x10
00440db7  cmp        eax, edi
00440db9  je         0x440dd2

// block 00440dbb
00440dbb  mov        ecx, 4

// block 00440dc0
00440dc0  mov        esi, dword ptr [eax]
00440dc2  cmp        word ptr [esi + 0x3ea], cx
00440dc9  je         0x440dd4

// block 00440dcb
00440dcb  mov        eax, dword ptr [eax + 4]
00440dce  cmp        eax, edi
00440dd0  jne        0x440dc0

// block 00440dd2
00440dd2  xor        esi, esi

// block 00440dd4
00440dd4  mov        eax, dword ptr [esi + 0x494]
00440dda  cmp        eax, edi
00440ddc  je         0x440ded

// block 00440dde
00440dde  mov        edx, 0x1d
00440de3  mov        ecx, esi
00440de5  call       eax

// block 00440de7
00440de7  mov        edx, dword ptr [0x4ce8cc]

// block 00440ded
00440ded  mov        ecx, 0x1d
00440df2  mov        word ptr [esi + 0x3c4], cx
00440df9  mov        ecx, dword ptr [ebp + 0x2c8]
00440dff  cmp        ecx, edi
00440e01  jne        0x440e07

// block 00440e03
00440e03  xor        eax, eax
00440e05  jmp        0x440e43

// block 00440e07
00440e07  mov        eax, dword ptr [edx + 0x8856b8]
00440e0d  cmp        eax, edi
00440e0f  je         0x440e1e

// block 00440e11
00440e11  mov        esi, dword ptr [eax]
00440e13  cmp        dword ptr [esi], ecx
00440e15  je         0x440e39

// block 00440e17
00440e17  mov        eax, dword ptr [eax + 4]
00440e1a  cmp        eax, edi
00440e1c  jne        0x440e11

// block 00440e1e
00440e1e  mov        eax, dword ptr [edx + 0x8856c0]
00440e24  cmp        eax, edi
00440e26  je         0x440e03

// block 00440e28
00440e28  mov        edx, dword ptr [eax]
00440e2a  cmp        dword ptr [edx], ecx
00440e2c  je         0x440e3d

// block 00440e2e
00440e2e  mov        eax, dword ptr [eax + 4]
00440e31  cmp        eax, edi
00440e33  jne        0x440e28

// block 00440e35
00440e35  xor        eax, eax
00440e37  jmp        0x440e43

// block 00440e39
00440e39  mov        eax, esi
00440e3b  jmp        0x440e3f

// block 00440e3d
00440e3d  mov        eax, edx

// block 00440e3f
00440e3f  cmp        eax, edi
00440e41  jne        0x440e49

// block 00440e43
00440e43  mov        dword ptr [ebp + 0x2c8], edi

// block 00440e49
00440e49  add        eax, 0x10
00440e4c  cmp        eax, edi
00440e4e  je         0x440e67

// block 00440e50
00440e50  mov        ecx, 0xc

// block 00440e55
00440e55  mov        edx, dword ptr [eax]
00440e57  cmp        word ptr [edx + 0x3ea], cx
00440e5e  je         0x440ebc

// block 00440e60
00440e60  mov        eax, dword ptr [eax + 4]
00440e63  cmp        eax, edi
00440e65  jne        0x440e55

// block 00440e67
00440e67  xor        esi, esi

// block 00440e69
00440e69  mov        eax, dword ptr [esi + 0x494]
00440e6f  cmp        eax, edi
00440e71  je         0x440e7c

// block 00440e73
00440e73  mov        edx, 0x1d
00440e78  mov        ecx, esi
00440e7a  call       eax

// block 00440e7c
00440e7c  mov        eax, 0x1d
00440e81  mov        word ptr [esi + 0x3c4], ax

// block 00440e88
00440e88  test       dword ptr [0x4d48c4], 0x80001
00440e92  je         0x441161

// block 00440e98
00440e98  mov        ecx, dword ptr [ebp + 0x2c8]
00440e9e  push       ecx
00440e9f  mov        ebx, 6
00440ea4  call       0x461970

// block 00440ea9
00440ea9  mov        eax, dword ptr [ebp + 0x28]
00440eac  cmp        eax, 7
00440eaf  ja         0x441161

// block 00440eb5
00440eb5  jmp        dword ptr [eax*4 + 0x441184]

// block 00440ebc
00440ebc  mov        esi, edx
00440ebe  jmp        0x440e69

// block 00440ec0
00440ec0  mov        edx, 7
00440ec5  call       0x453d90

// block 00440eca
00440eca  mov        edx, dword ptr [ebp + 0x470]
00440ed0  push       edx
00440ed1  mov        ebx, 1
00440ed6  call       0x461970

// block 00440edb
00440edb  mov        eax, dword ptr [ebp + 0x474]
00440ee1  xor        esi, esi
00440ee3  push       eax
00440ee4  mov        dword ptr [ebp + 0x470], esi
00440eea  call       0x461970

// block 00440eef
00440eef  mov        dword ptr [ebp + 0x474], esi
00440ef5  mov        ecx, dword ptr [ebp + 0x720]
00440efb  push       ecx
00440efc  call       0x461970

// block 00440f01
00440f01  lea        ecx, [ebx + 3]
00440f04  mov        eax, ebp
00440f06  call       0x43ef40

// block 00440f0b
00440f0b  pop        edi
00440f0c  pop        esi
00440f0d  pop        ebx
00440f0e  mov        eax, 1
00440f13  pop        ebp
00440f14  ret        4

// block 00440f17
00440f17  mov        edx, 7
00440f1c  call       0x453d90

// block 00440f21
00440f21  mov        edx, dword ptr [ebp + 0x470]
00440f27  push       edx
00440f28  mov        ebx, 1
00440f2d  call       0x461970

// block 00440f32
00440f32  mov        eax, dword ptr [ebp + 0x474]
00440f38  xor        esi, esi
00440f3a  push       eax
00440f3b  mov        dword ptr [ebp + 0x470], esi
00440f41  call       0x461970

// block 00440f46
00440f46  mov        dword ptr [ebp + 0x474], esi
00440f4c  mov        ecx, dword ptr [ebp + 0x720]
00440f52  push       ecx
00440f53  call       0x461970

// block 00440f58
00440f58  lea        ecx, [ebx + 3]
00440f5b  mov        eax, ebp
00440f5d  call       0x43ef40

// block 00440f62
00440f62  lea        edx, [ebx + 6]
00440f65  call       0x453d90

// block 00440f6a
00440f6a  pop        edi
00440f6b  pop        esi
00440f6c  pop        ebx
00440f6d  mov        eax, 1
00440f72  pop        ebp
00440f73  ret        4

// block 00440f76
00440f76  mov        edx, 7
00440f7b  jmp        0x440f82

// block 00440f7d
00440f7d  mov        edx, 9

// block 00440f82
00440f82  call       0x453d90

// block 00440f87
00440f87  mov        ecx, 4
00440f8c  mov        eax, ebp
00440f8e  call       0x43ef40

// block 00440f93
00440f93  pop        edi
00440f94  pop        esi
00440f95  pop        ebx
00440f96  mov        eax, 1
00440f9b  pop        ebp
00440f9c  ret        4

// block 00440f9f
00440f9f  cmp        dword ptr [ebp + 0x2b8], 0x14
00440fa6  jl         0x441161

// block 00440fac
00440fac  mov        eax, dword ptr [ebp + 0x28]
00440faf  lea        esi, [ebp + 0x28]
00440fb2  cmp        eax, 7
00440fb5  ja         0x441161

// block 00440fbb
00440fbb  jmp        dword ptr [eax*4 + 0x4411a4]

// block 00440fc2
00440fc2  and        dword ptr [0x4b0ce0], 0xffffffef
00440fc9  jmp        0x441053

// block 00440fce
00440fce  and        dword ptr [0x4b0ce0], 0xffffffef
00440fd5  mov        eax, dword ptr [ebp + 0x454]
00440fdb  push       eax
00440fdc  mov        ebx, 3
00440fe1  call       0x4619e0

// block 00440fe6
00440fe6  lea        edx, [ebx + 2]
00440fe9  mov        eax, ebp
00440feb  call       0x43eee0

// block 00440ff0
00440ff0  mov        eax, esi
00440ff2  call       0x464900

// block 00440ff7
00440ff7  mov        ecx, dword ptr [0x4b0ca8]
00440ffd  mov        eax, 4
00441002  mov        dword ptr [ebp + 0x598c], ecx
00441008  mov        dword ptr [0x4aebd0], eax
0044100d  mov        dword ptr [0x4b0ca8], eax
00441012  mov        eax, dword ptr [esi + 8]
00441015  test       eax, eax
00441017  je         0x44103a

// block 00441019
00441019  jg         0x44102a

// block 0044101b
0044101b  dec        eax
0044101c  pop        edi
0044101d  mov        dword ptr [esi], eax
0044101f  pop        esi
00441020  pop        ebx
00441021  mov        eax, 1
00441026  pop        ebp
00441027  ret        4

// block 0044102a
0044102a  xor        eax, eax
0044102c  pop        edi
0044102d  mov        dword ptr [esi], eax
0044102f  pop        esi
00441030  pop        ebx
00441031  mov        eax, 1
00441036  pop        ebp
00441037  ret        4

// block 0044103a
0044103a  pop        edi
0044103b  mov        dword ptr [esi], 0
00441041  pop        esi
00441042  pop        ebx
00441043  mov        eax, 1
00441048  pop        ebp
00441049  ret        4

// block 0044104c
0044104c  or         dword ptr [0x4b0ce0], 0x10

// block 00441053
00441053  mov        edx, dword ptr [ebp + 0x454]
00441059  push       edx
0044105a  mov        ebx, 3
0044105f  call       0x4619e0

// block 00441064
00441064  lea        edx, [ebx + 2]
00441067  mov        eax, ebp
00441069  call       0x43eee0

// block 0044106e
0044106e  mov        eax, esi
00441070  call       0x464900

// block 00441075
00441075  mov        edi, dword ptr [0x4aebd0]
0044107b  cmp        edi, 4
0044107e  jl         0x441091

// block 00441080
00441080  mov        edi, 1
00441085  mov        dword ptr [0x4aebd0], edi
0044108b  mov        dword ptr [0x4b0ca8], edi

// block 00441091
00441091  mov        ecx, edi
00441093  mov        edx, esi
00441095  call       0x40f790

// block 0044109a
0044109a  mov        dword ptr [0x4b0ca8], edi
004410a0  pop        edi
004410a1  pop        esi
004410a2  pop        ebx
004410a3  mov        eax, 1
004410a8  pop        ebp
004410a9  ret        4

// block 004410ac
004410ac  mov        eax, dword ptr [ebp + 0x454]
004410b2  push       eax
004410b3  mov        ebx, 3
004410b8  call       0x4619e0

// block 004410bd
004410bd  lea        edx, [ebx + 8]
004410c0  mov        eax, ebp
004410c2  call       0x43eee0

// block 004410c7
004410c7  mov        eax, esi
004410c9  call       0x464900

// block 004410ce
004410ce  pop        edi
004410cf  pop        esi
004410d0  pop        ebx
004410d1  mov        eax, 1
004410d6  pop        ebp
004410d7  ret        4

// block 004410da
004410da  mov        ecx, dword ptr [ebp + 0x454]
004410e0  push       ecx
004410e1  mov        ebx, 3
004410e6  call       0x4619e0

// block 004410eb
004410eb  lea        edx, [ebx + 7]
004410ee  mov        eax, ebp
004410f0  call       0x43eee0

// block 004410f5
004410f5  mov        eax, esi
004410f7  call       0x464900

// block 004410fc
004410fc  pop        edi
004410fd  pop        esi
004410fe  pop        ebx
004410ff  mov        eax, 1
00441104  pop        ebp
00441105  ret        4

// block 00441108
00441108  mov        edx, dword ptr [ebp + 0x454]
0044110e  push       edx
0044110f  mov        ebx, 3
00441114  call       0x4619e0

// block 00441119
00441119  lea        edx, [ebx + 0xa]
0044111c  mov        eax, ebp
0044111e  call       0x43eee0

// block 00441123
00441123  mov        eax, esi
00441125  call       0x464900

// block 0044112a
0044112a  pop        edi
0044112b  pop        esi
0044112c  pop        ebx
0044112d  mov        eax, 1
00441132  pop        ebp
00441133  ret        4

// block 00441136
00441136  mov        edx, 3
0044113b  mov        eax, ebp
0044113d  call       0x43eee0

// block 00441142
00441142  mov        eax, esi
00441144  call       0x464900

// block 00441149
00441149  pop        edi
0044114a  pop        esi
0044114b  pop        ebx
0044114c  mov        eax, 1
00441151  pop        ebp
00441152  ret        4

// block 00441155
00441155  mov        edx, 2
0044115a  mov        eax, ebp
0044115c  call       0x43eee0

// block 00441161
00441161  pop        edi
00441162  pop        esi
00441163  pop        ebx

// block 00441164
00441164  mov        eax, 1
00441169  pop        ebp
0044116a  ret        4
