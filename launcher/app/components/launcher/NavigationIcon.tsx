import {Sun, Moon, Menu, PanelRightOpen, UserRound, Gamepad2, UsersRound, FileCheck2, CircleHelp, BookOpenText, LayoutTemplate} from 'lucide-react';
const icons = {
  sun: Sun, moon: Moon, menu: Menu, drawer: PanelRightOpen, profile: UserRound, single: Gamepad2, multiplayer: UsersRound,
  firstUse: FileCheck2, faq: CircleHelp, multiplayerGuide: BookOpenText, handbook: LayoutTemplate,
};
export function NavigationIcon({name}: {name: keyof typeof icons}) {
  const Icon = icons[name];
  return <Icon className="book-navigation-icon" aria-hidden="true"/>;
}
