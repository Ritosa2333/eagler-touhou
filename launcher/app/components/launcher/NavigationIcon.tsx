import {Sun, Moon, Menu, UserRound, Gamepad2, UsersRound} from 'lucide-react';
const icons = {sun: Sun, moon: Moon, menu: Menu, profile: UserRound, single: Gamepad2, multiplayer: UsersRound};
export function NavigationIcon({name}: {name: keyof typeof icons}) {
  const Icon = icons[name];
  return <Icon className="book-navigation-icon" aria-hidden="true"/>;
}
