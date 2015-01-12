/****************************************
  * Title : Bubble Shooter Game *
  * Author: Garcia, Noel and Nuevo, Jay Martin *
  ***************************************/

/* LIBRARIES */
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <graphics.h>
#include <dos.h>
#include <time.h>
#include <math.h>

/* GAME AREA BORDERS*/
#define GP1X 240
#define GP1Y 75
#define GP2X 630
#define GP2Y 462

/*CONSTANTS*/
#define RADIUS 19.5
#define BORDERCOLOR LIGHTCYAN
#define FONTCOLOR YELLOW
#define BTNFONTCOLOR WHITE
#define BUTTONCOLOR LIGHTRED
#define PENALTYDUE 240

/* MOUSE FUNCTIONS*/
union REGS in,out;
int callmouse(void);
int mousehide(void);
void mouseposi(int *xpos, int *ypos, int *click);
void setposi(int *xpos,int *ypos);
void restrictmouseptr(int x1,int y1,int x2,int y2);

int cursorInRange(int pt1x,int pt1y,int pt2x,int pt2y);

void gameTime(void);
void newGame(void);
void initialize(void);
void statusCheck(void);
void gamePenalty(void);
void gameLose(void);
void gameWin(void);
void bulletColors(void);
void genBullet(void);
void genBubLine(int num, float cpy);
void showBubbles(void);
void shoot(void);
void bubbleBump(double *movex, double *movey, int *hit);
void bubblePop(double x, double y);
void bubbleEraser(void);
void score(int pop);
void background(void);
void gameTitle(char notif[15]);
void gameGUI(void);
void help(void);
void music(double pitch, int time);

 /*  VARIABLES  */
int xpos,ypos,cl;

int bubctr;
int bulletctr;
int bubline;
int popctr=0;
int pop=0;
int colors[6],numcolors;
char mode[9];

double octave[8]={261.62,293.66,329.62,349.22,392,440,493.88,523.24};

struct bubble{
	float cpx;
	float cpy;
	int color;
} bubble[500],bullet[500];

struct pop{
	int index;
	int bubtype;
} bubpop[20];

int status=NULL;

time_t t;
long int startT,pauseT=0,elapsedT,elapsedPT,gameT,penaltyT;

int tpop,tscore;

/*-------------------------------------------------------*/

int main(void)
{
	int gd,gm,error;

	/* LAUNCH GRAPHICS MODE */
    detectgraph(&gd,&gm);
    initgraph(&gd,&gm,"");
	error = graphresult();
	if (error != grOk)
	{
		printf("Graphics error occurred");
		printf("Press any key to halt:");
		getch();     /* wait for a key */
		exit(1);
    }

	/* GAME START-UP */
	gameTitle("Welcome !");

	while(1)
	{
		mouseposi(&xpos,&ypos,&cl);

        /* GAME TIME */
		gameTime();
		if ((gameT%PENALTYDUE==0) && gameT!=NULL && status==0)
		{
			gamePenalty();
			delay(400);
			statusCheck();
		}

		if (cl==2 && cursorInRange(GP1X,GP1Y-25,GP2X,GP2Y))
			restrictmouseptr(0,0,639,479);

		/* NEW GAME BUTTON CLICK */
		if (cl==1 && cursorInRange(45,32,195,70))
		{
            music(octave[0],50);
			music(octave[7],50);
			newGame();
			continue;
		}
		/* EXIT BUTTON CLICK */
		else if (cl==1 && cursorInRange(45,85,195,123))
		{
            music(octave[0],50);
			music(octave[7],50);
			break;
		}
		/* SHOOT */
		else if (status==NULL && status==0 && cl==1 && cursorInRange(GP1X,GP1Y,GP2X,GP2Y))
		{
			restrictmouseptr(GP1X,GP1Y-25,GP2X,GP2Y-RADIUS*2);
			shoot();
		}

		/* HELP */
		else if (cl==1 && status!=1 && cursorInRange(GP1X+75,GP1Y-20, GP1X+75+65,GP1Y-5))
		{
			music(octave[0],50);
			music(octave[7],50);
			mousehide();
			help();
			status=1;
			pauseT=t;
			restrictmouseptr(0,0,639,479);
			callmouse();
		}

		/* PAUSE */
		else if (status==NULL && status==0 && cl==1 && cursorInRange(GP1X+160,GP1Y-20, GP1X+160+65,GP1Y-5))
		{
			music(octave[0],50);
			music(octave[7],50);
			status=2;
			pauseT=t;
			restrictmouseptr(0,0,639,479);

			mousehide();
			setviewport(GP1X+1,GP1Y+1,GP2X-1,GP2Y-1,1);

			setcolor(BORDERCOLOR);
            setfillstyle(1,BLACK);
			bar(120,190,265,250);
			rectangle(120,190,265,250);

			setcolor(FONTCOLOR);
			settextjustify(CENTER_TEXT,TOP_TEXT);
			settextstyle(SMALL_FONT,HORIZ_DIR,8);
			outtextxy(195,207,"PAUSED");

			callmouse();
		}

		/* RESUME */
		else if(status!=0 && status!=-1 && cl==1 && cursorInRange(GP1X+245,GP1Y-20, GP1X+245+65,GP1Y-5))
		{
			music(octave[0],50);
			music(octave[7],50);
			status=0;
			elapsedPT += elapsedT;
			restrictmouseptr(GP1X,GP1Y-25,GP2X,GP2Y);
			mousehide();
			clearviewport();
			showBubbles();
			callmouse();
		}
		elapsedT = t-pauseT;
	}

	/* CLOSE GAME */
	mousehide();
	cleardevice();
	gameTitle("Thanks for Playing !");

	cleardevice();
	closegraph();
	return 0;
}

int cursorInRange(int pt1x, int pt1y, int pt2x, int pt2y )
{
	if (xpos>=pt1x && ypos>=pt1y && xpos<=pt2x && ypos<=pt2y)
		return 1;
	else
		return 0;
}

void gameTime(void)
{
	time(&t);
	if(startT!=NULL && status==0)
	{
		gameT = (t-startT-elapsedPT);
		gotoxy(6,21);
		printf("Game Time: %02ld",gameT/60);
		gotoxy(19,21);
		printf(":%02ld    ",gameT%60);
	}
	
}

void newGame(void)
{
	int i,j=0,numlines;
	float cpy=RADIUS;
	flushall();

	/*INITIALIZE*/
    gotoxy(6,21);
	printf("Game Time: 00:00 ");
	sprintf(mode,"------");
	pop=0,tpop=0;
	tscore=0;
	bubctr=0,bulletctr=0,bubline=0;
	score(0);
	bulletctr--;

	setviewport(GP1X+1,GP1Y+1,GP2X-1,GP2Y-1,1);
	clearviewport();
	background();

	/* GAME MODE */
	setcolor(FONTCOLOR);
	settextjustify(CENTER_TEXT,TOP_TEXT);
	settextstyle(SMALL_FONT,HORIZ_DIR,9);
	outtextxy(205,50,"SELECT DIFFICULTY: ");

	setcolor(BORDERCOLOR);
    setfillstyle(1,BLACK);
	bar(120,120,265,180);        /* EASY */
	rectangle(120,120,265,180);
	bar(120,190,265,250);        /*MEDIUM*/
	rectangle(120,190,265,250);
	bar(120,260,265,320);
	rectangle(120,260,265,320);  /* HARD */

	setcolor(FONTCOLOR);
	settextstyle(SMALL_FONT,HORIZ_DIR,8);
	outtextxy(195,137,"EASY");
	outtextxy(195,207,"MEDIUM");
	outtextxy(195,277,"HARD");

	while(1)
	{
		mouseposi(&xpos,&ypos,&cl);
		if (cursorInRange(GP1X+120,GP1Y+120,GP1X+265,GP1Y+180) && cl==1)
		{
			numlines=3;
			numcolors=4;
			sprintf(mode,"EASY");
			break;
		}
		else if (cursorInRange(GP1X+120,GP1Y+190,GP1X+265,GP1Y+250) && cl==1)
			{
			numlines=4;
			numcolors=5;
			sprintf(mode,"MEDIUM");
			break;
		}
		else if (cursorInRange(GP1X+120,GP1Y+260,GP1X+265,GP1Y+320) && cl==1)
		{
			numlines=6;
			numcolors=7;
			sprintf(mode,"HARD");
			break;
		}
		if (cl==1 && cursorInRange(45,85,195,123))
			return;
	}
	music(octave[0],50);
	music(octave[7],50);	
	
	/*INITIALIZE TIME*/
	srand((unsigned)time(&t));
	elapsedT=1;
	elapsedPT=0;
	startT=t+1;
	status=0;
	pauseT=0;

	/*AVAILABLE COLORS */
	for (i=9; i<9+numcolors; i++)
		colors[j++]=i;

	/* GEN NUM OF LINES */
	for(i=0; i<numlines; i++)
	{
	   genBubLine(9+(i+1)%2,cpy);
       cpy += RADIUS+(RADIUS*0.75);
	}

	showBubbles();
	genBullet();
	score(0);
	restrictmouseptr(GP1X,GP1Y-25,GP2X,GP2Y-RADIUS*2);
}

void statusCheck(void)
{
	int i,test=0;

	for(i=0; i<bubctr; i++)
		test = test || bubble[i].color;

	if (test==0 && status==0)
		gameWin();
	else if (test && status==0)
	{
		for (i=0; i<360; i++)
			if(getpixel((GP2X-GP1X)/2+(RADIUS*cos(i))+cos(i), (GP2Y-GP1Y)-RADIUS-2+(RADIUS*sin(i))+sin(i)) != BLACK)
			{
				gameLose();
				return;
			}

		for(i=1; i<388; i++)
			if(getpixel(i, 385) != BLACK)
			{
				gameLose();
				return;
			}
	}
}

void gamePenalty(void)
{
	int i;

	if (penaltyT==t)
		return;
	for(i=0; i<bubctr; i++)
		bubble[i].cpy += RADIUS + 0.75*RADIUS;
	for(i=0; i<bulletctr; i++)
		bullet[i].cpy += RADIUS + 0.75*RADIUS;

	setviewport(GP1X+1,GP1Y+1,GP2X-1,GP2Y-1,1);
	clearviewport();

	genBubLine(9+(bubline%2),RADIUS);
	bubline++;
	
	showBubbles();
	gameTime();
	penaltyT=t;
	music(octave[3],125);
	music(octave[2],125);
	music(220,250);
}

void gameLose(void)
{
	pauseT=t;
	status=-2;
	mousehide();
	setviewport(GP1X+1,GP1Y+1,GP2X-1,GP2Y-1,1);
	clearviewport();
	background();

	setcolor(FONTCOLOR);
	settextstyle(SMALL_FONT,HORIZ_DIR,10);
	settextjustify(CENTER_TEXT,TOP_TEXT);
	line(RADIUS,RADIUS*4+40,(GP2X-GP1X)-RADIUS,RADIUS*4+40);
	outtextxy((GP2X-GP1X)/2,RADIUS+120,"GAME OVER!");
	line(RADIUS,RADIUS*4+120,(GP2X-GP1X)-RADIUS,RADIUS*4+120);
	callmouse();
	restrictmouseptr(0,0,639,479);
	
	music(octave[5],500);
	music(octave[2],500);
	music(octave[0],500);
	music(247,500);
	music(octave[0],500);
}

void gameWin(void)
{
	int i;
	pauseT=t;
	status=-1;
	setviewport(GP1X+1,GP1Y+1,GP2X-1,GP2Y-1,1);
	clearviewport();
	mousehide();
	background();
	setcolor(FONTCOLOR);
	settextstyle(SMALL_FONT,HORIZ_DIR,10);
	settextjustify(CENTER_TEXT,TOP_TEXT);
	outtextxy((GP2X-GP1X)/2,RADIUS+100,"CONGRATULATIONS!");

	for (i=8; i<16; i++)
	{
		setcolor(i);
		setfillstyle(1,i);
		circle(50+(i-8)*40,RADIUS+165,20);
		floodfill(50+(i-8)*40,RADIUS+165,i);
	}

	music(octave[0],800);
	music(octave[4],800);
	
	music(octave[3],250);
    music(octave[2],250);
    music(octave[1],250);
    music(octave[7],750);
    music(octave[4],750);

    music(octave[3],250);
    music(octave[2],250);
    music(octave[1],250);
    music(octave[7],750);
    music(octave[4],750);

    music(octave[3],250);
    music(octave[2],250);
    music(octave[3],250);
    music(octave[1],1000);
	
	callmouse();
	restrictmouseptr(0,0,639,479);
}

void bulletColors(void)
{
	int i,j,k;

	for (i=0; i<numcolors; i++)
	{
		if (colors[i]==0)
			continue;
		for(j=0; j<bubctr; j++)
			if (colors[i]==bubble[j].color)
				break;
		for(k=0; k<bulletctr; k++)
			if (colors[i]==bullet[k].color)
				break;
		if (j==bubctr && k==bulletctr)
			colors[i]=0;
	}
}

void genBullet(void)
{
	int color;
	srand((unsigned)time(&t));
	bulletctr++;
    setviewport(GP1X+1,GP1Y+1,GP2X-1,GP2Y-1,1);

	color = colors[random(numcolors)];
	while (color==0)
		color = colors[random(numcolors)];

	setcolor(color);
	setfillstyle(1,color);

	bullet[bulletctr].cpx = (GP2X-GP1X)/2;
	bullet[bulletctr].cpy = (GP2Y-GP1Y)-RADIUS-2;
	bullet[bulletctr].color = color;

	circle(bullet[bulletctr].cpx, bullet[bulletctr].cpy, RADIUS);
	floodfill(bullet[bulletctr].cpx, bullet[bulletctr].cpy, bullet[bulletctr].color);
}

void genBubLine(int num, float cpy)
{
	int i, color;
	float cpx;

	if (num==9)
		cpx = RADIUS*2;
	else
		cpx = RADIUS;

	for (i=0; i<num; i++)
	{
		color = colors[random(numcolors)];
		while(color==0)
			color = colors[random(numcolors)];
		setcolor(color);
		setfillstyle(1,color);

		bubble[bubctr].cpx = cpx;
		bubble[bubctr].cpy = cpy;
		bubble[bubctr++].color = color;
		cpx += RADIUS*2;
	}
	bubble[bubctr].color= '/0';
}

void showBubbles(void)
{
	int i;
	setviewport(GP1X+1,GP1Y+1,GP2X-1,GP2Y-1,1);
	mousehide();
	clearviewport();

	/*SHOW TARGET BUBBLES */
	for(i=0; i<bubctr; i++)
	{
		if (bubble[i].color==0)
			continue;
		setcolor(bubble[i].color);
		setfillstyle(1,bubble[i].color);
		circle(bubble[i].cpx, bubble[i].cpy, RADIUS);
		floodfill(bubble[i].cpx, bubble[i].cpy, bubble[i].color);
	}
	/*SHOW BULLETS */
	for(i=0; i<bulletctr+1; i++)
	{
		if (bullet[i].color==0)
			continue;
		setcolor(bullet[i].color);
		setfillstyle(1,bullet[i].color);
		circle(bullet[i].cpx, bullet[i].cpy, RADIUS);
		floodfill(bullet[i].cpx, bullet[i].cpy, bullet[i].color);
	}
	callmouse();
}

void shoot(void)
{
	double movex = (435-xpos)/15;
	double movey = (440.5-ypos)/15;
	int hit=0;
	popctr=0,pop=0;

	setviewport(GP1X+1,GP1Y+1,GP2X-1,GP2Y-1,1);
	mousehide();
	music(octave[7],50);
	while(hit==0)
	{
		/* GAMETIME and PENALTYCHECK */
		gameTime();
		if ((gameT%PENALTYDUE==0) && gameT!=NULL && status==0)
		{
			gamePenalty();
			delay(400);
			statusCheck();
		}
	
		/* ERASE BUBBLE PREV LOCATION */
		setfillstyle(1,0);
		floodfill(bullet[bulletctr].cpx, bullet[bulletctr].cpy, BLACK);

		statusCheck();

		/*MOVE TO NEW LOCATION*/
		bubbleBump(&movex,&movey,&hit);
		bullet[bulletctr].cpx-=movex;
		bullet[bulletctr].cpy-=movey;

		/* DISPLAY BUBBLE */
		setcolor(bullet[bulletctr].color);
		setfillstyle(1,bullet[bulletctr].color);
		circle(bullet[bulletctr].cpx, bullet[bulletctr].cpy, RADIUS);
		floodfill(bullet[bulletctr].cpx, bullet[bulletctr].cpy, bullet[bulletctr].color);
		
		music(octave[0],50);
	}

	/* POP BUBBLES*/
	bubblePop(bullet[bulletctr].cpx,bullet[bulletctr].cpy);
	bubbleEraser();

	statusCheck();

	callmouse();

	if (status<0)
		return;

	bulletColors();
	genBullet();

	/*SCORING*/
	if (pop>=3)
		score(pop);
	else
		score(0);
}

void bubbleBump(double *movex, double *movey, int *hit)
{
	int i;

	if(bullet[bulletctr].cpx-*movex+GP1X<=GP1X+RADIUS || bullet[bulletctr].cpx-*movex+GP1X>=GP2X-RADIUS)
		*movex = -*movex;

	for (i=90; i>0;)
	{
		if(getpixel(bullet[bulletctr].cpx-*movex+(RADIUS*cos(i))+cos(i), bullet[bulletctr].cpy-*movey-(RADIUS*sin(i))+sin(i)))
		{
			*hit =1;
			*movex+=cos(i);
			*movey-=sin(i);
		}
		if(getpixel(bullet[bulletctr].cpx-*movex+RADIUS*cos(180-i)+cos(180-i), bullet[bulletctr].cpy-*movey-RADIUS*sin(180-i)+sin(180-i)))
		{
			*hit =1;
			*movex+=cos(180-i);
			*movey-=sin(180-i);
		}
		if(getpixel(bullet[bulletctr].cpx-*movex+(RADIUS*cos(360-i))+cos(360-i), bullet[bulletctr].cpy-*movey-(RADIUS*sin(360-i))+sin(360-i)))
		{
			*hit =1;
			*movex+=cos(360-i);
			*movey-=sin(360-i);
		}
		if(getpixel(bullet[bulletctr].cpx-*movex+RADIUS*cos(270+i)+cos(270+i), bullet[bulletctr].cpy-*movey-RADIUS*sin(270+i)+sin(270+i)))
		{
			*hit =1;
			*movex+=cos(270+i);
			*movey-=sin(270+i);
		}
		if (getpixel(bullet[bulletctr].cpx-*movex+RADIUS*cos(i)    +cos(i),     bullet[bulletctr].cpy-*movey-RADIUS*sin(i)+sin(i)) ==0 &&
			getpixel(bullet[bulletctr].cpx-*movex+RADIUS*cos(180-i)+cos(180-i), bullet[bulletctr].cpy-*movey-RADIUS*sin(180-i)+sin(180-i))==0 &&
			getpixel(bullet[bulletctr].cpx-*movex+RADIUS*cos(360-i)+cos(360-i), bullet[bulletctr].cpy-*movey-RADIUS*sin(360-i)+sin(360-i))==0 &&
			getpixel(bullet[bulletctr].cpx-*movex+RADIUS*cos(270+i)+cos(270+i), bullet[bulletctr].cpy-*movey-RADIUS*sin(270+i)+sin(270+i))==0)
			i--;

	}
}

void bubblePop(double x, double y)
{
	int i,j,k,repeat=0;
	double posx,posy;

	for (i=0; i<360; i++)
	{
		posx=x+RADIUS*cos(i)+2*cos(i);
		posy=y+RADIUS*sin(i)+2*sin(i);

		if(getpixel(posx,posy) == bullet[bulletctr].color)
		{
			for(j=0; j<bubctr; j++)
				if (posx>bubble[j].cpx - RADIUS &&
					posx<bubble[j].cpx + RADIUS &&
					posy>bubble[j].cpy - RADIUS &&
					posy<bubble[j].cpy + RADIUS &&
					bubble[j].color == bullet[bulletctr].color)
				{
					for(k=0; k<popctr; k++)
						if (bubpop[k].index==j)
							repeat=1;
					if (repeat)
					{
						repeat=0;
						break;
					}
					bubpop[popctr].index=j;
					bubpop[popctr++].bubtype=1;
					pop++;
					bubblePop(bubble[j].cpx,bubble[j].cpy);
				}

			for(j=0; j<bulletctr; j++)
				if (posx>bullet[j].cpx - RADIUS &&
					posx<bullet[j].cpx + RADIUS &&
					posy>bullet[j].cpy - RADIUS &&
					posy<bullet[j].cpy + RADIUS &&
					bullet[j].color == bullet[bulletctr].color)
				{
					for(k=0; k<popctr; k++)
						if (bubpop[k].index==j)
							repeat=1;
					if (repeat)
					{
						repeat=0;
						break;
					}
					bubpop[popctr].index=j;
					bubpop[popctr++].bubtype=2;
					pop++;
					bubblePop(bullet[j].cpx,bullet[j].cpy);
				}
		}

	}
	return;
}

void bubbleEraser(void)
{
	int i;
	pop++;

	if (pop<3)
		return;
	
	setviewport(GP1X+1,GP1Y+1,GP2X-1,GP2Y-1,1);
	gotoxy(6,18);
	printf("CLEARING...");
	
	bullet[bulletctr].color=0;
	for (i=0; i<popctr; i++)
	{
		if (bubpop[i].bubtype ==1)
			bubble[bubpop[i].index].color=0;
		else if (bubpop[i].bubtype == 2)
			bullet[bubpop[i].index].color=0;
	}
	
	music(octave[0],90);
	music(octave[4],90);
	music(octave[7],500);
	
	clearviewport();
	showBubbles();

	gotoxy(6,18);
	printf("           ");
}

void score(int pop)
{
	setviewport(0,0,639,479,1);
	settextstyle(2,HORIZ_DIR,9);
	settextjustify(CENTER_TEXT,TOP_TEXT);
	setcolor(FONTCOLOR);
	outtextxy(120,140,"SCORE");
	gotoxy(6,12);
	printf("DIFFICULTY: %s   ",mode);
	gotoxy(6,14);
	printf("BUBBLES SHOT:   %d  ",bulletctr);
	gotoxy(6,15);
	printf("BUBBLES POPPED: %d  ",pop);
	gotoxy(6,16);
	printf("TOTAL # POPPED: %d  ",tpop+=pop);
}

void background(void)
{
	int i;
	for(i=0; i<500; i++)
		putpixel(random(640), random(480),9+random(6));
}

void gameTitle(char notif[15])
{
	int i;
	setviewport(0,0,639,479,1);

	setbkcolor(BLUE);
	background();
	settextjustify(CENTER_TEXT,TOP_TEXT);
	settextstyle(SMALL_FONT, HORIZ_DIR, 10);
	setcolor(FONTCOLOR);
	outtextxy(320,160,"BUBBLE SHOOTER");

	setcolor(WHITE);
	settextstyle(SMALL_FONT,HORIZ_DIR,5);
	outtextxy(320,450,"(c) Garcia and Nuevo - 2014");
	
	delay(100);
	/* LOADING... */
	for (i=2; i<16; i++)
	{
		setcolor(i);
		setfillstyle(1,i);
		circle(20+((i-1)*40),225,20);
		floodfill(20+1+((i-1)*40),225+1,i);
		delay(100);
	}
	
	setcolor(WHITE);
	settextstyle(SMALL_FONT, HORIZ_DIR, 7);
	outtextxy(320,260,notif);
	
	
	music(octave[0],400);
	music(octave[4],400);
	
	music(octave[3],125);
    music(octave[2],125);
    music(octave[1],125);
    music(octave[7],375);
    music(octave[4],375);

    music(octave[3],125);
    music(octave[2],125);
    music(octave[1],125);
    music(octave[7],375);
    music(octave[4],375);

    music(octave[3],125);
    music(octave[2],125);
    music(octave[3],125);
    music(octave[1],500);
	
	
	cleardevice();
	gameGUI();
	setposi(0,0);
	callmouse();
	setviewport(GP1X+1,GP1Y+1,GP2X-1,GP2Y-1,1);
}

void gameGUI(void)
{
	background();
	
	setcolor(BORDERCOLOR);
	/*WINDOWS */
	rectangle(10,365,230,380);   /* created by */
	rectangle(10,380,230,GP2Y);  /* authors   */
	rectangle(10,10 ,230,355);   /* control  */
	rectangle(30,140,210,340);   /* score   */

	setfillstyle(1,BUTTONCOLOR);
	/*BUTTONS*/
	bar(45,32, 195,70);   /* newgame */
	bar(45,85, 195,123);  /*  exit  */

	/*BUTTONS TEXT*/
	setcolor(BTNFONTCOLOR);
	settextstyle(DEFAULT_FONT, HORIZ_DIR, 2);
	settextjustify(CENTER_TEXT,TOP_TEXT);
    outtextxy(120,45,"NEW GAME");
	outtextxy(120,98,"EXIT");

	/* AUTHORS */
	settextstyle(SMALL_FONT,HORIZ_DIR,5);
	settextjustify(LEFT_TEXT,TOP_TEXT);
	outtextxy(78,367, "CREATED BY");
	outtextxy(40,385, "Garcia, Noel A.");
	outtextxy(40,400, "Nuevo, Jay Martin");

	/* GAME TITLE */
	setcolor(FONTCOLOR);
	settextstyle(2,HORIZ_DIR,8);
	settextjustify(CENTER_TEXT,TOP_TEXT);
	outtextxy(435,18,"BUBBLE SHOOTER");

	/*GAME BOX*/
	setcolor(BORDERCOLOR);
    rectangle(GP1X,10,GP2X,50);
	rectangle(GP1X,GP1Y,GP2X,GP2Y);

	/*GAME BUTTONS */
	settextstyle(SMALL_FONT,HORIZ_DIR,5);
	settextjustify(CENTER_TEXT,TOP_TEXT);
	setfillstyle(1, BUTTONCOLOR);
	rectangle(GP1X,50,GP2X,75);

	setcolor(BTNFONTCOLOR);
	/*HELP BUTTON*/
	bar(GP1X+75,GP1Y-20, GP1X+75+65,GP1Y-5);
	outtextxy(GP1X+75+32,GP1Y-19,"HELP");

	/*PAUSE BUTTON*/
	bar(GP1X+160,GP1Y-20, GP1X+160+65,GP1Y-5);
	outtextxy(GP1X+160+32,GP1Y-19,"PAUSE");

	/*RESUME BUTTON*/
	bar(GP1X+245,GP1Y-20, GP1X+245+65,GP1Y-5);
	outtextxy(GP1X+245+32,GP1Y-19,"RESUME");
}

void help(void)
{
	int i;
	setviewport(GP1X+1,GP1Y+1,GP2X-1,GP2Y-1,1);
	clearviewport();
	background();
	for (i=2; i<7; i++)
	{
		setcolor(i);
		setfillstyle(1,i);
		circle((i-1)*20,RADIUS+10,10);
		floodfill((i-1)*20,RADIUS+10,i);
	}

	setcolor(FONTCOLOR);
	settextstyle(SMALL_FONT,HORIZ_DIR,7);
	settextjustify(CENTER_TEXT,TOP_TEXT);
	outtextxy((GP2X-GP1X)/2,RADIUS,"HOW TO PLAY");

	for (i=11; i<16; i++)
	{
		setcolor(i);
		setfillstyle(1,i);
		circle(85+(i-1)*20,RADIUS+10,10);
		floodfill(85+1+(i-1)*20,RADIUS+10,i);
	}

	setcolor(WHITE);
	settextjustify(LEFT_TEXT,TOP_TEXT);
	settextstyle(SMALL_FONT,HORIZ_DIR,5);
	outtextxy(38,60,"The goal of this game is to clear the board");
	outtextxy(20,72,"from the colorful bubbles. Shoot the bubbles");
	outtextxy(20,84,"that appear at the bottom of the game window");
	outtextxy(20,96,"by pointing the cursor to the desired target.");
	outtextxy(20,108,"When three or more bubbles of the same color");
	outtextxy(20,120,"come together, they will disappear.");
	outtextxy(38,132,"If you are still unable to finish the game");
	outtextxy(20,144,"within a certain period, there will be a penalty.");

    for (i=2; i<7; i++)
	{
		setcolor(i);
		setfillstyle(1,i);
		circle((i-1)*20,(GP2Y-GP1Y)/2,10);
		floodfill((i-1)*20,(GP2Y-GP1Y)/2,i);
	}

	setcolor(FONTCOLOR);
	settextstyle(SMALL_FONT,HORIZ_DIR,7);
	settextjustify(CENTER_TEXT,TOP_TEXT);
	outtextxy((GP2X-GP1X)/2,(GP2Y-GP1Y)/2 -10,"CONTROLS");

	for (i=11; i<16; i++)
	{
		setcolor(i);
		setfillstyle(1,i);
		circle(85+(i-1)*20,(GP2Y-GP1Y)/2,10);
		floodfill(85+1+(i-1)*20,(GP2Y-GP1Y)/2,i);
	}

    setcolor(WHITE);
	settextjustify(LEFT_TEXT,TOP_TEXT);
	settextstyle(SMALL_FONT,HORIZ_DIR,5);
	outtextxy(20,235,"CONTROLLER: Mouse");
	outtextxy(20,247,"Left Click  (Button 1) - Shoot");
	outtextxy(20,259,"Right Click (Button 2) - Escape mouse pointer");
	outtextxy(20,271,"                       restriction");
}

void music(double pitch, int time)
{
	sound(pitch);
	delay(time);
	nosound();
}

  /*------------------------*/
 /*     MOUSE FUNCTIONS    */
/*------------------------*/
int callmouse(void)
{
     in.x.ax=1;
     int86(51,&in,&out);
     return 1;
}

void mouseposi(int *xpos, int *ypos, int *clickick)
{
      in.x.ax=3;
      int86(51,&in,&out);
	  *clickick=out.x.bx;
 	  *xpos=out.x.cx;
	  *ypos=out.x.dx;
}

int mousehide(void)
{
      in.x.ax=2;
      int86(51,&in,&out);
      return 1;
}

void setposi(int *xpos,int *ypos)
{
     in.x.ax=4;
	 in.x.cx=*xpos;
	 in.x.dx=*ypos;
     int86(51,&in,&out);
}

void restrictmouseptr(int x1,int y1,int x2,int y2)
{
     in.x.ax=7;
     in.x.cx=x1;
     in.x.dx=x2;
     int86(51,&in,&out);
     in.x.ax=8;
     in.x.cx=y1;
     in.x.dx=y2;
     int86(51,&in,&out);
}