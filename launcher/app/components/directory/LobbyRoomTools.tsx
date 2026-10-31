import { RefreshCw } from 'lucide-react';
import { useLocale } from '../../i18n';
import { Button } from '../ui/button';

/** Shared original lobby.html room actions, including its disabled boot state. */
export function LobbyRoomTools({
  disabled,
  onCreate,
  onJoin,
  onRefresh,
  onReplay,
}: {
  disabled: boolean;
  onCreate?: () => void;
  onJoin?: () => void;
  onRefresh?: () => void;
  onReplay?: () => void;
}) {
  const { t } = useLocale();
  return (
    <>
      <Button
        variant="legacy"
        size="legacy"
        className="lobby-button lobby-primary"
        id="createButton"
        type="button"
        disabled={disabled}
        onClick={onCreate}
      >
        {t('lobby.create')}
      </Button>
      {onReplay && (
        <Button
          variant="legacy"
          size="legacy"
          className="lobby-button lobby-secondary"
          id="watchReplayButton"
          type="button"
          onClick={onReplay}
        >
          {t('lobby.watchReplay')}
        </Button>
      )}
      <Button
        variant="legacy"
        size="legacy"
        className="lobby-button lobby-secondary"
        id="codeButton"
        type="button"
        disabled={disabled}
        onClick={onJoin}
      >
        {t('lobby.byCode')}
      </Button>
      <Button
        variant="legacy"
        size="legacy"
        className="lobby-button lobby-refresh"
        id="refreshRooms"
        type="button"
        aria-label={t('lobby.refresh')}
        onClick={onRefresh}
      >
        <RefreshCw size={24} aria-hidden="true" focusable="false" />
      </Button>
    </>
  );
}
