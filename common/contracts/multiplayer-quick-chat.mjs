// Populate only after the wording has been chosen. The relay and UI use the
// same IDs, so clients never submit arbitrary text or a claimed sender name.
export const QUICK_CHAT_ROWS = Object.freeze([
    [{ id: "1", zh: "1", en: "1" }],
    [{ id: "request-life", zh: "请求给条命（停止开火，然后在我旁边连点 5 次低速即可）", en: "Life please (stop firing, then tap focus 5 times next to me)" }],
    [{ id: "request-power", zh: "请求给 Power（在我身旁快速按多下开火键即可）", en: "Power please (tap fire repeatedly next to me)" }],
    [{ id: "share-resources", zh: "请均分资源", en: "Please share resources evenly" }],
    [{ id: "follow-me", zh: "跟着我", en: "Follow me" }, { id: "spread-out", zh: "分开走", en: "Spread out" }],
    [{ id: "stop-fire", zh: "停枪", en: "Stop firing" }],
    [{ id: "bomb-me", zh: "我开 Bomb", en: "I'll use Bomb" }, { id: "bomb-you", zh: "你开 Bomb", en: "You use Bomb" }],
    [{ id: "leaving", zh: "退了", en: "I'm leaving" }],
    [{ id: "last-game", zh: "最后一局吧", en: "One last game?" }, { id: "last", zh: "last", en: "last" }],
    [{ id: "thanks", zh: "谢谢指教", en: "Thanks for the games" }, { id: "xxzj", zh: "xxzj", en: "xxzj" }],
    [{ id: "wait", zh: "等一下", en: "Wait a moment" }],
    [{ id: "sorry", zh: "非常抱歉", en: "I am so sorry" }],
    [{ id: "continue", zh: "继续吧", en: "Let’s continue" }],
    [{ id: "lagging", zh: "卡了卡了", en: "I am lagging" }],
    [{ id: "no-worries", zh: "没事没事", en: "No worries" }],
    [{ id: "laugh", zh: "笑死", en: "That’s hilarious" }],
    [{ id: "retry", zh: "重新来吗", en: "Try again?" }],
    [{ id: "no-no", zh: "No no no no", en: "No no no no" }],
    [{ id: "yes-yes", zh: "Yes yes", en: "Yes yes" }],
    [{ id: "ah", zh: "あぁー", en: "Ah" }],
    [{ id: "move-right", zh: "向右微移", en: "Move slightly right" }],
    [{ id: "move-left", zh: "向左微移", en: "Move slightly left" }],
    [{ id: "busy", zh: "有点事情", en: "Something came up" }],
    [{ id: "you-first", zh: "你先选", en: "You choose first" }],
    [{ id: "nice", zh: "nice", en: "Nice" }],
    [{ id: "amazing", zh: "好厉害", en: "Amazing" }],
    [{ id: "great-opportunity", zh: "多么好的机会啊", en: "What a great opportunity" }],
    [{ id: "feels-good", zh: "好舒服啊", en: "That feels good" }],
]);
export const QUICK_CHAT_PHRASES = Object.freeze(QUICK_CHAT_ROWS.flat());
export function quickChatPhrase(id) {
    return typeof id === "string" ? QUICK_CHAT_PHRASES.find(phrase => phrase.id === id) : undefined;
}
