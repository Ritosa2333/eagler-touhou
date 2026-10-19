# 外置资源模式

外置模式把玩家使用的 Launcher 部署在一个 HTTPS 源站，把游戏包部署在与之配对的 Hosted 源站。外置站保留 Launcher App Shell 和 Runtime 文件；仅 `/games/**` 与 `/shared/**` 包资源转向 Hosted 源站。

## 构建配对站点

在包含已验证 Runtime Release 和全部原作输入的 Host 工作区运行：

```sh
npm run host:build
npm run package:external-site -- \
  --source=dist/site \
  --runtime-release=runtime-release \
  --output=dist/external-site \
  --profile=web-release-external
npm run verify:server -- dist/site
npm run verify:server -- dist/external-site
```

`dist/site/` 是完整资源源站；`dist/external-site/` 是玩家访问的站点。打包器校验 Hosted 源站，并复制相同的 Package Descriptor 与所选 Runtime 文件。成功构建后会替换已有的 `dist/external-site/`；生成外置站期间保持 Hosted 源站不变。

## 配置两个源站

将两个目录内容分别发布到各自的 HTTPS 文档根目录。在玩家站只将游戏包和共享资源路径重定向到资源源站：

```nginx
location ^~ /games/ { return 307 https://assets.example.org$request_uri; }
location ^~ /shared/ { return 307 https://assets.example.org$request_uri; }
```

资源源站允许准确的玩家站 Origin，并公开包下载需要的响应头：

```nginx
add_header Access-Control-Allow-Origin "https://play.example.org" always;
add_header Access-Control-Expose-Headers "Accept-Ranges, Content-Length, Content-Range, ETag" always;
add_header Vary "Origin" always;
```

资源源站和 CDN 保留 `v` 等查询参数、`Range` 请求头及 `Accept-Ranges`、`Content-Length`、`Content-Range` 响应头。可满足的一字节 Range 请求返回 `206 Partial Content` 和对应范围。最终响应都应用 CORS 响应头，包括部分响应和错误响应。

外置站的 App Shell Service Worker 只控制自己的源站，远端 Package 资源不进入其缓存。资源源站与外置站使用同一 Hosted 版本，保证 Package 元数据对应实际文件。

## 发布与验证

先把两个站点发布到新的版本目录，先验证资源源站，再切换玩家站到配对版本。Package 版本变化时两个站点要一起回滚。

```sh
npm run verify:deployed -- https://assets.example.org/
npm run verify:deployed -- https://play.example.org/
```

验证器检查部署清单、资源重定向、最终 CORS 响应头和 Range 响应。还应在支持的浏览器打开玩家站、安装一个游戏包并启动 Runtime，确认跨源流程完整。

Host 配置中的 `externalImportSource` 是玩家手动下载 ZIP 的入口，与本模式无关。普通 Hosted 构建见[自托管](SELF_HOSTING.md)，三种交付方式见[自托管参考](SELF_HOSTING_REFERENCE.md)。
