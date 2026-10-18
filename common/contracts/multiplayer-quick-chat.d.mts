export interface QuickChatPhrase {
    readonly id: string;
    readonly zh: string;
    readonly en: string;
}
export declare const QUICK_CHAT_ROWS: readonly (readonly QuickChatPhrase[])[];
export declare const QUICK_CHAT_PHRASES: readonly QuickChatPhrase[];
export declare function quickChatPhrase(id: unknown): QuickChatPhrase | undefined;
