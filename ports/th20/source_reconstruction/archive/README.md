# 独立 C++ 资源包读取模块

`archive.cpp` 实现 THA1 包头/目录解析、按文件名选解码参数、数据解码及 LZSS 解压。库只依赖 C++ 标准库，运行时不读取、不映射、不执行原游戏 EXE，也不调用 thtk。它用于继续搭建完整源码版的资源加载层，本身不是游戏。

## 逆向依据

| 原 VA | 恢复到 C++ 的内容 |
|---|---|
| `0x004100e0` | 分块字节置换与递增密钥；奇数长度及不足四分之一块的尾部规则 |
| `0x00456270` | 文件名字节和取低三位选择解码参数 |
| `0x005ae000`（数据） | 从原 EXE 读取的八条密钥/步长/块长/限制常量 |
| `0x005391f0` | MSB 位流、13 位字典地址、4 位长度加三、零地址终止、8192 字节环形字典 |
| `0x00539ed0` | 包头验证、目录偏移及解码 |
| `0x0053a210`、`0x0053a680` | 名称四字节对齐、offset/size/extra、最后条目的存储末端 |
| `0x0053a350` | 不区分 ASCII 大小写的线性首项查找，保留重复名 |
| `0x0053a3c0` | 按元数据读取、先解码后解压、未压缩成员直接返回 |

主要依据为 `analysis/ghidra/pseudocode` 对应地址和原 PE 数据。此前使用的固定版本 thtk (`third_party/thtk/thtk/thdat95.c`、`thcrypt.c`) 用于独立格式交叉核对与提取参考；本模块没有链接该工具。

## 已验证

- 实际编译的 C++ 读取原 `th20.dat`，独立解码全部 **285 个记录，152,040,763 字节**，与之前 thtk 提取的每个文件逐字节一致。
- 两个重复文件名按索引与 `assets/duplicate_entries` 的独立提取比较，未用同一条目的输出充当两个参考。
- `archive_tests.cpp` 覆盖 LZSS 重叠回引用、字典回绕、跨调用保留字典、显式复位、截断/越界，以及解码尾部规则。
- 结果在 `archive_validation.json`。这项验证没有运行原 EXE。

原解压器使用全局字典而且每次只将游标设为 1，未清空字典。因此 `LzssDecoder` 对象保留跨调用字典状态；需要按原资源系统的并发/序列所有权接入。未来多个 Archive 对象之间的全局字典共享和原文件控制器同步仍需集成验证。

## 复跑

```powershell
cmake -S source_reconstruction/archive -B source_reconstruction/archive/build -G 'Visual Studio 16 2019' -A Win32
cmake --build source_reconstruction/archive/build --config Release
ctest --test-dir source_reconstruction/archive/build -C Release --output-on-failure
.\source_reconstruction\archive\build\Release\th20_source_archive_verify.exe 'E:\BaiduNetdiskDownload\PCGAME-Touhou.Kinjoukyou.Fossilized.Wonders-P2P\Touhou.Kinjoukyou.Fossilized.Wonders\th20.dat' assets/raw source_reconstruction/archive/archive_validation.json
```

## 边界

这里恢复的是有效数据上的算法，使用了独立的 owning C++ API。它没有复现原类的 vtable、内存分配器跟踪、整个文件控制器与区域设置相关的非 ASCII 文件名比较行为。新增检查会拒绝非法长度、越界解压和原实现可能越界的参数；这些拒绝行为不宣称与损坏文件下的原程序等价。不能将此模块标成对应所有原函数均已完成 ABI 恢复。
