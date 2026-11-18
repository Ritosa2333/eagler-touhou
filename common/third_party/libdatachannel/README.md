# 原生 WebRTC 依赖补丁

联机 Relay 配置了 NAT4 观测节点时，原生构建通过 libjuice 的同一个 ICE UDP socket 访问三个 STUN 端点。普通单 STUN 建连沿用 libjuice 的地址选择。

Windows 构建把 UDP 轮询和 ICE 地址解析提交到系统线程池，避免信令同步等待线程创建。

在专用依赖目录中取出带子模块的 libdatachannel `bdc5ff28e9d3b863144c94a677ecf5bf043aaf15`。该版本固定 libjuice `b89c792e3612faf2f12cf35bcc56857313a06be3`。将该目录的绝对路径赋给 `$rtcSource`，从本仓库根目录应用补丁，然后把依赖目录传给构建参数 `TH09_LIBDATACHANNEL_SOURCE`。

```powershell
$patch = (Resolve-Path common/third_party/libdatachannel).Path
git -C $rtcSource apply "$patch/libdatachannel.patch"
git -C "$rtcSource/deps/libjuice" apply "$patch/libjuice.patch"
```

这两份补丁按对应上游项目的 MPL-2.0 许可提供。
