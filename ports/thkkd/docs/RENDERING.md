# 网页显示与原作对照

海惠堂使用 640×480 的逻辑坐标。本机原游戏的五位显示配置为 `10110`，选择 640×480、32 位颜色的全屏输出。网页版保留相同尺寸的 WebGL 绘制缓冲区，全屏时按 4:3 放大到浏览器可用区域，系统桌面继续使用当前分辨率。

## 贴图与混合

原程序在 `0x653286` 压入参数 1，随后调用 `SetDrawMode`，采用双线性过滤。网页版的纹理保持 `LINEAR`，继续使用原裁切、旋转、缩放、裁剪区域及绘制顺序。三张共 79 字节的显示索引表、原始素材、核心、生成显示源码和 WASM 均沿用已验证版本。

PNG 解码明确指定 `premultiplyAlpha: 'none'` 和 `colorSpaceConversion: 'none'`，保留原始 RGB 与透明度。渲染器再按原混合模式应用透明度。原先的默认 ImageBitmap 解码会先把 RGB 乘以透明度，随后 `SRC_ALPHA` 混合再次相乘，导致半透明弹幕、武器和自机边缘变暗。实际 GPU 读回中，`[255,64,0,128]` 曾变成 `[128,32,0,128]`；明确关闭预乘后保留原值。独立 `_a` 遮罩仍按资源准备脚本的既有配方与原生透明度相乘。

[WebGL 规范](https://registry.khronos.org/webgl/specs/latest/1.0/)说明 ImageBitmap 的预乘处理由其创建选项决定；现有 `UNPACK_PREMULTIPLY_ALPHA_WEBGL` 设置继续服务其他图像来源。纹理解码、上传和缓存只发生在资源生命周期内。

## 字体与布局

本机原程序的最终 `CreateFontA` 跟踪得到 38 次微软雅黑创建调用。菜单对应 26 号、300 字重、抗锯齿与描边字体；网页版按原 `Thick × 100` 保留粗细分档，超出 CSS 范围的粗细使用最高 900 字重。默认字体使用 400 字重。

文字优先使用系统微软雅黑，随后选择 Noto Sans CJK、苹方和 Arial，随包的 Unifont 补齐字形。字体文件、加载完成屏障和离线缓存沿用既有流程。每个字符的宽度按实际字体测量并取整，描边占用两侧字距；宽度查询与绘制共用同一布局。原非抗锯齿字体先生成二值字形，再扩展描边，保留清晰的小字。文本与布局均设置有界缓存。

原作 `Game Start` 的前景范围宽 149 像素；旧网页字体宽 127 像素，本轮网页前景宽 148 像素。浏览器与 GDI 各自栅格化字体，当前对照保留字形、字重、行位与字距的实际结果，详见验收图。

## 全屏与 DPI

`display.mjs` 根据设备像素计算 4:3 显示区域，边长使用 4 与 3 的整数倍并居中。窗口、横竖屏、DPI 和缩放变化会重新计算尺寸；呈现循环额外检查未触发 resize 的 DPI 变化。此计算只调整 CSS，逻辑画布和更新频率保持 640×480、60 Hz。FPS 标记使用相同画布边界定位。

网站的“像素缩放”继续控制画布最终放大时的采样方式，游戏内部贴图保留原双线性过滤。高刷新率呈现继续读取命令快照，显示尺寸计算不会推进游戏、消费输入或创建音频实例。

## 验证入口

完整证据见[本轮对照记录](../../../archive/thkkd-rendering-20261126/README.md)。以下命令从已打包站点检查真实 Runtime：

```powershell
node ports/thkkd/tests/rendering-browser.mjs --url=http://127.0.0.1:8174/ --offline=1
node ports/thkkd/tests/rendering-corpus-browser.mjs --url=http://127.0.0.1:8174/ --replays=path/to/authorized-replays --baseline=path/to/before-captures
python -X utf8 -B ports/thkkd/scripts/compare-rendering.py --originals=path/to/original-captures --before=path/to/before-captures --after=path/to/current-captures --output=path/to/comparison
```

录像目录及原程序采集属于已有授权验证材料，游戏构建和发布独立于这些材料。显卡读回检查使用随源码保存的 18 个 RGBA 样本，覆盖自机、判定点和带独立遮罩的弹型。
