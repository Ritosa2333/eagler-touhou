
// block 00479f3a
00479f3a  mov        edi, edi
00479f3c  push       ebp
00479f3d  mov        ebp, esp
00479f3f  sub        esp, 0x18
00479f42  push       ebx
00479f43  push       dword ptr [ebp + 0xc]
00479f46  lea        ecx, [ebp - 0x18]
00479f49  call       0x46d3b4

// block 00479f4e
00479f4e  mov        ebx, dword ptr [ebp + 8]
00479f51  cmp        ebx, 0xff
00479f57  jbe        0x479fba

// block 00479f59
00479f59  mov        eax, ebx
00479f5b  shr        eax, 8
00479f5e  movzx      ecx, al
00479f61  mov        byte ptr [ebp - 4], al
00479f64  mov        eax, dword ptr [ebp - 0x14]
00479f67  mov        byte ptr [ebp - 3], bl
00479f6a  test       byte ptr [ecx + eax + 0x1d], 4
00479f6f  jne        0x479f82

// block 00479f71
00479f71  cmp        byte ptr [ebp - 0xc], 0
00479f75  je         0x479f7e

// block 00479f77
00479f77  mov        eax, dword ptr [ebp - 0x10]
00479f7a  and        dword ptr [eax + 0x70], 0xfffffffd

// block 00479f7e
00479f7e  mov        eax, ebx
00479f80  jmp        0x479fdd

// block 00479f82
00479f82  push       1
00479f84  push       dword ptr [eax + 4]
00479f87  lea        ecx, [ebp - 8]
00479f8a  push       2
00479f8c  push       ecx
00479f8d  push       2
00479f8f  lea        ecx, [ebp - 4]
00479f92  push       ecx
00479f93  push       0x200
00479f98  push       dword ptr [eax + 0xc]
00479f9b  lea        eax, [ebp - 0x18]
00479f9e  push       eax
00479f9f  call       0x48364d

// block 00479fa4
00479fa4  add        esp, 0x24
00479fa7  test       eax, eax
00479fa9  je         0x479f71

// block 00479fab
00479fab  movzx      eax, byte ptr [ebp - 8]
00479faf  movzx      ecx, byte ptr [ebp - 7]
00479fb3  shl        eax, 8
00479fb6  add        eax, ecx
00479fb8  jmp        0x479fd0

// block 00479fba
00479fba  mov        eax, dword ptr [ebp - 0x14]
00479fbd  add        eax, ebx
00479fbf  test       byte ptr [eax + 0x1d], 0x20
00479fc3  je         0x479fce

// block 00479fc5
00479fc5  movzx      eax, byte ptr [eax + 0x11d]
00479fcc  jmp        0x479fd0

// block 00479fce
00479fce  mov        eax, ebx

// block 00479fd0
00479fd0  cmp        byte ptr [ebp - 0xc], 0
00479fd4  je         0x479fdd

// block 00479fd6
00479fd6  mov        ecx, dword ptr [ebp - 0x10]
00479fd9  and        dword ptr [ecx + 0x70], 0xfffffffd

// block 00479fdd
00479fdd  pop        ebx
00479fde  leave      
00479fdf  ret        
