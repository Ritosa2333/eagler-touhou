
// block 0048aaac
0048aaac  mov        edi, edi
0048aaae  push       ebp
0048aaaf  mov        ebp, esp
0048aab1  sub        esp, 0x30
0048aab4  mov        eax, dword ptr [0x4ad138]
0048aab9  xor        eax, ebp
0048aabb  mov        dword ptr [ebp - 4], eax
0048aabe  mov        eax, dword ptr [ebp + 8]
0048aac1  push       ebx
0048aac2  push       esi
0048aac3  mov        esi, dword ptr [ebp + 0xc]
0048aac6  push       edi
0048aac7  push       0x16
0048aac9  pop        edi
0048aaca  push       edi
0048aacb  lea        ecx, [ebp - 0x1c]
0048aace  push       ecx
0048aacf  lea        ecx, [ebp - 0x30]
0048aad2  push       ecx
0048aad3  push       dword ptr [eax + 4]
0048aad6  push       dword ptr [eax]
0048aad8  call       0x48e03c

// block 0048aadd
0048aadd  xor        ebx, ebx
0048aadf  add        esp, 0x14
0048aae2  cmp        esi, ebx
0048aae4  jne        0x48ab01

// block 0048aae6
0048aae6  call       0x46e96f

// block 0048aaeb
0048aaeb  push       ebx
0048aaec  push       ebx
0048aaed  push       ebx
0048aaee  push       ebx
0048aaef  push       ebx
0048aaf0  mov        dword ptr [eax], edi
0048aaf2  call       0x470f2d

// block 0048aaf7
0048aaf7  add        esp, 0x14
0048aafa  mov        eax, edi
0048aafc  jmp        0x48ab97

// block 0048ab01
0048ab01  mov        ecx, dword ptr [ebp + 0x10]
0048ab04  cmp        ecx, ebx
0048ab06  jbe        0x48aae6

// block 0048ab08
0048ab08  mov        eax, dword ptr [ebp - 0x2c]
0048ab0b  dec        eax
0048ab0c  mov        dword ptr [ebp - 0x20], eax
0048ab0f  xor        eax, eax
0048ab11  cmp        dword ptr [ebp - 0x30], 0x2d
0048ab15  sete       al
0048ab18  lea        edi, [eax + esi]
0048ab1b  cmp        ecx, -1
0048ab1e  jne        0x48ab24

// block 0048ab20
0048ab20  or         ecx, ecx
0048ab22  jmp        0x48ab26

// block 0048ab24
0048ab24  sub        ecx, eax

// block 0048ab26
0048ab26  lea        eax, [ebp - 0x30]
0048ab29  push       eax
0048ab2a  push       dword ptr [ebp + 0x14]
0048ab2d  push       ecx
0048ab2e  push       edi
0048ab2f  call       0x48dec0

// block 0048ab34
0048ab34  add        esp, 0x10
0048ab37  cmp        eax, ebx
0048ab39  je         0x48ab3f

// block 0048ab3b
0048ab3b  mov        byte ptr [esi], bl
0048ab3d  jmp        0x48ab97

// block 0048ab3f
0048ab3f  mov        eax, dword ptr [ebp - 0x2c]
0048ab42  dec        eax
0048ab43  cmp        dword ptr [ebp - 0x20], eax
0048ab46  setl       cl
0048ab49  cmp        eax, -4
0048ab4c  jl         0x48ab7b

// block 0048ab4e
0048ab4e  cmp        eax, dword ptr [ebp + 0x14]
0048ab51  jge        0x48ab7b

// block 0048ab53
0048ab53  cmp        cl, bl
0048ab55  je         0x48ab61

// block 0048ab57
0048ab57  mov        al, byte ptr [edi]
0048ab59  inc        edi
0048ab5a  test       al, al
0048ab5c  jne        0x48ab57

// block 0048ab5e
0048ab5e  mov        byte ptr [edi - 2], bl

// block 0048ab61
0048ab61  push       dword ptr [ebp + 0x1c]
0048ab64  lea        eax, [ebp - 0x30]
0048ab67  push       1
0048ab69  push       dword ptr [ebp + 0x14]
0048ab6c  mov        ecx, esi
0048ab6e  push       dword ptr [ebp + 0x10]
0048ab71  call       0x48a8dd

// block 0048ab76
0048ab76  add        esp, 0x10
0048ab79  jmp        0x48ab97

// block 0048ab7b
0048ab7b  push       dword ptr [ebp + 0x1c]
0048ab7e  lea        eax, [ebp - 0x30]
0048ab81  push       1
0048ab83  push       eax
0048ab84  push       dword ptr [ebp + 0x18]
0048ab87  mov        eax, esi
0048ab89  push       dword ptr [ebp + 0x14]
0048ab8c  push       dword ptr [ebp + 0x10]
0048ab8f  call       0x48a2eb

// block 0048ab94
0048ab94  add        esp, 0x18

// block 0048ab97
0048ab97  mov        ecx, dword ptr [ebp - 4]
0048ab9a  pop        edi
0048ab9b  pop        esi
0048ab9c  xor        ecx, ebp
0048ab9e  pop        ebx
0048ab9f  call       0x46c932

// block 0048aba4
0048aba4  leave      
0048aba5  ret        
