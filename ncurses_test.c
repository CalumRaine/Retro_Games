#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <math.h>

struct snakePos{
	int x;	
	int y;	
};

int main(){	
	srand(0);
	initscr();			// Start curses mode 		  
	cbreak();			// make input characters immediately available (no buffering)
	nodelay(stdscr,1);
	noecho();			// do not print input characters to the screen
	keypad(stdscr,1);		// make the arrow keys work
	curs_set(0);
	attron(A_BOLD);
	
	// INITIALISE COLOUR
	start_color();
	init_pair(6,COLOR_YELLOW,COLOR_RED);
	init_pair(1,COLOR_GREEN,COLOR_GREEN);
	init_pair(2,COLOR_BLUE,COLOR_BLUE);
	init_pair(3,COLOR_MAGENTA,COLOR_MAGENTA);
	init_pair(4,COLOR_CYAN,COLOR_CYAN);
	init_pair(5,COLOR_YELLOW,COLOR_YELLOW);
	int snakeColour=(rand()%5)+1, foodColour=(rand()%5)+1;

	int maxX=0, maxY=0;
	getmaxyx(stdscr,maxY,maxX);

	// INITIALISE SNAKE
	struct snakePos snakeStack[1000];
	int snakeLength=10, maxLength=1000, headPos=9;
	for (int i=0, x=(maxX/2)-2, y=(maxY/2); i<snakeLength; ++i, ++x){
		snakeStack[i].y=y;
		snakeStack[i].x=x;
		mvaddch(y,x, (chtype)' ' | COLOR_PAIR(snakeColour));
	}

	// INITIALISE FOOD
	struct snakePos food;
	food.x=rand()%maxX;
	food.y=(rand()%(maxY-1))+1;
	mvaddch(food.y,food.x,' ' | COLOR_PAIR(foodColour));

	// INITIALISE SCORES
	int foodCounter=0, level=1;
	move(0,0);
	attron(COLOR_PAIR(6));
	for (int i=0; i<maxX; ++i) addch(' ');
	mvprintw(0,0,"SNAKE\tLevel %i\tScore %i",level,foodCounter);
	attroff(COLOR_PAIR(6));

	// WAIT FOR USER TO START GAME
	refresh();
	int latestArrow;
	while ( (latestArrow=getch()) == ERR );

	for (int yDirection=0, xDirection=1, nextArrow=ERR, delay=10000000; 1 ; nextArrow=getch()){
		if (nextArrow!=ERR) latestArrow=nextArrow;
		if (latestArrow == KEY_UP && xDirection) yDirection=-1, xDirection=0;
		else if (latestArrow == KEY_DOWN && xDirection) yDirection=1, xDirection=0;
		else if (latestArrow == KEY_RIGHT && yDirection) yDirection=0, xDirection=1;
		else if (latestArrow == KEY_LEFT && yDirection) yDirection=0, xDirection=-1;
		else if (latestArrow == 'c') goto end;

		int newX=snakeStack[headPos].x + xDirection;
		int newY=snakeStack[headPos].y + yDirection;
		if (newX<0) newX=maxX;
		else if (newX==maxX) newX=0;
		if (newY==0) newY=maxY;
		else if (newY==maxY) newY=1;
		if (newX==food.x && newY==food.y){
			for (int segment=snakeLength; segment>headPos+1; --segment) snakeStack[segment]=snakeStack[segment-1];
			++headPos;
			snakeStack[headPos].x=food.x;
			snakeStack[headPos].y=food.y;
			++snakeLength;
			food.x=rand()%maxX;
			food.y=(rand()%(maxY-1))+1;
			snakeColour=foodColour;
			foodColour=(foodColour%5)+1;
			mvaddch(food.y,food.x,' ' | COLOR_PAIR(foodColour));
			if ( (++foodCounter%5)==0 ) {
				delay-=1000000;
				++level;
			}
			move(0,0);
			attron(COLOR_PAIR(6));
			for (int i=0; i<maxX; ++i) addch(' ');
			mvprintw(0,0,"SNAKE\tLevel %i\tScore %i",level,foodCounter);
			attroff(COLOR_PAIR(6));
		}
		else {
			headPos=(headPos+1)%snakeLength;
			mvaddch(snakeStack[headPos].y,snakeStack[headPos].x,' ' | COLOR_PAIR(0)); 
			snakeStack[headPos].x=newX;
			snakeStack[headPos].y=newY;
			mvaddch(snakeStack[headPos].y,snakeStack[headPos].x,' ' | COLOR_PAIR(snakeColour));
			for (int segment=0, check=(headPos+1)%snakeLength; segment<snakeLength-1; ++segment, check=(check+1)%snakeLength){
				if (snakeStack[check].x==snakeStack[headPos].x && snakeStack[check].y==snakeStack[headPos].y) goto end;
			}
		}
		refresh();
		for (int i=0; i<delay; ++i);
	}

	end:
	for (int segment=0; segment<snakeLength; ++segment) mvaddch(snakeStack[segment].y,snakeStack[segment].x,' ' | COLOR_PAIR(6));
	refresh();
	while(1);
	endwin();			// End curses mode		  
	return 0;
}
