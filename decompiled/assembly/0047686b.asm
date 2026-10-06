
// block 0047686b
0047686b  push       0x2c
0047686d  push       0x4aad78
00476872  call       0x46fcec

// block 00476877
00476877  xor        ebx, ebx
00476879  mov        dword ptr [ebp - 0x38], ebx
0047687c  mov        dword ptr [ebp - 0x2c], ebx
0047687f  mov        dword ptr [ebp - 0x1c], ebx
00476882  mov        dword ptr [ebp - 0x24], ebx
00476885  mov        dword ptr [ebp - 0x28], ebx
00476888  mov        dword ptr [ebp - 0x20], ebx
0047688b  push       7
0047688d  call       0x46ec9a

// block 00476892
00476892  pop        ecx
00476893  mov        dword ptr [ebp - 4], ebx
00476896  call       0x477413

// block 0047689b
0047689b  mov        dword ptr [ebp - 0x20], eax
0047689e  lea        eax, [ebp - 0x1c]
004768a1  push       eax
004768a2  call       0x477324

// block 004768a7
004768a7  pop        ecx
004768a8  cmp        eax, ebx
004768aa  je         0x4768b9

// block 004768ac
004768ac  push       ebx
004768ad  push       ebx
004768ae  push       ebx
004768af  push       ebx
004768b0  push       ebx
004768b1  call       0x470dc6

// block 004768b6
004768b6  add        esp, 0x14

// block 004768b9
004768b9  lea        eax, [ebp - 0x24]
004768bc  push       eax
004768bd  call       0x4772b2

// block 004768c2
004768c2  pop        ecx
004768c3  cmp        eax, ebx
004768c5  je         0x4768d4

// block 004768c7
004768c7  push       ebx
004768c8  push       ebx
004768c9  push       ebx
004768ca  push       ebx
004768cb  push       ebx
004768cc  call       0x470dc6

// block 004768d1
004768d1  add        esp, 0x14

// block 004768d4
004768d4  lea        eax, [ebp - 0x28]
004768d7  push       eax
004768d8  call       0x4772eb

// block 004768dd
004768dd  pop        ecx
004768de  cmp        eax, ebx
004768e0  je         0x4768ef

// block 004768e2
004768e2  push       ebx
004768e3  push       ebx
004768e4  push       ebx
004768e5  push       ebx
004768e6  push       ebx
004768e7  call       0x470dc6

// block 004768ec
004768ec  add        esp, 0x14

// block 004768ef
004768ef  call       0x484896

// block 004768f4
004768f4  mov        dword ptr [ebp - 0x34], eax
004768f7  mov        dword ptr [0x4b41c4], ebx
004768fd  or         edi, 0xffffffff
00476900  mov        dword ptr [0x4adaec], edi
00476906  mov        dword ptr [0x4adae0], edi
0047690c  push       0x49d580
00476911  call       0x48af42

// block 00476916
00476916  pop        ecx
00476917  mov        esi, eax
00476919  mov        dword ptr [ebp - 0x3c], esi
0047691c  cmp        esi, ebx
0047691e  je         0x47699c

// block 00476920
00476920  cmp        byte ptr [esi], bl
00476922  je         0x47699c

// block 00476924
00476924  mov        eax, dword ptr [0x4b41c8]
00476929  cmp        eax, ebx
0047692b  je         0x47694e

// block 0047692d
0047692d  push       eax
0047692e  push       esi
0047692f  call       0x472ac0

// block 00476934
00476934  pop        ecx
00476935  pop        ecx
00476936  test       eax, eax
00476938  je         0x476a87

// block 0047693e
0047693e  mov        eax, dword ptr [0x4b41c8]
00476943  cmp        eax, ebx
00476945  je         0x47694e

// block 00476947
00476947  push       eax
00476948  call       0x46c941

// block 0047694d
0047694d  pop        ecx

// block 0047694e
0047694e  push       esi
0047694f  call       0x475860

// block 00476954
00476954  inc        eax
00476955  push       eax
00476956  call       0x4777ce

// block 0047695b
0047695b  pop        ecx
0047695c  pop        ecx
0047695d  mov        dword ptr [0x4b41c8], eax
00476962  cmp        eax, ebx
00476964  je         0x476a87

// block 0047696a
0047696a  push       esi
0047696b  push       esi
0047696c  call       0x475860

// block 00476971
00476971  pop        ecx
00476972  inc        eax
00476973  push       eax
00476974  push       dword ptr [0x4b41c8]
0047697a  call       0x47a2b9

// block 0047697f
0047697f  add        esp, 0xc
00476982  cmp        eax, ebx
00476984  je         0x476a8e

// block 0047698a
0047698a  push       ebx
0047698b  push       ebx
0047698c  push       ebx
0047698d  push       ebx
0047698e  push       ebx
0047698f  call       0x470dc6

// block 00476994
00476994  add        esp, 0x14
00476997  jmp        0x476a8e

// block 0047699c
0047699c  mov        eax, dword ptr [0x4b41c8]
004769a1  cmp        eax, ebx
004769a3  je         0x4769b2

// block 004769a5
004769a5  push       eax
004769a6  call       0x46c941

// block 004769ab
004769ab  pop        ecx
004769ac  mov        dword ptr [0x4b41c8], ebx

// block 004769b2
004769b2  push       0x4b4118
004769b7  call       dword ptr [0x498130]

// block 004769bd
004769bd  cmp        eax, edi
004769bf  je         0x476a87

// block 004769c5
004769c5  xor        ecx, ecx
004769c7  inc        ecx
004769c8  mov        dword ptr [0x4b41c4], ecx
004769ce  mov        eax, dword ptr [0x4b4118]
004769d3  imul       eax, eax, 0x3c
004769d6  mov        dword ptr [ebp - 0x1c], eax
004769d9  cmp        word ptr [0x4b415e], bx
004769e0  je         0x4769f0

// block 004769e2
004769e2  mov        edx, dword ptr [0x4b416c]
004769e8  imul       edx, edx, 0x3c
004769eb  add        eax, edx
004769ed  mov        dword ptr [ebp - 0x1c], eax

// block 004769f0
004769f0  cmp        word ptr [0x4b41b2], bx
004769f7  je         0x476a13

// block 004769f9
004769f9  mov        eax, dword ptr [0x4b41c0]
004769fe  cmp        eax, ebx
00476a00  je         0x476a13

// block 00476a02
00476a02  mov        dword ptr [ebp - 0x24], ecx
00476a05  sub        eax, dword ptr [0x4b416c]
00476a0b  imul       eax, eax, 0x3c
00476a0e  mov        dword ptr [ebp - 0x28], eax
00476a11  jmp        0x476a19

// block 00476a13
00476a13  mov        dword ptr [ebp - 0x24], ebx
00476a16  mov        dword ptr [ebp - 0x28], ebx

// block 00476a19
00476a19  lea        eax, [ebp - 0x30]
00476a1c  push       eax
00476a1d  push       ebx
00476a1e  push       0x3f
00476a20  mov        eax, dword ptr [ebp - 0x20]
00476a23  push       dword ptr [eax]
00476a25  push       edi
00476a26  push       0x4b411c
00476a2b  push       ebx
00476a2c  push       dword ptr [ebp - 0x34]
00476a2f  mov        edi, dword ptr [0x498134]
00476a35  call       edi

// block 00476a37
00476a37  test       eax, eax
00476a39  je         0x476a4a

// block 00476a3b
00476a3b  cmp        dword ptr [ebp - 0x30], ebx
00476a3e  jne        0x476a4a

// block 00476a40
00476a40  mov        eax, dword ptr [ebp - 0x20]
00476a43  mov        eax, dword ptr [eax]
00476a45  mov        byte ptr [eax + 0x3f], bl
00476a48  jmp        0x476a51

// block 00476a4a
00476a4a  mov        eax, dword ptr [ebp - 0x20]
00476a4d  mov        eax, dword ptr [eax]
00476a4f  mov        byte ptr [eax], bl

// block 00476a51
00476a51  lea        eax, [ebp - 0x30]
00476a54  push       eax
00476a55  push       ebx
00476a56  push       0x3f
00476a58  mov        eax, dword ptr [ebp - 0x20]
00476a5b  push       dword ptr [eax + 4]
00476a5e  push       -1
00476a60  push       0x4b4170
00476a65  push       ebx
00476a66  push       dword ptr [ebp - 0x34]
00476a69  call       edi

// block 00476a6b
00476a6b  test       eax, eax
00476a6d  je         0x476a7f

// block 00476a6f
00476a6f  cmp        dword ptr [ebp - 0x30], ebx
00476a72  jne        0x476a7f

// block 00476a74
00476a74  mov        eax, dword ptr [ebp - 0x20]
00476a77  mov        eax, dword ptr [eax + 4]
00476a7a  mov        byte ptr [eax + 0x3f], bl
00476a7d  jmp        0x476a87

// block 00476a7f
00476a7f  mov        eax, dword ptr [ebp - 0x20]
00476a82  mov        eax, dword ptr [eax + 4]
00476a85  mov        byte ptr [eax], bl

// block 00476a87
00476a87  mov        dword ptr [ebp - 0x2c], 1

// block 00476a8e
00476a8e  push       dword ptr [ebp - 0x1c]
00476a91  call       0x47685a

// block 00476a96
00476a96  push       dword ptr [ebp - 0x24]
00476a99  call       0x476838

// block 00476a9e
00476a9e  push       dword ptr [ebp - 0x28]
00476aa1  call       0x476849

// block 00476aa6
00476aa6  add        esp, 0xc
00476aa9  mov        dword ptr [ebp - 4], 0xfffffffe
00476ab0  call       0x476b17

// block 00476ab5
00476ab5  cmp        dword ptr [ebp - 0x2c], ebx
00476ab8  jne        0x476bab

// block 00476abe
00476abe  push       3
00476ac0  push       esi
00476ac1  push       0x40
00476ac3  mov        edi, dword ptr [ebp - 0x20]
00476ac6  push       dword ptr [edi]
00476ac8  call       0x4830fd

// block 00476acd
00476acd  add        esp, 0x10
00476ad0  test       eax, eax
00476ad2  je         0x476ae1

// block 00476ad4
00476ad4  push       ebx
00476ad5  push       ebx
00476ad6  push       ebx
00476ad7  push       ebx
00476ad8  push       ebx
00476ad9  call       0x470dc6

// block 00476ade
00476ade  add        esp, 0x14

// block 00476ae1
00476ae1  add        esi, 3
00476ae4  cmp        byte ptr [esi], 0x2d
00476ae7  jne        0x476af1

// block 00476ae9
00476ae9  mov        dword ptr [ebp - 0x38], 1
00476af0  inc        esi

// block 00476af1
00476af1  push       esi
00476af2  call       0x46d114

// block 00476af7
00476af7  pop        ecx
00476af8  imul       eax, eax, 0xe10
00476afe  mov        dword ptr [ebp - 0x1c], eax

// block 00476b01
00476b01  mov        al, byte ptr [esi]
00476b03  cmp        al, 0x2b
00476b05  je         0x476b0f

// block 00476b07
00476b07  cmp        al, 0x30
00476b09  jl         0x476b20

// block 00476b0b
00476b0b  cmp        al, 0x39
00476b0d  jg         0x476b20

// block 00476b0f
00476b0f  inc        esi
00476b10  jmp        0x476b01

// block 00476b20
00476b20  cmp        byte ptr [esi], 0x3a
00476b23  jne        0x476b5d

// block 00476b25
00476b25  inc        esi
00476b26  push       esi
00476b27  call       0x46d114

// block 00476b2c
00476b2c  pop        ecx
00476b2d  imul       eax, eax, 0x3c
00476b30  add        dword ptr [ebp - 0x1c], eax
00476b33  jmp        0x476b3a

// block 00476b35
00476b35  cmp        al, 0x39
00476b37  jg         0x476b40

// block 00476b39
00476b39  inc        esi

// block 00476b3a
00476b3a  mov        al, byte ptr [esi]
00476b3c  cmp        al, 0x30
00476b3e  jge        0x476b35

// block 00476b40
00476b40  cmp        byte ptr [esi], 0x3a
00476b43  jne        0x476b5d

// block 00476b45
00476b45  inc        esi
00476b46  push       esi
00476b47  call       0x46d114

// block 00476b4c
00476b4c  pop        ecx
00476b4d  add        dword ptr [ebp - 0x1c], eax
00476b50  jmp        0x476b57

// block 00476b52
00476b52  cmp        al, 0x39
00476b54  jg         0x476b5d

// block 00476b56
00476b56  inc        esi

// block 00476b57
00476b57  mov        al, byte ptr [esi]
00476b59  cmp        al, 0x30
00476b5b  jge        0x476b52

// block 00476b5d
00476b5d  cmp        dword ptr [ebp - 0x38], ebx
00476b60  je         0x476b65

// block 00476b62
00476b62  neg        dword ptr [ebp - 0x1c]

// block 00476b65
00476b65  movsx      eax, byte ptr [esi]
00476b68  mov        dword ptr [ebp - 0x24], eax
00476b6b  cmp        eax, ebx
00476b6d  je         0x476b92

// block 00476b6f
00476b6f  push       3
00476b71  push       esi
00476b72  push       0x40
00476b74  push       dword ptr [edi + 4]
00476b77  call       0x4830fd

// block 00476b7c
00476b7c  add        esp, 0x10
00476b7f  test       eax, eax
00476b81  je         0x476b97

// block 00476b83
00476b83  push       ebx
00476b84  push       ebx
00476b85  push       ebx
00476b86  push       ebx
00476b87  push       ebx
00476b88  call       0x470dc6

// block 00476b8d
00476b8d  add        esp, 0x14
00476b90  jmp        0x476b97

// block 00476b92
00476b92  mov        eax, dword ptr [edi + 4]
00476b95  mov        byte ptr [eax], bl

// block 00476b97
00476b97  mov        esi, dword ptr [ebp - 0x1c]
00476b9a  call       0x47740d

// block 00476b9f
00476b9f  mov        dword ptr [eax], esi
00476ba1  mov        esi, dword ptr [ebp - 0x24]
00476ba4  call       0x477401

// block 00476ba9
00476ba9  mov        dword ptr [eax], esi

// block 00476bab
00476bab  call       0x46fd31

// block 00476bb0
00476bb0  ret        
