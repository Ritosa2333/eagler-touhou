// Versus cards in native top/middle/bottom order: first three are 壱符,
// last three are 弐符. Story-only cards are not selectable here.
// Names/order: https://thbwiki.cc/东方萃梦想/符卡 and each character's versus table.
const characters = [
  ['博丽灵梦', 'Reimu Hakurei', [
    ['灵符「梦想妙珠」', 'Spirit Sign "Fantasy Orb"'], ['梦符「封魔阵」', 'Dream Sign "Evil-Sealing Circle"'], ['宝符「阴阳宝玉」', 'Treasure Sign "Yin-Yang Orb"'],
    ['神灵「梦想封印」', 'Divine Spirit "Fantasy Seal"'], ['神技「八方鬼缚阵」', 'Divine Art "Omnidirectional Demon-Binding Circle"'], ['宝具「阴阳鬼神玉」', 'Holy Relic "Yin-Yang Sanctifier Orb"'],
  ]],
  ['雾雨魔理沙', 'Marisa Kirisame', [
    ['恋符「极限火花」', 'Love Sign "Master Spark"'], ['魔符「星尘幻想」', 'Magic Sign "Stardust Reverie"'], ['仪符「太阳仪」', 'Ritual Sign "Orrery\'s Sun"'],
    ['魔炮「究极火花」', 'Magicannon "Final Spark"'], ['彗星「闪亮之星」', 'Comet "Blazing Star"'], ['天仪「太阳系仪」', 'Celestial Apparatus "Orrery\'s Solar System"'],
  ]],
  ['十六夜咲夜', 'Sakuya Izayoi', [
    ['幻符「杀人玩偶」', 'Illusion Sign "Killing Doll"'], ['时符「个人空间」', 'Time Sign "Private Square"'], ['伤符「铭刻于红魂」', 'Wound Sign "Inscribe Red Soul"'],
    ['幻葬「夜雾幻影杀人鬼」', 'Illusion Burial "Phantomic Killer in Night Mist"'], ['「咲夜的世界」', '"The World of Sakuya"'], ['伤魂「灵魂雕塑」', 'Wound Soul "Soul Sculpture"'],
  ]],
  ['爱丽丝·玛格特洛依德', 'Alice Margatroid', [
    ['魔符「狡猾的献祭」', 'Magic Sign "Artful Sacrifice"'], ['战符「小小军势」', 'War Sign "Little Legion"'], ['咒符「上海人偶」', 'Curse Sign "Shanghai Doll"'],
    ['魔操「回归虚无」', 'Magic Manipulation "Return Inanimateness"'], ['战操「玩偶战争」', 'War Manipulation "Dolls\' War"'], ['诅咒「蓬莱人偶」', 'Malediction "Hourai Doll"'],
  ]],
  ['帕秋莉·诺蕾姬', 'Patchouli Knowledge', [
    ['火金符「圣爱尔摩火柱」', 'Fire Metal Sign "St. Elmo Pillar"'], ['土水符「诺亚的大洪水」', 'Earth Water Sign "Noachian Deluge"'], ['金木符「元素收获者」', 'Metal Wood Sign "Elemental Harvester"'],
    ['日符「皇家烈焰」', 'Sun Sign "Royal Flare"'], ['月符「沉静的月神」', 'Moon Sign "Silent Selene"'], ['火水木金土符「贤者之石」', 'Fire Water Wood Metal Earth Sign "Philosopher\'s Stone"'],
  ]],
  ['魂魄妖梦', 'Youmu Konpaku', [
    ['人符「现世斩」', 'Human Sign "Slash of Present World"'], ['断命剑「冥想斩」', 'Life-Cutting Sword "Meditation Slash"'], ['魂符「幽明的苦轮」', 'Soul Sign "Wheel of Pain of the Living and Dead"'],
    ['人鬼「未来永劫斩」', 'Human Oni "Slash of the Eternal Future"'], ['断迷剑「迷津慈航斩」', 'Confusion-Cutting Sword "Slash of Departing Hesitation"'], ['魂魄「幽明求闻持聪明之法」', 'Soul "Sekha Enlightenment"'],
  ]],
  ['蕾米莉亚·斯卡蕾特', 'Remilia Scarlet', [
    ['红符「红色不夜城」', 'Scarlet Sign "Scarlet Brilliance"'], ['必杀「碎心」', 'Certain Kill "Heart Break"'], ['夜符「恶魔之王的摇篮」', 'Night Sign "Demon King Cradle"'],
    ['红魔「绯红恶魔」', 'Scarlet Devil "Scarlet Devil"'], ['神枪「冈格尼尔之枪」', 'Divine Spear "Spear the Gungnir"'], ['夜王「德古拉的摇篮」', 'Night King "Dracula Cradle"'],
  ]],
  ['西行寺幽幽子', 'Yuyuko Saigyouji', [
    ['死符「惊梦」', 'Death Sign "Ghastly Dream"'], ['冥符「黄泉平坂行路」', 'Nether Sign "Path to Yomotsu-Hirasaka"'], ['灵符「无寿之梦」', 'Spirit Sign "Dream of Agelessness"'],
    ['死蝶「华胥的永眠」', 'Deadly Butterfly "Eternal Sleep in Dreamland"'], ['再迷「幻想乡的黄泉归」', 'Re-Lost "Gensokyo\'s Rise from Yomi"'], ['寿命「通向无寿国的期票」', 'Lifespan "Ticket to the Ageless Land"'],
  ]],
  ['八云紫', 'Yukari Yakumo', [
    ['境符「四重结界」', 'Boundary Sign "Quadruple Barrier"'], ['式神「八云蓝」', 'Shikigami "Ran Yakumo"'], ['境符「二次元与三次元的界线」', 'Boundary Sign "Boundary of 2D and 3D"'],
    ['结界「充满魅力的四重结界」', 'Barrier "Charming Quadruple Barrier"'], ['式神「八云蓝+」', 'Shikigami "Ran Yakumo+"'], ['结界「客观结界」', 'Barrier "Objective Barrier"'],
  ]],
  ['伊吹萃香', 'Suika Ibuki', [
    ['萃符「户隐山之投」', 'Gathering Sign "Throwing Mt. Togakushi"'], ['醉符「鬼缚之术」', 'Drunk Sign "Art of Oni Binding"'], ['鬼符「遗失的力量」', 'Oni Sign "Missing Power"'],
    ['萃鬼「天手力男之投」', 'Gathering Oni "Throwing Ama-no-Tajikarao"'], ['醉梦「施饿鬼缚之术」', 'Drunken Dream "Art of Gaki Binding"'], ['鬼神「遗失的高贵之力」', 'Oni God "Missing Purple Power"'],
  ]],
  ['红美铃', 'Hong Meiling', [
    ['彩符「彩光风铃」', 'Colorful Sign "Colorful Light Wind Chime"'], ['光符「华光玉」', 'Light Sign "Blossoming Light Orb"'], ['华符「破山炮」', 'Flower Sign "Mountain Breaker"'],
    ['极彩「彩光乱舞」', 'Extreme Color "Colorful Light Chaotic Dance"'], ['极光「华严明星」', 'Extreme Light "Flower Adornment Star"'], ['三华「崩山彩极炮」', 'Three Flowers "Mountain-Crushing Colorful Cannon"'],
  ]],
] as const;

export const TH075_LOADOUT_MESSAGES: readonly (readonly [string, string, string])[] = characters.flatMap(([zh, en, cards], character) => [
  [`multiplayer.character.th075_${character}`, zh, en] as const,
  ...cards.map(([zh, en], card) => [`multiplayer.spell.th075_${character}_${card}`, zh, en] as const),
  ...Array.from({length:9}, (_, shot) => [`multiplayer.loadout.th075_${character}_${shot}`,
    `${zh} · ${cards[shot%3]![0]} / ${cards[3+Math.floor(shot/3)]![0]}`,
    `${en} · ${cards[shot%3]![1]} / ${cards[3+Math.floor(shot/3)]![1]}`] as const),
]);
