# TH01–TH04 服务器构建

本分支的四作运行代码、原生补丁和 `ports/th01` 至 `ports/th04` 的构建输入由 Git 管理。每作的 `source-import.json` 列出对应的资源输入；`build-eagler/`、`content/`、`node_modules/` 和 `.cache/` 都是生成目录，需要在服务器上重新生成。

需要 Node.js 22.18 或更新版本、Bun 1.4.4 或更新版本、Python 3、NASM、Rust 和 Cargo，并安装 Rust 的 `wasm32-unknown-unknown` 目标。安装启动器和 Relay 的锁定依赖：

```sh
cd launcher
bun install --frozen-lockfile --ignore-scripts
cd ../services/relay
npm ci
cd ../..
rustup target add wasm32-unknown-unknown
```

从仓库根目录构建四作和启动器：

```sh
for game in th01 th02 th03 th04; do
  (cd "ports/$game" && node scripts/build-eagler.mjs) || exit 1
done
cd launcher
node scripts/build-launcher.mjs --force
cd ..
```

每作的 `ports/<game>/build-eagler/` 是运行组件，`ports/<game>/content/` 包含生成的 `.data` 和音乐。构建脚本会检查原生练习资源和运行组件的资源摘要；构建后应作为同一版本部署，避免清单和文件不一致。

TH02–TH04 构建还会生成 `content/disks.json` 和 `content/disk-blocks/`。Host 组装使用这份清单发布按内容校验的静态块，启动器按所选语言与模式安装并缓存所需块，运行时在浏览器内还原磁盘。发布前 Host 会验证块还原的磁盘与 `.data` 中的原始磁盘一致。准备和迁移构建输入时，请将这两个路径与 `.data` 一起保留；没有静态块的旧输入仍使用完整 `.data` 发布。

Host 组装时会为 TH02–TH04 的联机 WASM 生成并校验 gzip 副本，保存于站点的 `_pc98-wasm-gzip/`。内置静态服务器在浏览器接受 gzip 时发送该副本，并以原 WASM 地址、`application/wasm` 和 `Content-Encoding: gzip` 响应；不支持时仍使用原文件。原始 WASM 与运行时清单继续保留，打包上传时仍要考虑原始文件的大小。使用外部静态服务器或 CDN 时，需要另行启用 WASM 的 gzip 或 Brotli，并检查线上实际响应；这些 gzip 副本不会被外部服务器自动选用。

本地测试可在两个终端分别运行 Relay 和开发服务器：

```sh
node services/relay/src/netplay-relay.mjs
EAGLER_DEVELOPMENT_GAMES=th01,th02,th03,th04 \
EAGLER_TOUHOU_NETPLAY_RELAY=ws://127.0.0.1:18142 \
node launcher/scripts/serve.mjs 8130
```

第二条命令只适合本机访问：公网客户端需要能够访问的 Relay 地址和 HTTPS/WSS。Relay 的生产配置及代理说明见 [Relay 文档](../services/relay/README.md)。开发服务器不等同于正式部署包；正式站点的 Runtime Release、Host 组装和验证流程见 [自托管文档](../launcher/docs/SELF_HOSTING.md)及[发布文档](../launcher/docs/RELEASE.md)。现有正式 Host 流程要求 Product Catalog 的全部已登记作品，不能直接把上述四作开发服务器当成完整正式发布包。
