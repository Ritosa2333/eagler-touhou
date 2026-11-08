# TH13 渲染提交链逆向 —— 假设备语义捕获施工图

来源：th13 v1.00c Ghidra 导出（`artifacts/decomp/jp/functions/*.c`，本文件所有 `file:line` 均指该目录）+ wine d3d9.h vtable/D3DRS 枚举权威核对（见 §0）。所有设备调用以**游戏侧函数地址**为锚（unicorn 直调/钩挂定位），不以 IAT/d3d9.dll 内部地址为锚。ANM 数据结构细节与 `anm-pipeline.md` 互引不重复。

**已验证 / 未验证边界**：本文全部结论来自静态反编译读证 + 与 anm-pipeline.md（字节级 dump 实证）交叉核对；**尚未**在 unicorn 上跑过挂钩实测（§8 列出待验证项）。Ghidra 对 0x467xxx–0x469xxx 绘制族的 `unaff_ESI/EDI`、`in_EAX` 隐参标注按§7 约定处理，需 unicorn 确认。

## 0. vtable 偏移→方法名对照（实证锚定）

游戏不封装设备，**直接裸调** `IDirect3DDevice9`（对象存全局 `DAT_004dc6a8`）。wine d3d9.h 方法序（1 基）减 1 得 0 基索引，以下偏移已被参数形状一一实证：

| 偏移 | 索引 | 方法 | 实证（参数形状吻合） |
|---|---|---|---|
| +0x0c | 3 | TestCooperativeLevel | 返回 0x88760869=DEVICELOST 比对 `0045c110.c:267` |
| +0x1c | 7 | GetDeviceCaps | `0045dc80.c:248` |
| +0x40 | 16 | Reset | 参数=&DAT_004dc794（14 dword present params）`0045c110.c:352` |
| +0x44 | 17 | Present | 4×NULL `0045d4b0.c:12` |
| +0x48 | 18 | GetBackBuffer | `(0,0,0,&surf)` `0045bf10.c:31` |
| +0x4c | 19 | GetRasterStatus | vsync 轮询 `0045d060.c:69-71` |
| +0x5c | 23 | CreateTexture | `(w,h,1,usage,fmt,pool,&out,0)` `0045be50.c:29` |
| +0x68 | 26 | CreateVertexBuffer | `(0x2d0,0,0x102,1,&vb,0)` `0046c950.c:57` |
| +0x94 | 37 | SetRenderTarget | ×4 处 |
| +0xa4 | 41 | BeginScene | `0045d2f0.c:43` |
| +0xa8 | 42 | EndScene | `0045d2f0.c:50` |
| +0xac | 43 | Clear | `(0,0,3,0xff000000,1.0,0)` `0045ee40.c:5` |
| +0xb0 | 44 | SetTransform | `(2/3/256/16, matrix)` 见§5 |
| +0xbc | 47 | SetViewport | `(game+0xd8)` `0045d2f0.c:30` |
| +0xc0 | 48 | GetViewport | `0045dc80.c:247` |
| +0xe4 | 57 | SetRenderState | 79 处，参数皆 D3DRS 枚举 |
| +0x104 | 65 | SetTexture | `(0, tex)` 9 处 |
| +0x10c | 67 | SetTextureStageState | `(stage,type,val)` 86 处 |
| +0x114 | 69 | SetSamplerState | `(0,1/2/5/6,val)` 11 处 |
| +0x144 | 81 | DrawPrimitive | `(5, start, 2)` `004692d0.c:245` |
| +0x14c | 83 | DrawPrimitiveUP | `(type,count,ptr,stride)` 14 处 |
| +0x164 | 89 | SetFVF | 0x102/0x144/0x44 |
| 400(0x190) | 100 | SetStreamSource | `(0,vb,0,0x14)` `0046c950.c:163` |

IDirect3D9（`DAT_004dc6a4`）：+0x20=GetAdapterDisplayMode、+0x28=CheckDeviceFormat、+0x40=CreateDevice（`0045dc80.c:65,142-179,259`）。
IDirect3DTexture9：+0x2c=LockRect、+0x30=UnlockRect、+0x44=GetLevelDesc(17)、+0x48=GetSurfaceLevel(18)、+0x54=AddDirtyRect(21)；IDirect3DVertexBuffer9：+0x2c=Lock、+0x30=Unlock（`0046c950.c:58,162`）。

## 1. 设备创建与帧边界

### 1.1 创建（一次性）
- `WinMain FUN_0045c110`（`0045c110.c`）：`Direct3DCreate9(0x20)` → `DAT_004dc6a4`（line 220）→ `FUN_0045da40` 建窗口（hwnd 存 `DAT_004dd0a8`）→ `FUN_0045dc80`：
  - CreateDevice（IDirect3D9+0x40，三档回退：HAL+硬件 VP(0x40)→HAL+软件 VP(0x20)→REF，`0045dc80.c:134-201`），设备对象落 `DAT_004dc6a8`；
  - 初始 present params 拷贝到 `DAT_004dc794`（line 209-215，Reset 复用）；
  - D3DXMatrixLookAtLH/PerspectiveFovLH → SetTransform(VIEW=2, PROJ=3)（line 228-245）；
  - SetViewport(`0045dc80.c:247`)、GetDeviceCaps（caps 落 `DAT_004dcc68`）、`FUN_0045e1e0`（全量初始状态，§5.1）。
- 管理器：`FUN_0046bc60`（0xf53c48 字节）→ `DAT_004dc688`（`0045c110.c:234-239`）。其 +0xb77b34 起 26 槽 ANM file 对象表；初始化时 `FUN_0043a040`→`FUN_0046c950` 建静态四边形 VB（§3.1）。

### 1.2 每帧驱动（WinMain 主循环 `0x45c57d` 按窗口/全屏/vsync 三选一）
| 驱动 | Present 尾 | 特征 |
|---|---|---|
| `FUN_0045d2f0` | `FUN_0045d4b0` | 常规窗`0045c110.c:277` |
| `FUN_0045d570` | `FUN_0045d690` | 全屏变体 |
| `FUN_0045cef0` | `FUN_0045d060` | vsync：GetRasterStatus 轮询（`0045d060.c:69-71`） |

三者帧内结构相同（以 `FUN_0045d2f0.c:21-53` 为准）：

```
追帧循环（1/60 步进）{
  FUN_004679a0()                    // 清残留 UP 批
  FUN_0043b900()                    // 每 tick：刷视/投矩阵 → SetTransform(2/3)（0043b900.c:38-41）
  SetViewport(DAT_004dc9d0 + 0xd8)  // 帧视口（0045d2f0.c:30）
  FUN_00470af0()                    // 逻辑步进（状态机虚调用）
  [跳帧计数到] {
    BeginScene
    FUN_00467960()                  // 清空本帧 UP 队列/特效 scratch（00467960.c:10-16）
    FUN_0043b080()                  // 视状态刷 SetRenderState(FOGENABLE,0)
    FUN_00470c30()                  // ★绘制分发：状态机 draw 虚调用（00470c30.c:37）
    FUN_004679a0()                  // ★最终 UP 批提交（DrawPrimitiveUP）
    SetTexture(0, NULL)             // 0045d2f0.c:49
    EndScene
    FUN_0045d4b0 → FUN_0045d7d0 → Present(NULL×4)   // 0045d4b0.c:11-12
  }
}
```

`FUN_00470c30` 遍历状态对象调 `state+8` 虚方法（draw）；各状态 draw（如游戏内 `FUN_004032d0`）循环调 `FUN_0046a700` 逐精灵提交（`004032d0.c:76-83`）。

### 1.3 Reset/设备丢失恢复（Present 或 TCL 失败）
`0045c110.c:319-415` 与 `0045d4b0.c:13-33`（同构）：`FUN_0045bdd0`（释放纹理链）→ `Reset(&DAT_004dc794)` → `FUN_0045be50`（**按 manager+0xb77b34 的 26 槽×entry 重建纹理**，`0045be50.c:15-39`）→ `FUN_0043cd40` → `FUN_0045e1e0`（重放初始状态）。**纹理指针在 Reset 后全部更换**——捕获注册表必须随之重建。

## 2. 每帧绘制提交路径

`FUN_0046a700`（入口 0x46a700）= **逐精灵提交分发器**（render type = sprite flags+0x18 >> 0x19 & 0x1f，`0046a700.c:29`）。前置：sprite+0x594 预绘回调（line 16-17）、可见性（flags+0x18 bit0/bit1、+0x52b/+0x52f）。之后两条主路径：

### 2.1 矩阵路径（type 8/0x12，主 ANM 精灵）→ `FUN_004692d0`
一次调用 = 一个四边形 = 一次 DrawPrimitive。时序（`004692d0.c`）：
1. 可见/延迟处理；ZWRITEENABLE 切换（`FUN_0043b100/0c0`，缓存 gamestate+0x894）；
2. flags+0x18 满足 `(bit2|bit3)!=0 且 bit16==0` 时重算矩阵：抄 +0x3e0 基矩阵→+0x420，乘入缩放/旋转（D3DXMatrixRotation{X,Y,Z}×Multiply，序由 (flags+0x1c)>>2&7 决定，算完清 bit3；`004692d0.c:43-140`）；
3. 最终平移 = pos(+0x3c..44) + offset(+0x574/578/57c) + 锚点补偿 + 父链(+0x570)（line 141-169）——写入世界矩阵第 4 行；
4. `FUN_004670e0`：混合/采样状态分发（§5.2）；
5. TFACTOR：color(+0x528/0x52c 按 flags bit16) × manager 渐变(+0xf53c40..43) → SetRenderState(TEXTUREFACTOR=60, argb)（line 171-205，缓存 +0xb78184）；
6. **SetTransform(D3DTS_WORLD=0x100, world)**（line 208）；
7. **SetTexture(0, *(*(sprite+0x2c)+8))**，缓存 +0xb78188（line 209-213）；
8. 纹理矩阵（UV 滚动/缩放，+0x460 基×scale(+0x78/7c)+offset(+0x88/8c/3b8/3bc)）→ **SetTransform(D3DTS_TEXTURE0=0x10)**（line 214-232，缓存 +0xb78198）；
9. 懒置：SetStreamSource(0, 静态VB, 0, 0x14) + SetFVF(0x102=XYZ|TEX1) + ALPHAARG2/COLORARG2=TFACTOR（line 233-239，缓存 +0xb7818e）；
10. **DrawPrimitive(TRIANGLESTRIP=5, startVertex=((flags>>0x17&3)*3+(flags>>0x15&3))*4, 2)**（line 245-247）——从 9 组预建四边形选一。

### 2.2 预变换顶点路径（type 0-7）→ `FUN_00467350` 族
`FUN_0046a700` case 0-7 → `FUN_00467d80/00468130/00467dc0/00468170/00468640/00469050/00468670/00469080`（各为 2D/旋转/翻转等变体）→ 核心 `FUN_00467350`（0x467350，thiscall：ECX=manager，stack=flags，**EAX=sprite**，`00467350.c`）：
1. 全局角点 scratch `0x4e47d8..0x4834`（4×28B XYZRHW 顶点，初值来自 `FUN_0046c950` 写的 `0x4e4848+` 静态角点）平移 +0xd0/+0xd4，可选半像素对齐 ROUND−0.5（line 37-54）；
2. UV = sprite UV 基(+0x3b8..0x3d4)×scale(+0x78/7c)+scroll(+0x88/8c)（line 55-82）；顶点色 = color(+0x528/0x52c 按极性 flags>>0x11&3)×manager 渐变（line 167-218）；
3. 视口裁剪（对 `DAT_004dcc18+0xd8..0xe4`，即本帧 SetViewport 的同一块，line 125-156）；
4. 纹理变更→SetTexture+刷批（line 157-162）；`FUN_004670e0`（状态）；**`FUN_00467a80` 入队 6 顶点**（line 220-221）。

`FUN_00467a80`（0x467a80）：把 scratch 的 4 顶点展开为 6 顶点（两三角形）**memcpy 到 manager+0xef81f4 游标处**（每精灵 0xa8=6×0x1c 字节），count++（+0xb781f0）（`00467a80.c:12-54`）。

**批提交 `FUN_004679a0`**（0x4679a0，ESI=manager）：count≠0 时 → SetTextureStageState 懒置 + SetFVF(0x144=XYZRHW|DIFFUSE|TEX1) + **DrawPrimitiveUP(TRIANGLELIST=4, count*2, manager+0xef81f8, stride=0x1c)**（`004679a0.c:9-18`），然后清 count。任何状态/纹理变化前都会先调它刷批（隐式批边界）。

### 2.3 特效/杂项 UP 站点（不经精灵队列，直接 DrawPrimitiveUP）
| 函数 | FVF | 图元 | 顶点源 | 用途 |
|---|---|---|---|---|
| `FUN_0046a5f0`/`FUN_0046a150`（type 9/0xb/0xc-e） | 0x144 | FAN(6)/STRIP(5) | sprite+0x58c 自定义顶点数组（+0x4ac 顶点数） | 自定义顶点精灵 |
| `FUN_0046ad00`/`FUN_0046afa0`（type 0xf/0x13） | 0x44 | STRIP(5) | manager+0xf48200（stride 0x14） | 线状特效（激光等） |
| `FUN_0046b050`/`FUN_0046b3c0`（type 0x10/0x11） | 0x44 | STRIP/LIST | 同上 | 粒子带 |
| `FUN_0046a310`/`FUN_0046a480`/`FUN_00469b10` | 0x44 | STRIP/FAN | 同上/自定义 | 环形/链特效 |
| `FUN_004032d0`（游戏内主 draw） | 0x44 | STRIP | 栈上 4 顶点 | 关卡状态叠加层（`004032d0.c:62-67`） |
| `FUN_0045efc0`/`FUN_0045f1b0` | 0x44 | STRIP(5,2) | 栈上 | 全屏暗幕/渐变四边形 |

（0x44 = XYZRHW|DIFFUSE，无 UV。）

### 2.4 文本路径（动态纹理，非顶点）
ANM VM 软件文本指令 → `FUN_0046e810` → `FUN_0045aef0`：GDI TextOutA（4 向描边+填充，7 套字体 DAT_004dc2f4..310）画入内存 DC 位图 `DAT_004be9c0` → D3DXLoadSurfaceFromMemory 上传到当前 entry 纹理（`0045aef0.c:25-71`）。文本一旦上传就是普通 ANM 纹理，走§2.1/2.2 绘制——**捕获层无需专门处理**，只需纹理内容比对时知道该纹理是动态的。

## 3. 顶点填充路径与捕获层选择

### 3.1 静态 VB（矩阵路径几何）
`FUN_0046c950`（0x46c950）：CreateVertexBuffer(720B, FVF 0x102, MANAGED) → Lock(+0x2c) → 写 9 组四边形（每组 4×20B：x,y,z,u,v；256×256 中心化 + ±128/±256 锚点九宫格，UV 0..1）→ Unlock → SetStreamSource(0,VB,0,0x14)（`0046c950.c:57-163`）。**仅初始化/Reset 后调用一次**。同时把同值角点写全局 `0x4e4848..0x48a4` 供 UP 路径复用（`0046c950.c:29-56`）。

### 3.2 捕获层结论：**不钩 Lock/memcpy**
- 矩阵路径顶点是**静态常量**，语义全部在 SetTransform 三矩阵 + TFACTOR + 纹理 + startVertex（锚点组号）——钩 `FUN_004692d0` 一次拿全；
- UP 路径顶点在 `FUN_004679a0` 被调时已是最终值（该函数入口直接 memcpy 队列给 DrawPrimitiveUP）——钩它读 `manager+0xef81f8` 起 count×0xa8 字节即可；
- 特效路径（§2.3）顶点在 `manager+0xf48200`（或 sprite+0x58c），同样在各自 DrawPrimitiveUP 前定型。
- 结论：**最稳捕获层 = 各 Draw* 调用的"游戏内层语义函数"入口**（§7 表），Lock/memcpy 层（仅 `0046c950` 一处）无需挂钩。

## 4. 纹理创建路径与 纹理↔ANM 映射

### 4.1 装载链
```
FUN_004708f0（文件名→槽 id） → FUN_0046deb0（file 对象 malloc 0x13c，+0x0=槽 id，挂 manager+0xb77b34+槽*4）
  → FUN_0046dcf0（读档入 file+0x108；链式 entry 计数(+0x24 nextoffset) → +0x110 数；
     分配 entries 0x18×N → +0x124；sprites 0x48×N → +0x11c；scripts 4×N → +0x120）
  每帧增量：FUN_0046e0b0（file+0x128 游标，每帧 1-2 entry）→ 每 entry FUN_0046e230：
    entry 头 param_5（布局见 anm-pipeline.md §2：+0xa w/+0xc h/+0xe fmt/+0x1C THTX 偏移/+0x20 hasData）
    三选一建纹理（写 entry 元素+0x0 = IDirect3DTexture9*）：
      a) hasData=1 内部位图 → FUN_0046dae0：D3DXCreateTexture(dev,w,h,1,0,fmt表[fmt],MANAGED)
         + GetSurfaceLevel(0) + D3DXLoadSurfaceFromMemory(entry 数据+0x10, fmt表, pitch=w*bpp表)
         （格式表 DAT_004acbc4、bpp 表 DAT_004acbe8；0046dae0.c:31-42）
      b) hasData=0 整图文件 → FUN_0046d8f0：D3DXCreateTextureFromFileInMemoryEx(dev, data, len, …)
         尺寸不符时 D3DXCreateTexture + D3DXLoadSurfaceFromSurface 子拷（0046d8f0.c:34-60）
      c) 名 '@R' 开头渲染目标 → FUN_0046dc60：CreateTexture(w,h,1,RENDERTARGET,A8R8G8B8,DEFAULT)
    GetLevelDesc(0) 回填真实 w/h/format（0046e230.c:32-33）
    sprite 表展开 → FUN_0046e770（0x48 记录：UV 分数、+0x1c/20 texH/W、+0x34/38 像素 w/h、
      +0x3c/40 UV 缩放；**+0x8 = IDirect3DTexture9*（供 SetTexture 用，见 004692d0.c:209）**）
    script 偏移入全局表 +0x120（anm-pipeline.md:51 已证）
```
Reset 重建：`FUN_0045be50` 遍历 26 槽×各 entry，**仅对 entry+0x14 flag bit0 置位者**（即 `@R` 渲染目标类，DEFAULT 池不渡 Reset）重建 CreateTexture(w,h,1,Usage=RENDERTARGET,A8R8G8B8,DEFAULT)；MANAGED 纹理渡 Reset 不重建（`0045be50.c:15-39`）。

### 4.2 纹理指针 → (归档, entry, sprite) 反查（捕获侧映射表）
给定 `IDirect3DTexture9* T`（SetTexture/绘制处拿到的就是它）：
1. 线性扫 `manager+0xb77b34` 26 槽 → file 对象 F（+0x0 = 槽 id）；
2. 扫 `F+0x124` entries（F+0x110 个，步 0x18）：`entry_elem+0x0 == T` 命中 → entry 序号 i、entry 头在 `entry_elem+0x10`（w/h/fmt/THTX 偏移/名字 name1 相对偏移 +0x10 处的字段——归档内纹理名如 `front/front00.png`，cp932）；
3. sprite 名反查：用 sprite 记录（sprite+0x2c 指向的 0x48 记录）在 `F+0x11c` 表中的下标 = 全局 sprite 序号；sprite→name 需经 ANM dump 侧（anm-list.txt 已有全量 2480 sprite 与 script 名）。VM 侧 sprite+0x34 = 当前 sprite id（op300 写入，`00461a60.c:18`）——**建议捕获直接记 (槽 id, sprite id) 二元组**，名字映射离线做。
4. 特殊纹理：`@`（空白）/`@R`（渲染目标，text.anm ×2，anm-pipeline.md:24）；`FUN_0045bf10`/`FUN_0046eaf0` 会把**后台缓冲**拷进某些 entry 纹理（GetBackBuffer+D3DXLoadSurfaceFromSurface+AddDirtyRect，`0045bf10.c:31-48`）——这些纹理内容逐帧变，语义比对时按"动态纹理"标记。

## 5. 状态设置

### 5.1 初始全量状态（`FUN_0045e1e0`，设备创建/Reset 后）
SetRenderState（`0045e1e0.c:8-28`）：ZENABLE=1、LIGHTING=0、CULLMODE=1(NONE)、ALPHABLENDENABLE=1、SEPARATEALPHABLENDENABLE(206)=0、FILLMODE=2(SOLID)、SRCBLEND=5(SRCALPHA)、DESTBLEND=6(INVSRCALPHA)、BLENDOP(171)=1(ADD)、ALPHATESTENABLE=1、ALPHAREF=1、ALPHAFUNC=7(GREATEREQUAL)、FOGENABLE=1、FOGDENSITY=1.0、TEXTUREFACTOR?、FOGCOLOR=0xffa0a0a0、FOGSTART=1000、FOGEND=5000、(161)=0。
SetTextureStageState（line 29-36）：COLORARG2/ALPHAARG2=TFACTOR(3)、ALPHAOP/COLOROP=MODULATE(4)、(0x18/0xb…)。
SetSamplerState（line 37-42）：MAGFILTER/MINFILTER=2(LINEAR)、MIPFILTER=3?、ADDRESSU/V=3(CLAMP)/1(WRAP)。

### 5.2 每精灵状态分发 `FUN_004670e0`（0x4670e0，EAX=manager，EDI=sprite）
带设备状态影子缓存（manager+0xb7818c/92/94/95），只在变化时刷并先刷 UP 批：
- **混合模式**（sprite flags+0x18 >> 5 & 0xf，8 种，`004670e0.c:16-56` 精确值）：

| 模式 | SRCBLEND(19) | DESTBLEND(20) | BLENDOP(171) | 语义 |
|---|---|---|---|---|
| 0 | 5 SRCALPHA | 6 INVSRCALPHA | 1 ADD | 标准半透明 |
| 1 | 5 SRCALPHA | 2 ONE | 1 ADD | 加法 |
| 2 | 5 SRCALPHA | 2 ONE | 3 REVSUBTRACT | 减色 |
| 3 | 2 ONE | 1 ZERO | 1 ADD | 不透明覆盖 |
| 4 | 10 INVDESTCOLOR | 6 INVSRCALPHA | 1 ADD | 乘性暗化 |
| 5 | 9 DESTCOLOR | 1 ZERO | 1 ADD | 乘 |
| 6 | 4 INVSRCCOLOR | 6 INVSRCALPHA | 1 ADD | 反乘 |
| 7 | 7 DESTALPHA | 8 INVDESTALPHA | 1 ADD | 目标 alpha |

- 采样：MAG/MINFILTER 线性/点（flags2+0x1c bit10，`(0,5,·)(0,6,·)`）；ADDRESSU=flags2&3（0→1 WRAP，1→3 CLAMP，2→2 MIRROR）；ADDRESSV=flags+0x18>>30（同值域）（line 59-105）。
- 附加：`FUN_0043b100/0c0` ZWRITEENABLE（缓存 gamestate+0x894）；`FUN_0043b040/080` FOGENABLE（+0x890）；`FUN_004032d0.c:34-41` 阶段特效叠层用 SRCBLEND=1(ZERO)/DEST=2(ONE)+SEPARATEALPHA(206)=1+SRCBLENDALPHA(207)=2/DESTBLENDALPHA(208)=1/BLENDOPALPHA(209)=1。

### 5.3 矩阵/视口
- VIEW/PROJECTION：每 tick `FUN_0043b900`（D3DX LookAtLH(320,240 视高缩放)/PerspectiveFovLH → SetTransform(2/3)，矩阵同时存 gamestate+0x58/0x98，`0043b900.c:29-41`）；初建同构（`0045dc80.c:216-245`）。
- WORLD(0x100)/TEXTURE0(0x10)：每精灵（§2.1 第 6/8 步）。
- 视口：每帧 SetViewport(`DAT_004dc9d0+0xd8`，即 `DAT_004dcc18+0xd8`)——同一块内存也是 UP 路径裁剪框（`00467350.c:125-156`）。

## 6. 游戏侧状态影子（免钩 79 处 SetRenderState 的关键）
manager(`DAT_004dc688`) 常备当前设备语义，**Present 边界一次快照即得全帧终态**：
| 偏移 | 内容 | 写点 |
|---|---|---|
| +0xb78184 | TFACTOR 当前 ARGB | `004692d0.c:204` |
| +0xb78188 | 当前纹理指针 | `004692d0.c:211`/`00467350.c:159` |
| +0xb7818c | 混合模式 0-8 | `004670e0.c:15`（8=外部直设后写，`004032d0.c:74`） |
| +0xb7818e | stage 状态标记(1/2) | `004692d0.c:238` |
| +0xb78192/94/95 | 采样滤波/寻址模式 | `004670e0.c:62/76/93` |
| +0xb78198 | 纹理矩阵脏标记比较基 | `004692d0.c:217` |
| +0xb78188 队列 | UP 批顶点游标 +0xef81f4 / 头 +0xef81f8 / 计数 +0xb781f0 | `00467a80.c:53-54` |
| +0xf48200 | 特效顶点 scratch | `00467960.c:15-16` |
| +0xf53c40..43 | 全局 RGBA 渐变系数 | `00467350.c:180-203` |

## 7. 挂钩点总表（建议捕获方式）

约定说明：MSVC x86；`__fastcall`=ECX,EDX,栈；`__thiscall`=ECX,栈；**标注 EAX/ESI/EDI 者为 Ghidra 隐参（ZUN 自定义寄存器约定），需 unicorn 入口实测确认**。捕获一律"入口读参数 + 返回后读副作用"，用 oracle/machine.py 的 override_call/钩子机制，不动游戏代码。

| # | 环节 | 地址 | 约定/参数 | 捕获方式 |
|---|---|---|---|---|
| H1 | 帧边界（统一） | `0x45d7d0` | fastcall(param_1) | 三 Present 尾共有；入口=本帧全部绘制完成、Present 未发。入口处快照 §6 影子 + gamestate 视口。备选：钩三尾各自 Present 后 |
| H2 | 逐精灵语义（矩阵路径） | `0x4692d0` | fastcall(ECX=manager?, stack=sprite？——实为 (param_1=manager, param_2=sprite) 双参 fastcall，`004692d0.c:3`） | 入口记录 sprite 指针；在第 245 行 DrawPrimitive 前时刻（=函数尾部）读三矩阵（sprite+0x420 世界、sprite+0x460 系纹理矩阵）、TFACTOR、纹理指针、startVertex；或直接在入口读 sprite 全结构离线重放§2.1 计算 |
| H3 | UP 批提交 | `0x4679a0` | ESI=manager（隐参待证） | 入口读 count(+0xb781f0) 与顶点块(+0xef81f8, count×0xa8B)，即该次 DrawPrimitiveUP 全量 |
| H4 | 逐精灵语义（UP 路径） | `0x467350` | thiscall(ECX=manager, stack=flags, **EAX=sprite**) | 入口读 EAX=sprite（含 UV/色/纹理引用）；与 H3 互补：H3 给最终顶点，H4 给归属与语义字段 |
| H5 | 精灵可见性与渲染类型 | `0x46a700` | EAX=sprite（隐参待证），栈=manager | 入口读 flags(+0x18) 即知 render type 与可见性；作为 H2/H4 的统一前置分发点 |
| H6 | 混合/采样切换 | `0x4670e0` | EAX=manager, EDI=sprite（隐参待证） | 可不钩：读 §6 影子即可；若要时序，入口读 sprite flags 推导§5.2 表 |
| H7 | 纹理注册表（装载） | `0x46e230` | fastcall(脚本基址, file, entry 序号, ?, entry 头) | 返回后读 `file+0x124+序号*0x18` 的 +0x0=纹理指针、+0x10=entry 头（w/h/fmt/name1 偏移）→ 登记 T→(槽=*(file+0), entry 序号) |
| H8 | 纹理注册表（Reset 重建） | `0x45be50` | 无参 | 返回后全表重扫（配合 H7 重建映射；旧指针全部失效） |
| H9 | 设备创建 | `0x45dc80` 返回处 | — | 假设备层：伪造 CreateDevice/Direct3DCreate9 返回哨兵对象；记录 present params 块 DAT_004dc794 |
| H10 | 每 tick 视/投矩阵 | `0x43b900` | fastcall/EDI=gamestate（隐参待证） | 返回后读 gamestate+0x58(视)/+0x98(投) 16 float |
| H11 | 后台缓冲→纹理捕获 | `0x45bf10` | 无参 | 入口知 4 组 (槽, entry, 矩形)（manager+0..每组 10 dword）——动态纹理内容语义等价物 |
| H12 | 特效 UP（线/带/环） | `0x46ad00/0x46b050/0x46b3c0/0x46a310/0x46a480` | 各见§2.3 | DrawPrimitiveUP 前顶点在 manager+0xf48200 或 sprite+0x58c；按需补挂 |

**最小挂钩集**：H1（帧边界）+ H3（UP 批）+ H7（纹理注册，含 H8 Reset 重建）+ H2/H4（两路精灵语义，或先只挂 H5 统一分发 + H3 顶点）。理由：H3 的顶点 + H7 的纹理表已能重建"画了什么"；H2/H4 补"为什么这么画"（归属/矩阵/颜色语义）；§6 影子免去全部状态钩。

## 8. 风险与待 unicorn 验证项
1. **隐参约定**：0x467xxx-0x469xxx 族 EAX/ESI/EDI 隐参（H3/H4/H5/H6/H10）需入口断点实证寄存器来源；Ghidra 显示的 `unaff_*` 是线索非证明。
2. **sprite 记录 +0x8 = 纹理**的写入指令未定位（装载展开在 `FUN_0046e770` 的复制源里，源指针本身是隐参）；不影响使用（多函数已实证读取），但映射表若按记录遍历需 unicorn 确认该字段在 Reset 重建后同样刷新。
3. `FUN_0046a700` 的 4-5 次循环调用未显式传入不同 sprite（`004032d0.c:76-83`）——列表迭代细节（EAX 推进方）需实测。
4. 特效路径（§2.3）顶点 scratch +0xf48200 的逐调用覆盖时序：若同一帧多次特效提交互相覆盖，H12 需逐调用捕获而非帧末快照。
5. text.anm 的 `@R` 渲染目标纹理被 `FUN_0045bf10`/`0045eaf0` 用作拷贝目标，其"源"是后台缓冲——假设备侧需提供可读的假后台缓冲（假 Clear/假 draw 的合成面）才能让语义对照闭环；或按 H11 只记事件不比内容。
6. FVF 0x102 的顶点第 4/5 分量：0x14=20B=5 float/顶点，静态 VB 写入的布局是 x,y,z,u,v（`0046c950.c` 逐组四边形写入），未逐字段反汇编核对 D3D 语义（XYZ|TEX1 期望 x,y,z + 2D uv）——初值（±128/±256 与 0/1）强烈支持，但列待证。
7. 阶段状态 `(0,0x18,2)/(0,0xb,0)`（`0045e1e0.c:35-36`）等个别 TSS 值未逐一命名（0xb=11?），不影响主链。

## 9. 与既有资产的关系
- 事件流 oracle（stage1_oracle.py/stage1_compare.py）的 RESULT 帧语义可继续沿用：把 H1 快照 + H3 顶点批 + 纹理表序列化成"语义绘制清单"（纹理(槽,entry) / 变换 / 顶点 / 混合 / 层序=提交序），与移植侧同构导出逐帧比对——即 AGENTS.md「渲染语义对齐」的原版侧采集端。
- ANM 数据侧（sprite/script 名、UV）已在 anm-pipeline.md 全量实证，捕获清单只需 (槽, sprite id, matrix, color, blend) 五元组 + 动态纹理事件。
