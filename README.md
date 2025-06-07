# 🐍 snake built in discord

this shit barely works dude i have to be honest im not fighting discord ratelimits here

## ✨ cool stuff this bot does

- 🎮 **snake go brrr** - snake moves by itself every 2 secs lol
- 🎯 **clicky buttons** - use these arrow thingies (⬆️⬇️⬅️➡️) to make snake go where u want
- 🍎 **nom nom** - eat the red dots to get bigger
- 💀 **ouch** - game over if u hit stuff
- 📊 **big number** - see how long ur snake got
- 🔄 **no spam** - game just updates the same message over and over
- 👥 **play with friends** - everyone can play at once if they want

## 🎯 how 2 play

1. type `/startsnake` somewhere in discord
2. click the buttons to move ur snake around:
   - ⬆️ go up
   - ⬇️ go down  
   - ⬅️ go left
   - ➡️ go right
3. eat the 🍎 things to get bigger
4. dont hit walls or urself lmao
5. see how big u can make it

## 🛠️ how to not break everything

### stuff u need

- c++ compiler that doesnt suck (clang++, g++)
- [dpp library](https://github.com/brainboxdotcc/DPP) (discord c++ thing)
- discord bot token (dont leak it)

### making it work

1. **get the code:**
   ```bash
   git clone <ur-repo-here>
   cd discordcpp
   ```

2. **install dpp:**
   ```bash
   # good luck lol
   # https://dpp.dev/install.html
   ```

3. **compile this mess:**
   ```bash
   clang++ -std=c++17 -O2 -Wall -Wextra -Wpedantic -pthread \
           -I./DPP/include main.cpp -o discord_bot \
           -L./DPP/build/library -ldpp \
           -framework CoreFoundation -framework Security
   ```

4. **put ur token somewhere:**
   ```bash
   export BOT_TOKEN=ur_actual_token_here_dumbass
   ```

5. **run the thing:**
   ```bash
   ./discord_bot
   ```

## 🔧 settings n stuff

### environment variables (fancy words for secrets)

- `BOT_TOKEN` - ur discord bot token (dont be stupid)

### game tweaks

u can change stuff in `main.cpp` if u want:

- **how fast** - change `milliseconds(2000)` to make snake zoom or crawl
- **board size** - mess with `SnakeGame(9, 9, ...)` for bigger/smaller grid  
- **starting size** - change how long snake starts

## 🎮 how this garbage works

### snake movement
- snake moves every 2 seconds whether u like it or not
- button clicks change direction instantly
- cant do 180s (no going left when going right u idiot)

### scoring
- score = how long ur snake is
- starts at 1 (just the head)
- each apple makes it longer

### ways to die
- hit any wall
- bite urself like an ouroboros

## 🏗️ code structure (for nerds)

```
main.cpp
├── coord struct          # x,y positions 
├── snakegame class       # all the game logic shit
│   ├── move()           # make snake go places
│   ├── check_collision() # did u hit something
│   ├── check_food()     # did u eat something  
│   ├── place_food()     # put apple somewhere random
│   └── render()         # draw the grid with emojis
└── discord bot stuff     # dpp event handlers
    ├── slash commands   # /startsnake
    ├── button clicks    # arrow controls
    └── game loop        # updates every 2 secs
```

## 📋 commands

| what u type | what happens |
|-------------|--------------|
| `/startsnake` | starts snake game duh |

## 🎨 what stuff looks like

- 🐍 snake head
- 🟩 snake body  
- 🍎 food/apple
- ⬜ nothing

## 🤝 contributing (if u want)

1. fork it
2. make a branch (`git checkout -b feature/some-cool-shit`)
3. commit ur changes (`git commit -m 'added cool shit'`)
4. push that branch (`git push origin feature/some-cool-shit`)
5. make a pull request

## 📝 license

mit license - see [LICENSE](LICENSE) file if u care about legal stuff

## 🐛 known problems

- discord rate limits suck so updates are slow
- starting multiple games overwrites the old one whoops

## 🚀 maybe ill add later

- [ ] leaderboards (if i feel like it)
- [ ] different game modes 
- [ ] multiplayer snake fight
- [ ] bigger grids
- [ ] powerups and weird shit

## 📞 help

if something breaks just make an issue on github and maybe ill fix it

---

made with ❤️ and way too much caffeine ☕ 