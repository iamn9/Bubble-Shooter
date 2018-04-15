Bubble Shooter
======
![Gameplay Demo](https://user-images.githubusercontent.com/22837457/38773898-eb4cf5f4-408a-11e8-86d4-df03b3ccfd14.gif)

Authors
-----
- Garcia, Noel A.
- Nuevo, Jay Martin

Rules of the Game:
-----
- The aim of the game is to clear the board from the colorful bubbles.
- You can shoot the bubble that appears at the bottom of the board.
- When three or more bubbles of the same color come together, they will all disappear
- If your shot fails to detonate any bubbles, you get a foul, and when there care several fouls, a new line of bubbles appears at the top.
- There is no time limit in this game. Either you finish the game by clearing all bubbles, or they move all the way down to you.
	
Controls
-----
- Left Click Mouse Button to Shoot bubble
- Right Click Mouse Button to escape from mouse restriction
		  
Functions
-----
- void gameTime(void); - updates and shows Game Time;
- void newGame(void); - resets or starts a newGame by clearing the game area and generating initial bubbles.
- void initialize(void); - reverts all values used back to the game default
- void statusCheck(void); - checks if the game is paused, game over, or finished.
- void gamePenalty(void); - gives penalty to the game by generating another line of bubbles within a defined period
- void gameLose(void); - displays game over
- void gameWin(void); - displays winner
- void bulletColors(void); - scans for available colors of bubbles left
- void genBullet(void); - generates bubbles for shooting
- void genBubLine(int num, float cpy); - generates one line of bubbles
- void showBubbles(void); - refreshes the game area and shows all the bubbles
- void shoot(void); - shoots the bullet to the pointed area and executes necessary actions after
- void bubbleBump(double *movex, double *movey, int *hit); - controls the movement of the bubble so that it would bump
- void bubblePop(double x, double y); - scans for similarly colored bubbles surrounding the bullet shot and tags them for erasement
- void bubbleEraser(void); - checks bubpop[] and erases all bubbles enlisted on it.
- void score(int pop); displays and updates scores by receiving the number of bubbles popped
- void background(void); - prints the starry background design on the screen
- void gameTitle(char notif[15]); - shows the game title
- void gameGUI(void); - shows all buttons and windows
- void help(void); - shows the instructions and controls in playing the game
- void music(double pitch, int time); - plays a note of a certain pitch and plays it within the specified time
		  
Legend of status(int) variable:
---
- 0 = in game
- 1 = help
- 2 = pause
- -1 = game win
- -2 = game lose
