# SDL 原生层来源与许可

来源：[saekaze/th12-switch](https://github.com/saekaze/th12-switch)，固定提交见 [source.json](source.json)。原作者为 saekaze；其 C++ 前端中的 MIT 派生实现来自 Tweet-META 的 TH12 网页还原。迁入保留以下许可边界。

| 文件 | 许可 |
| --- | --- |
| `Animation.*`、`Scene.*`、`Audio.*`、`Game.*`、`TitleScenes.*`、`TitlePresentation.*`、`Profile.*`、`Replay.*`、`Ending.*` | [TH12 MIT](licenses/TH12-MIT.txt) |
| `main.cpp`、`Platform.*`、`Memory.cpp`、`Archive.*`、`ExeTables.*`、`Anm.*`、`Renderer.*`、`TextRaster.*`、`BgmStream.*`、`scripts/gen-cp932.py` | [CC0-1.0](licenses/CC0-1.0.txt) |
| `GraphicsApi.*`、`Application.cpp`、`GameHost.cpp`、`FrameClock.hpp`、`RuntimeOptions.hpp`、`PracticeOverlay.*`、`BinaryReader.hpp`、`Localization.cpp`、`ResourceImages.*`、`Host.cmake`、`MiniaudioConfig.hpp` 的主仓库接入修改 | TH12 MIT |
| `third_party/stb_image.h` | [stb 原始许可](third_party/stb_image.LICENSE) |
| `third_party/miniaudio.h` | [miniaudio 原始许可](third_party/miniaudio.LICENSE) |
| `third_party/khronos/` | 各头文件内保留的 Khronos 许可；来源提交见 `source.json` |

`Archive.cpp` 的 TH12 归档参数沿原移植使用 thpatch/thtk 的研究资料。原版游戏、角色、文字、图像、音乐与音效归上海爱丽丝幻乐团所有。本目录不分发原版资源。
