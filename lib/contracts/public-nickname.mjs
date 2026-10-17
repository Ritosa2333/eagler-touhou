import {loadCompiledContract} from './load-compiled-contract.mjs';
const contract = await loadCompiledContract('public-nickname');
export const {normalizePublicNickname, PUBLIC_NICKNAME_STATUSES, parsePublicNicknameState} = contract;
