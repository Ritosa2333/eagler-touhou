/** Old operator commands may still set react. They cannot select another UI. */
export function assertReactFrontendEnvironment(environment=process.env) {
  if (environment.EAGLER_FRONTEND !== undefined && environment.EAGLER_FRONTEND !== 'react') {
    throw new Error('该版本只提供 React 前端，请移除旧的 EAGLER_FRONTEND 设置');
  }
}
