
// block 0047478b
0047478b  mov        edi, edi
0047478d  push       ebp
0047478e  mov        ebp, esp
00474790  sub        esp, 0xb4
00474796  mov        eax, dword ptr [0x4ad138]
0047479b  xor        eax, ebp
0047479d  mov        dword ptr [ebp - 4], eax
004747a0  mov        eax, dword ptr [ebp + 0xc]
004747a3  push       ebx
004747a4  push       esi
004747a5  mov        esi, dword ptr [ebp + 8]
004747a8  push       edi
004747a9  mov        edi, dword ptr [ebp + 0x14]
004747ac  mov        dword ptr [ebp - 0x9c], eax
004747b2  mov        eax, dword ptr [ebp + 0x18]
004747b5  mov        dword ptr [ebp - 0xa4], edi
004747bb  mov        dword ptr [ebp - 0xa0], eax
004747c1  call       0x475467

// block 004747c6
004747c6  add        eax, 0x9c
004747cb  lea        ecx, [eax + 0x28]
004747ce  mov        dword ptr [ebp - 0xac], ecx
004747d4  lea        ecx, [eax + 0x2c]
004747d7  lea        ebx, [eax + 0x20]
004747da  add        eax, 0xaf
004747df  mov        dword ptr [ebp - 0xa8], ecx
004747e5  mov        dword ptr [ebp - 0x98], eax
004747eb  test       esi, esi
004747ed  je         0x4749ab

// block 004747f3
004747f3  cmp        dword ptr [ebp - 0x9c], 0
004747fa  je         0x4749ab

// block 00474800
00474800  cmp        dword ptr [ebp + 0x10], 0
00474804  je         0x4749ab

// block 0047480a
0047480a  cmp        byte ptr [esi], 0x43
0047480d  jne        0x474866

// block 0047480f
0047480f  cmp        byte ptr [esi + 1], 0
00474813  jne        0x474866

// block 00474815
00474815  push       0x49d504
0047481a  push       dword ptr [ebp + 0x10]
0047481d  push       dword ptr [ebp - 0x9c]
00474823  call       0x47a2b9

// block 00474828
00474828  add        esp, 0xc
0047482b  xor        esi, esi
0047482d  test       eax, eax
0047482f  je         0x47483e

// block 00474831
00474831  push       esi
00474832  push       esi
00474833  push       esi
00474834  push       esi
00474835  push       esi
00474836  call       0x470dc6

// block 0047483b
0047483b  add        esp, 0x14

// block 0047483e
0047483e  cmp        edi, esi
00474840  je         0x47484f

// block 00474842
00474842  xor        eax, eax
00474844  mov        word ptr [edi], ax
00474847  mov        word ptr [edi + 2], ax
0047484b  mov        word ptr [edi + 4], ax

// block 0047484f
0047484f  mov        eax, dword ptr [ebp - 0xa0]
00474855  cmp        eax, esi
00474857  je         0x47485b

// block 00474859
00474859  mov        dword ptr [eax], esi

// block 0047485b
0047485b  mov        eax, dword ptr [ebp - 0x9c]
00474861  jmp        0x4749ad

// block 00474866
00474866  push       esi
00474867  call       0x475860

// block 0047486c
0047486c  mov        edi, 0x83
00474871  pop        ecx
00474872  mov        dword ptr [ebp - 0xb0], eax
00474878  cmp        eax, edi
0047487a  jae        0x4748a8

// block 0047487c
0047487c  push       esi
0047487d  push       dword ptr [ebp - 0x98]
00474883  call       0x472ac0

// block 00474888
00474888  pop        ecx
00474889  pop        ecx
0047488a  test       eax, eax
0047488c  je         0x474942

// block 00474892
00474892  push       esi
00474893  push       dword ptr [ebp - 0xa8]
00474899  call       0x472ac0

// block 0047489e
0047489e  pop        ecx
0047489f  pop        ecx
004748a0  test       eax, eax
004748a2  je         0x474942

// block 004748a8
004748a8  and        dword ptr [ebp - 0xb4], 0
004748af  lea        eax, [ebp - 0x94]
004748b5  push       esi
004748b6  push       eax
004748b7  call       0x474478

// block 004748bc
004748bc  pop        ecx
004748bd  pop        ecx
004748be  test       eax, eax
004748c0  jne        0x4749ab

// block 004748c6
004748c6  lea        eax, [ebp - 0x94]
004748cc  push       eax
004748cd  push       ebx
004748ce  push       eax
004748cf  call       0x486001

// block 004748d4
004748d4  add        esp, 0xc
004748d7  test       eax, eax
004748d9  je         0x4749ab

// block 004748df
004748df  movzx      eax, word ptr [ebx + 4]
004748e3  mov        ecx, dword ptr [ebp - 0xac]
004748e9  mov        dword ptr [ecx], eax
004748eb  lea        eax, [ebp - 0x94]
004748f1  push       eax
004748f2  push       edi
004748f3  push       dword ptr [ebp - 0x98]
004748f9  call       0x4745a1

// block 004748fe
004748fe  add        esp, 0xc
00474901  cmp        byte ptr [esi], 0
00474904  je         0x474910

// block 00474906
00474906  mov        eax, dword ptr [ebp - 0xb0]
0047490c  cmp        eax, edi
0047490e  jb         0x47491b

// block 00474910
00474910  mov        eax, dword ptr [ebp - 0xb4]
00474916  mov        esi, 0x49fe19

// block 0047491b
0047491b  inc        eax
0047491c  push       eax
0047491d  push       esi
0047491e  push       edi
0047491f  push       dword ptr [ebp - 0xa8]
00474925  call       0x4830fd

// block 0047492a
0047492a  add        esp, 0x10
0047492d  test       eax, eax
0047492f  je         0x474942

// block 00474931
00474931  xor        esi, esi
00474933  push       esi
00474934  push       esi
00474935  push       esi
00474936  push       esi
00474937  push       esi
00474938  call       0x470dc6

// block 0047493d
0047493d  add        esp, 0x14
00474940  jmp        0x474944

// block 00474942
00474942  xor        esi, esi

// block 00474944
00474944  cmp        dword ptr [ebp - 0xa4], esi
0047494a  je         0x47495d

// block 0047494c
0047494c  push       6
0047494e  push       ebx
0047494f  push       dword ptr [ebp - 0xa4]
00474955  call       0x47a330

// block 0047495a
0047495a  add        esp, 0xc

// block 0047495d
0047495d  cmp        dword ptr [ebp - 0xa0], esi
00474963  je         0x47497b

// block 00474965
00474965  push       4
00474967  push       dword ptr [ebp - 0xac]
0047496d  push       dword ptr [ebp - 0xa0]
00474973  call       0x47a330

// block 00474978
00474978  add        esp, 0xc

// block 0047497b
0047497b  push       dword ptr [ebp - 0x98]
00474981  push       dword ptr [ebp + 0x10]
00474984  push       dword ptr [ebp - 0x9c]
0047498a  call       0x47a2b9

// block 0047498f
0047498f  add        esp, 0xc
00474992  test       eax, eax
00474994  je         0x4749a3

// block 00474996
00474996  push       esi
00474997  push       esi
00474998  push       esi
00474999  push       esi
0047499a  push       esi
0047499b  call       0x470dc6

// block 004749a0
004749a0  add        esp, 0x14

// block 004749a3
004749a3  mov        eax, dword ptr [ebp - 0x98]
004749a9  jmp        0x4749ad

// block 004749ab
004749ab  xor        eax, eax

// block 004749ad
004749ad  mov        ecx, dword ptr [ebp - 4]
004749b0  pop        edi
004749b1  pop        esi
004749b2  xor        ecx, ebp
004749b4  pop        ebx
004749b5  call       0x46c932

// block 004749ba
004749ba  leave      
004749bb  ret        
