import {readCurrentExtraAccess, requestNormalExtraStart} from './extra-eligibility.mjs';

export function describeNormalProgress(progress) {
  const satisfied = progress?.policySatisfied === true && progress?.count === 6;
  return Object.freeze({
    eligibilityText: satisfied
      ? '规则条件已满足：6/6（仅事实规则，不是可信原生通关证明）。'
      : `资格未满足：${Number.isInteger(progress?.count) ? progress.count : 0}/6；每线须 Normal rank ≥1、真实全路线 clear、原 ending 完成且全程 no-taint。`,
    runtimeText: '运行支持未完成：实际 rank 4 尚未验收；即使资格满足也不能启动。stage 16 不是 Normal rank 1。',
    canStart: false,
  });
}

export function mountExtraMenu(documentRef = document) {
  const required = selector => {
    const node = documentRef.querySelector(selector);
    if (!node) throw new Error(`Missing visible Extra menu target: ${selector}`);
    return node;
  };
  const panel = required('#extra-menu-panel');
  const normalButton = required('#normal-extra-start');
  const debugButton = required('#debug-extra-start');
  const status = required('#extra-status');
  const refresh = () => {
    const access = readCurrentExtraAccess();
    const presentation = describeNormalProgress(access.progress);
    required('#extra-progress').textContent = `Normal Extra · Locked · ${access.progress.count}/6`;
    required('#extra-provider').textContent = '系统未实现：缺 trusted normal clear provider；当前没有可信原生通关提交与 ending 完成证明。资格只读 0/6，不读取本地解锁标志。';
    required('#extra-eligibility').textContent = presentation.eligibilityText;
    required('#extra-runtime').textContent = presentation.runtimeText;
    required('#extra-missing').textContent = `缺少可信完整通关：${access.progress.missingRoutes.join('、')}。未知不是已通关，也不是已确认失败。`;
    for (const row of documentRef.querySelectorAll('[data-extra-route]')) {
      const entry = access.progress.routes.find(value => value.route === row.dataset.extraRoute);
      row.textContent = entry
        ? `${entry.route}：未确认 · 缺可信 Normal 全路线 clear + 原 ending；不能用第三面进入、挂机或 control15/16 补数。`
        : '未知路线：不计资格。';
    }
    normalButton.disabled = !access.canStart;
    normalButton.setAttribute('aria-disabled', String(!access.canStart));
    normalButton.textContent = `Normal Extra（锁定 ${access.progress.count}/6）`;
    debugButton.disabled = true;
    debugButton.setAttribute('aria-disabled', 'true');
    debugButton.textContent = 'Debug EX · tainted（开发中／未验收）';
    return access;
  };
  normalButton.addEventListener('click', () => {
    try {
      requestNormalExtraStart();
    } catch (error) {
      status.textContent = error.message;
    }
    refresh();
  });
  required('#extra-refresh').addEventListener('click', () => {
    refresh();
    status.textContent = '只读检查完成：Normal Extra 仍锁定；无通关记录或本地存档被修改。';
  });
  panel.addEventListener('keydown', event => event.stopPropagation());
  refresh();
  return Object.freeze({readAccess: readCurrentExtraAccess, requestNormalStart: requestNormalExtraStart, refresh});
}
