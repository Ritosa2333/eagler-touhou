
// block 0046c932
0046c932  cmp        ecx, dword ptr [0x4ad138]
0046c938  jne        0x46c93c

// block 0046c93a
0046c93a  repz ret   

// block 0046c93c
0046c93c  jmp        0x46e827

// block 0046e827
0046e827  mov        edi, edi
0046e829  push       ebp
0046e82a  mov        ebp, esp
0046e82c  sub        esp, 0x328
0046e832  mov        dword ptr [0x4b39e8], eax
0046e837  mov        dword ptr [0x4b39e4], ecx
0046e83d  mov        dword ptr [0x4b39e0], edx
0046e843  mov        dword ptr [0x4b39dc], ebx
0046e849  mov        dword ptr [0x4b39d8], esi
0046e84f  mov        dword ptr [0x4b39d4], edi
0046e855  mov        word ptr [0x4b3a00], ss
0046e85c  mov        word ptr [0x4b39f4], cs
0046e863  mov        word ptr [0x4b39d0], ds
0046e86a  mov        word ptr [0x4b39cc], es
0046e871  mov        word ptr [0x4b39c8], fs
0046e878  mov        word ptr [0x4b39c4], gs
0046e87f  pushfd     
0046e880  pop        dword ptr [0x4b39f8]
0046e886  mov        eax, dword ptr [ebp]
0046e889  mov        dword ptr [0x4b39ec], eax
0046e88e  mov        eax, dword ptr [ebp + 4]
0046e891  mov        dword ptr [0x4b39f0], eax
0046e896  lea        eax, [ebp + 8]
0046e899  mov        dword ptr [0x4b39fc], eax
0046e89e  mov        eax, dword ptr [ebp - 0x320]
0046e8a4  mov        dword ptr [0x4b3938], 0x10001
0046e8ae  mov        eax, dword ptr [0x4b39f0]
0046e8b3  mov        dword ptr [0x4b38ec], eax
0046e8b8  mov        dword ptr [0x4b38e0], 0xc0000409
0046e8c2  mov        dword ptr [0x4b38e4], 1
0046e8cc  mov        eax, dword ptr [0x4ad138]
0046e8d1  mov        dword ptr [ebp - 0x328], eax
0046e8d7  mov        eax, dword ptr [0x4ad13c]
0046e8dc  mov        dword ptr [ebp - 0x324], eax
0046e8e2  call       dword ptr [0x4981c4]

// block 0046e8e8
0046e8e8  mov        dword ptr [0x4b3930], eax
0046e8ed  push       1
0046e8ef  call       0x47b3ff

// block 0046e8f4
0046e8f4  pop        ecx
0046e8f5  push       0
0046e8f7  call       dword ptr [0x4981c8]

// block 0046e8fd
0046e8fd  push       0x49cd68
0046e902  call       dword ptr [0x4981cc]

// block 0046e908
0046e908  cmp        dword ptr [0x4b3930], 0
0046e90f  jne        0x46e919

// block 0046e911
0046e911  push       1
0046e913  call       0x47b3ff

// block 0046e918
0046e918  pop        ecx

// block 0046e919
0046e919  push       0xc0000409
0046e91e  call       dword ptr [0x4981fc]

// block 0046e924
0046e924  push       eax
0046e925  call       dword ptr [0x4981f8]

// block 0046e92b
0046e92b  leave      
0046e92c  ret        
