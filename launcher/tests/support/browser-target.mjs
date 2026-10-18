/** 外部浏览器场景必须声明目标，不连接碰巧占用历史端口的服务。 */
export function browserTestUrl({
  environment = 'EAGLER_TEST_URL',
  args = process.argv.slice(2),
} = {}) {
  const explicit = args.find((value) => value.startsWith('--url='))?.slice('--url='.length);
  const value = explicit || process.env[environment];
  if (!value) throw new Error(`Pass --url=URL or set ${environment} to the Launcher under test`);
  const url = new URL(value);
  if (!['http:', 'https:'].includes(url.protocol) || url.username || url.password) {
    throw new Error('Launcher test URL must use HTTP(S) without embedded credentials');
  }
  return url.href;
}
