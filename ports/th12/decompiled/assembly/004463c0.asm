
// block 004463c0
004463c0  sub        esp, 0xc
004463c3  push       ebp
004463c4  mov        ebp, dword ptr [esp + 0x14]
004463c8  mov        eax, dword ptr [ebp + 0x24]
004463cb  lea        ecx, [eax - 2]
004463ce  mov        eax, 1
004463d3  cmp        ecx, eax
004463d5  ja         0x446554

// block 004463db
004463db  fld        dword ptr [0x4a3f00]
004463e1  push       esi
004463e2  mov        esi, dword ptr [0x4b43b8]
004463e8  fst        dword ptr [esp + 8]
004463ec  fstp       dword ptr [esp + 0xc]
004463f0  mov        dword ptr [esi + 0x18f94], eax
004463f6  cmp        dword ptr [ebp + 0x2b8], 0xa
004463fd  fldz       
004463ff  fstp       dword ptr [esp + 0x10]
00446403  jge        0x44640f

// block 00446405
00446405  cmp        dword ptr [ebp + 0x24], 3
00446409  jne        0x44653a

// block 0044640f
0044640f  fld        dword ptr [0x4a3f74]
00446415  push       ebx
00446416  fstp       dword ptr [esp + 0xc]
0044641a  push       edi
0044641b  fld        dword ptr [0x4a3f70]
00446421  mov        edi, eax
00446423  fstp       dword ptr [esp + 0x14]
00446427  jmp        0x446430

// block 00446430
00446430  mov        ecx, dword ptr [0x4b451c]
00446436  lea        edx, [edi - 1]
00446439  cmp        dword ptr [ebp + 0x28], edx
0044643c  jne        0x4464ab

// block 0044643e
0044643e  mov        eax, dword ptr [0x4b0c94]
00446443  mov        edx, dword ptr [0x4b0c90]
00446449  lea        edx, [eax + edx*2]
0044644c  mov        eax, dword ptr [0x4b0ca8]
00446451  imul       edx, edx, 0x45f4
00446457  lea        eax, [eax + eax*2]
0044645a  lea        eax, [edi + eax*2]
0044645d  add        edx, ecx
0044645f  cmp        byte ptr [edx + eax*8 + 0x5a9], 0
00446467  je         0x44649f

// block 00446469
00446469  cmp        dword ptr [ebp + 0x24], 3
0044646d  jne        0x446493

// block 0044646f
0044646f  mov        edx, dword ptr [ebp + 0x2b8]
00446475  and        edx, 0x80000003
0044647b  jns        0x446482

// block 0044647d
0044647d  dec        edx
0044647e  or         edx, 0xfffffffc
00446481  inc        edx

// block 00446482
00446482  cmp        edx, 2
00446485  jl         0x446493

// block 00446487
00446487  mov        dword ptr [esi + 0x18f80], 0xff000000
00446491  jmp        0x4464b5

// block 00446493
00446493  mov        dword ptr [esi + 0x18f80], 0xffffff00
0044649d  jmp        0x4464b5

// block 0044649f
0044649f  mov        dword ptr [esi + 0x18f80], 0xffdfdfdf
004464a9  jmp        0x4464b5

// block 004464ab
004464ab  mov        dword ptr [esi + 0x18f80], 0xff808080

// block 004464b5
004464b5  mov        eax, dword ptr [0x4b0c94]
004464ba  mov        edx, dword ptr [0x4b0c90]
004464c0  lea        edx, [eax + edx*2]
004464c3  mov        eax, dword ptr [0x4b0ca8]
004464c8  imul       edx, edx, 0x45f4
004464ce  lea        eax, [eax + eax*2]
004464d1  add        edx, ecx
004464d3  lea        eax, [edi + eax*2]
004464d6  cmp        byte ptr [edx + eax*8 + 0x5a9], 0
004464de  lea        eax, [edx + eax*8]
004464e1  lea        ebx, [esp + 0x10]
004464e5  jne        0x4464fe

// block 004464e7
004464e7  mov        ecx, dword ptr [edi*4 + 0x4aee60]
004464ee  push       ecx
004464ef  push       0x4a1f58
004464f4  call       0x4015c0

// block 004464f9
004464f9  add        esp, 8
004464fc  jmp        0x44651a

// block 004464fe
004464fe  mov        edx, dword ptr [eax + 0x5a4]
00446504  mov        eax, dword ptr [edi*4 + 0x4aee60]
0044650b  push       edx
0044650c  push       eax
0044650d  push       0x4a1f68
00446512  call       0x4015c0

// block 00446517
00446517  add        esp, 0xc

// block 0044651a
0044651a  fld        dword ptr [esp + 0x14]
0044651e  mov        esi, dword ptr [0x4b43b8]
00446524  fadd       qword ptr [0x4a3f08]
0044652a  inc        edi
0044652b  cmp        edi, 7
0044652e  fstp       dword ptr [esp + 0x14]
00446532  jl         0x446430

// block 00446538
00446538  pop        edi
00446539  pop        ebx

// block 0044653a
0044653a  mov        dword ptr [esi + 0x18f80], 0xffffffff
00446544  mov        dword ptr [esi + 0x18f94], 0
0044654e  mov        eax, 1
00446553  pop        esi

// block 00446554
00446554  pop        ebp
00446555  add        esp, 0xc
00446558  ret        4
