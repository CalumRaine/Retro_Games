#include <stdlib.h>
#include <ncurses.h>
#include <time.h>

struct snakePos{
	int x;	
	int y;	
};

int main(){	
	srand(time(NULL));		// initialise random number generator
	initscr();			// start curses mode 		  
	cbreak();			// make input characters immediately available (no buffering)
	nodelay(stdscr,1);		// do not stall and wait for user input
	noecho();			// do not print input characters to the screen
	keypad(stdscr,1);		// make the arrow keys work
	curs_set(FALSE);		// invisible cursor
	attron(A_BOLD);			// make all characters bold
	
	// INITIALISE COLOUR
	start_color();
	init_pair(6,COLOR_YELLOW,COLOR_RED);		// this one is used for title bar and "Game Over" snake
	init_pair(1,COLOR_GREEN,COLOR_GREEN);		// others used as food/snake colours
	init_pair(2,COLOR_BLUE,COLOR_BLUE);
	init_pair(3,COLOR_MAGENTA,COLOR_MAGENTA);
	init_pair(4,COLOR_CYAN,COLOR_CYAN);
	init_pair(5,COLOR_YELLOW,COLOR_YELLOW);
	int snakeColour=1, foodColour=2;

	// FIND SCREEN BOUNDARIES
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

	// INITIALISE TITLE BAR
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
		if (nextArrow!=ERR) latestArrow=nextArrow; 						// update input only when user presses key
		if (latestArrow == KEY_UP && xDirection) yDirection=-1, xDirection=0;
		else if (latestArrow == KEY_DOWN && xDirection) yDirection=1, xDirection=0;
		else if (latestArrow == KEY_RIGHT && yDirection) yDirection=0, xDirection=1;
		else if (latestArrow == KEY_LEFT && yDirection) yDirection=0, xDirection=-1;
		else if (latestArrow == 'c') goto end;

		// FIND NEXT POSITION
		int newX=snakeStack[headPos].x + xDirection;
		int newY=snakeStack[headPos].y + yDirection;

		// CHECK SCREEN BOUNDARIES
		if (newX<0) newX=maxX;
		else if (newX==maxX) newX=0;
		if (newY==0) newY=maxY;
		else if (newY==maxY) newY=1;

		// CHECK IF EATEN FOOD
		if (newX==food.x && newY==food.y){
			// add new entry to snake stack
			for (int segment=snakeLength; segment>headPos+1; --segment) snakeStack[segment]=snakeStack[segment-1]; 
			++snakeLength;

			// add new segment to snake
			++headPos;			
			snakeStack[headPos].x=food.x;
			snakeStack[headPos].y=food.y;

			// generate new food position
			food.x=rand()%maxX;
			food.y=(rand()%(maxY-1))+1;
			snakeColour=foodColour;					// snake adopts food colour
			foodColour=(foodColour%5)+1;				// new food colour
			mvaddch(food.y,food.x,' ' | COLOR_PAIR(foodColour)); 	// draw new food

			// check if level needs to increase
			if ( (++foodCounter%5)==0 ) delay-=1000000, ++level;	// make faster

			// update title bar with new score
			move(0,0);
			attron(COLOR_PAIR(6));
			for (int i=0; i<maxX; ++i) addch(' ');
			mvprintw(0,0,"SNAKE\tLevel %i\tScore %i",level,foodCounter);
			attroff(COLOR_PAIR(6));
		}
		else { // OTHERWISE CLEAR TAIL POS AND ADD NEW HEAD POS
			headPos=(headPos+1)%snakeLength; 
			mvaddch(snakeStack[headPos].y,snakeStack[headPos].x,' ' | COLOR_PAIR(0)); 
			snakeStack[headPos].x=newX;
			snakeStack[headPos].y=newY;
			mvaddch(snakeStack[headPos].y,snakeStack[headPos].x,' ' | COLOR_PAIR(snakeColour));
			// CHECK FOR COLLISION
			for (int segment=0, check=(headPos+1)%snakeLength; segment<snakeLength-1; ++segment, check=(check+1)%snakeLength){
				if (snakeStack[check].x==snakeStack[headPos].x && snakeStack[check].y==snakeStack[headPos].y) goto end;
			}
		}
		refresh();
		for (int i=0; i<delay; ++i); // this should be updated to a true delay function, otherwise very processor-dependent
	}

	// COLLISION DETECTED: paint snake red and pause
	end:
	for (int segment=0; segment<snakeLength; ++segment) mvaddch(snakeStack[segment].y,snakeStack[segment].x,' ' | COLOR_PAIR(6));
	refresh();
	while(1);			// bad practice, program never finishes
	endwin();			// End curses mode		  
	return 0;
}
