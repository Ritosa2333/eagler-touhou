
// block 00475307
00475307  push       0xc
00475309  push       0x4aad28
0047530e  call       0x46fcec

// block 00475313
00475313  mov        esi, 0x49d51c
00475318  push       esi
00475319  call       dword ptr [0x498174]

// block 0047531f
0047531f  test       eax, eax
00475321  jne        0x47532a

// block 00475323
00475323  push       esi
00475324  call       0x472bdd

// block 00475329
00475329  pop        ecx

// block 0047532a
0047532a  mov        dword ptr [ebp - 0x1c], eax
0047532d  mov        esi, dword ptr [ebp + 8]
00475330  mov        dword ptr [esi + 0x5c], 0x49d5c8
00475337  xor        edi, edi
00475339  inc        edi
0047533a  mov        dword ptr [esi + 0x14], edi
0047533d  test       eax, eax
0047533f  je         0x475365

// block 00475341
00475341  push       0x49d50c
00475346  push       eax
00475347  mov        ebx, dword ptr [0x498170]
0047534d  call       ebx

// block 0047534f
0047534f  mov        dword ptr [esi + 0x1f8], eax
00475355  push       0x49d538
0047535a  push       dword ptr [ebp - 0x1c]
0047535d  call       ebx

// block 0047535f
0047535f  mov        dword ptr [esi + 0x1fc], eax

// block 00475365
00475365  mov        dword ptr [esi + 0x70], edi
00475368  mov        byte ptr [esi + 0xc8], 0x43
0047536f  mov        byte ptr [esi + 0x14b], 0x43
00475376  mov        dword ptr [esi + 0x68], 0x4ad4b0
0047537d  push       0xd
0047537f  call       0x46ec9a

// block 00475384
00475384  pop        ecx
00475385  and        dword ptr [ebp - 4], 0
00475389  push       dword ptr [esi + 0x68]
0047538c  call       dword ptr [0x498160]

// block 00475392
00475392  mov        dword ptr [ebp - 4], 0xfffffffe
00475399  call       0x4753dc

// block 0047539e
0047539e  push       0xc
004753a0  call       0x46ec9a

// block 004753a5
004753a5  pop        ecx
004753a6  mov        dword ptr [ebp - 4], edi
004753a9  mov        eax, dword ptr [ebp + 0xc]
004753ac  mov        dword ptr [esi + 0x6c], eax
004753af  test       eax, eax
004753b1  jne        0x4753bb

// block 004753b3
004753b3  mov        eax, dword ptr [0x4adab8]
004753b8  mov        dword ptr [esi + 0x6c], eax

// block 004753bb
004753bb  push       dword ptr [esi + 0x6c]
004753be  call       0x473ff5

// block 004753c3
004753c3  pop        ecx
004753c4  mov        dword ptr [ebp - 4], 0xfffffffe
004753cb  call       0x4753e5

// block 004753d0
004753d0  call       0x46fd31

// block 004753d5
004753d5  ret        
