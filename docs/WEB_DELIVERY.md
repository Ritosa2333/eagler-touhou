# TH128 网页交付 · 第一版

六条普通路线和 EX 完整单人功能流程已完成。此版直接复用成功的 private108 核心，不重新编译或重跑通关。

| 文件 | 大小 | SHA-256 |
|---|---:|---|
| th128-runtime108-v1.zip | 3095006 | 408baef78d529990f49a5d172793928c13980a91ea967f2fba08458459aa23fd |
| th128-import-5eb938644815ee24.zip | 344939528 | 9d23ac6878cb7c5296c6e17a5d154db7115da22a6562091192113ac9f3722cf0 |

资源包包含738个文件、344418593字节payload，规范为 `eagler-touhou/package/1`、game/target为th128；配套解压目录根有package.json。首版提供中文与原WAV/PCM，不广告尚未交付的JA/English/OGG/MIDI。

布局标识：`sha256-bd7ec34789cf733f6a04d0abe34364aca179f663e2b6956db615af4e917b7eed`。
Runtime核心代次：`b7f885be544802659eda1848a86bd9ee964a985f13b73d9a4705df2d9b25e744`。

## 启动

解压 Runtime，运行其中的Start.ps1，资源路径默认使用相邻的resource-package-v1/extracted。路径不同时：
```powershell
powershell -ExecutionPolicy Bypass -File .\Start.ps1 -Package "资源包解压目录"
```
随后打开终端显示的地址，默认 `http://127.0.0.1:8138/play.html`。需要Python3.11+和支持WebAssembly/WebGL的浏览器。启动先校验资源文件，不修改旧8128页面、Windows原版或其它来源的存档。

## 已验证

- 661：生成器11项组件测试通过。660首失败仅为测试将追加文件造成的size-mismatch误期望为hash-mismatch；新号修正并另加同大小篡改检查，产品模块未变。
- 662：生成真实738文件资源ZIP与完全匹配解压目录；独立Python zipfile逐文件CRC、SHA及大小全部通过。
- 664：生成Runtime108目录与ZIP；实际临时HTTP服务核对核心字节、中文catalog、原标题ANM、结局资源和WAV206分段响应。服务器在finally关闭，0Factory/0World/0浏览器；663在准入前因路径分隔符不一致失败，未执行组装。
- 首版启动页移除已过时的“EX未完成”等描述，开发工具默认折叠。

## 接续

浏览器内ZIP导入、独立资源IndexedDB安装、失败更新保旧版、安装后重开与离线启动正在接入。当前可使用已校验的解压资源直接启动；上述HTTP验证不冒浏览器已打开或整局验收。

固定上游启动器的产品catalog尚无th128项；本包使用TH128配套Runtime扩展目标，不能声称上游原版启动器已经识别安装。后续提供相应注册/接入，不通过改名TH12数据来伪造支持。

源码仓库不附带原作资源和WASM二进制；上述两个ZIP与资源解压目录单独交付。额外原版录像、字体、设备等问题继续独立改进，不回退已完成的六线＋EX功能结论。
