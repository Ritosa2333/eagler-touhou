# 蓝奏完整离线包上传流程

本文记录 TH06/TH07 完整离线包通过临时 OpenList 上传到蓝奏的可重复流程。它只属于私有运维层；产品代码仍只认识通用的 `gameDataFallback = { url, hint? }`，不得加入蓝奏专用逻辑。

## 固定位置

```text
工作区：D:\workspace\eagler
OpenList：archive\temporary\tools\openlist-v4.2.5
上传脚本：archive\temporary\openlist-upload-file.py
蓝奏目录：/lanzou/TouhouEaglerData
```

OpenList 的现有 `data-upload` 数据目录保存了私有挂载配置。凭据不要复制到命令、文档、仓库、日志或回复中；上传脚本会从本机 OpenList 读取管理员令牌。

## 1. 准备包名

只上传已经完成构建和验收的完整离线 ZIP。使用内容散列区分版本，例如：

```text
th07-offline-fe2462d63ac3c81d.zip
```

先确认本地文件名和目标文件名完全一致。不要把 game-data-only ZIP 当成完整离线包。

## 2. 启动临时 OpenList，并绕过本机代理

在 PowerShell 中执行：

```powershell
Set-Location 'D:\workspace\eagler'

$openListExe = 'D:\workspace\eagler\archive\temporary\tools\openlist-v4.2.5\openlist.exe'
$openListDir = Split-Path -LiteralPath $openListExe
$proxyBypass = @{
  HTTP_PROXY  = ''
  HTTPS_PROXY = ''
  ALL_PROXY   = ''
  NO_PROXY    = '*'
}

$openListProcess = Start-Process `
  -FilePath $openListExe `
  -ArgumentList @('server', '--data', 'data-upload') `
  -WorkingDirectory $openListDir `
  -WindowStyle Hidden `
  -Environment $proxyBypass `
  -PassThru

$openListProcess.Id
```

等待 `127.0.0.1:5244` 开始监听：

```powershell
for ($attempt = 0; $attempt -lt 30; $attempt++) {
  $ready = Test-NetConnection 127.0.0.1 -Port 5244 -WarningAction SilentlyContinue
  if ($ready.TcpTestSucceeded) { break }
  Start-Sleep -Seconds 1
}
if (-not $ready.TcpTestSucceeded) { throw 'OpenList 5244 未就绪' }
```

必须保留这组空代理环境。此前直接经本机代理上传约 100 MB 文件很慢并发生超时；OpenList 直连蓝奏才是已实际成功的路径。

## 3. 上传文件

把示例中的本地和远端文件名替换成同一个新包名：

```powershell
$localZip = 'D:\workspace\eagler\archive\temporary\th07-offline-fe2462d63ac3c81d.zip'
$remoteZip = '/lanzou/TouhouEaglerData/th07-offline-fe2462d63ac3c81d.zip'

python .\archive\temporary\openlist-upload-file.py $localZip $remoteZip
```

成功时脚本输出一行 JSON，其中包含：

```json
{"ok":true,"local":"文件名.zip","bytes":98009854,"remote":"/lanzou/TouhouEaglerData/文件名.zip"}
```

这表示 OpenList 上传 API 已成功返回。不要在同一流程里自动下载大包做重复校验；玩家侧下载/导入验收应作为单独步骤按需要执行。

## 4. 停止本次临时 OpenList

只停止刚才 `Start-Process` 返回的进程：

```powershell
if ($openListProcess -and -not $openListProcess.HasExited) {
  Stop-Process -Id $openListProcess.Id
}
```

## 5. 旧包清理规则

上传新包不等于可以立即删除旧包。默认顺序是：

```text
上传新包 -> 玩家侧实际下载/导入确认 -> 得到明确删除授权 -> 删除指定旧包
```

删除前列出精确文件名，绝不按宽泛通配符清理，也不触碰当前 TH06/TH07 各自的有效包。目录列表返回的文件大小和修改时间不能代替玩家侧导入验收。

## 故障定位

- `127.0.0.1:5244` 不通：检查 `$openListProcess.HasExited`，并确认 `--data data-upload` 和工作目录没有改错。
- 上传超时：确认临时进程启动时四个代理环境变量确实按上文设置，不要改用直接 Python 请求蓝奏。
- API 返回非 200：保留错误正文，但不要在回复或文档中抄出令牌、Cookie 或挂载凭据。
- 文件名冲突：生成新的内容散列文件名，不覆盖旧包；待新包验收后再清理旧包。
