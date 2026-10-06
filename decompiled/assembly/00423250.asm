
// block 00423250
00423250  push       ebx
00423251  push       0x3c
00423253  xor        ebx, ebx
00423255  push       ebx
00423256  push       esi
00423257  call       0x477420

// block 0042325c
0042325c  or         dword ptr [esi + 0x38], 0x100
00423263  mov        eax, 0x258
00423268  mov        word ptr [esi + 0x16], ax
0042326c  mov        ecx, eax
0042326e  mov        byte ptr [esi + 0x1a], bl
00423271  mov        byte ptr [esi + 0x1d], bl
00423274  mov        byte ptr [esi + 0x1e], bl
00423277  mov        dword ptr [esi], 0x120001
0042327d  mov        word ptr [esi + 0x18], cx
00423281  mov        al, 1
00423283  mov        byte ptr [esi + 0x1b], al
00423286  mov        byte ptr [esi + 0x1c], al
00423289  mov        edx, dword ptr [0x4d49ec]
0042328f  mov        dword ptr [esi + 4], edx
00423292  mov        eax, dword ptr [0x4d49f0]
00423297  mov        dword ptr [esi + 8], eax
0042329a  mov        ecx, dword ptr [0x4d49f4]
004232a0  mov        dword ptr [esi + 0xc], ecx
004232a3  mov        edx, dword ptr [0x4d49f8]
004232a9  mov        dword ptr [esi + 0x10], edx
004232ac  mov        ax, word ptr [0x4d49fc]
004232b2  mov        word ptr [esi + 0x14], ax
004232b6  mov        al, 2
004232b8  mov        byte ptr [esi + 0x1f], al
004232bb  mov        byte ptr [esi + 0x23], al
004232be  mov        eax, 0x80000000
004232c3  add        esp, 0xc
004232c6  mov        byte ptr [esi + 0x22], bl
004232c9  mov        byte ptr [esi + 0x20], 0x64
004232cd  mov        byte ptr [esi + 0x21], 0x50
004232d1  mov        dword ptr [esi + 0x28], eax
004232d4  mov        dword ptr [esi + 0x2c], eax
004232d7  pop        ebx
004232d8  ret        
