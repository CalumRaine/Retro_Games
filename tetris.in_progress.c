#include <stdlib.h>
#include <ncurses.h>
#include <time.h>

enum shapeDimensions {WIDTH,HEIGHT};
const int shapeBitmaps[5][6]={	{1,1,1,1,0,0}, // LINE
				{1,1,1,1,0,0}, // CUBE
				{1,0,1,0,1,1}, // L_SHAPE
				{1,0,1,1,0,1}, // S_SHAPE
				{1,1,1,0,1,0}  // T_SHAPE
			};

const int shapeDimensions[5][3]={ 	{4,1}, // LINE
					{2,2}, // CUBE
					{2,3}, // L_SHAPE
					{2,3}, // S_SHAPE
					{3,2}, // T_SHAPE
				};

struct shapeStruct {
	int bitmap[6];
	int height;
	int width;
	int colour;
};

struct shapeStruct generateNewShape();
struct shapeStruct rotateClockwise(struct shapeStruct oldShape);
int drawShape(int xPos, int yPos, struct shapeStruct newShape, WINDOW *field);
void clearShape(int xPos, int yPos, struct shapeStruct newShape, WINDOW *field);
int main(){	
	srand(time(NULL));		// initialise random number generator
	initscr();			// start curses mode 		  
	cbreak();			// make input characters immediately available (no buffering)
	nodelay(stdscr,1);		// do not stall and wait for user input
	noecho();			// do not print input characters to the screen
	keypad(stdscr,1);		// make the arrow keys work
	curs_set(FALSE);		// invisible cursor
	
	// INITIALISE COLOUR
	start_color();
	init_pair(1,COLOR_CYAN,COLOR_CYAN);
	init_pair(2,COLOR_YELLOW,COLOR_YELLOW);
	init_pair(3,COLOR_RED, COLOR_RED);
	init_pair(4,COLOR_GREEN,COLOR_GREEN);
	init_pair(5,COLOR_MAGENTA, COLOR_MAGENTA);

	// FIND SCREEN BOUNDARIES
	int maxX=0, maxY=0;
	getmaxyx(stdscr,maxY,maxX);

	// CREATE PLAYING FIELD
	WINDOW *field=newwin(22,12,(maxY/2)-11,(maxX/2)-6);
	wborder(field, '|', '|', '-', '-', '+', '+', '+', '+');
	keypad(field,1);		// make the arrow keys work
	nodelay(field,1);		// do not stall and wait for user input

	struct shapeStruct newShape=generateNewShape();
	for (int delay=20000, input=ERR, xPos=(12+newShape.width)/4, yPos=1;  input!='c' ; --delay, input=wgetch(field)){
		clearShape(xPos, yPos, newShape,field);
		if (input==KEY_RIGHT && (xPos+newShape.width)<11) ++xPos;
		else if (input==KEY_LEFT && xPos>1) --xPos;
		else if (input==KEY_UP) newShape=rotateClockwise(newShape);
		if ( delay==0){
			++yPos;
			delay=20000;
		}
		if ( !drawShape(xPos, yPos, newShape, field) ){
			newShape=generateNewShape();
			xPos=(12+newShape.width)/4;
			yPos=1;
		}
		wrefresh(field);
	}

	endwin();			// End curses mode		  
	return 0;
}

struct shapeStruct generateNewShape(){
	int shape=rand()%5;
	int flipRow=rand()%2;
	int flipCol=rand()%2;
	int startRow=(shapeDimensions[shape][HEIGHT]-1)*(flipRow==TRUE);
	int startCol=(shapeDimensions[shape][WIDTH]-1)*(flipCol==TRUE);
	int nextRow=1-(2*(flipRow==TRUE));
	int nextCol=1-(2*(flipCol==TRUE));

	struct shapeStruct newShape;
	newShape.colour=shape+1;
	newShape.height=shapeDimensions[shape][HEIGHT];
	newShape.width=shapeDimensions[shape][WIDTH];

	for (int inRow=startRow, outRow=0; outRow<shapeDimensions[shape][HEIGHT]; inRow+=nextRow, ++outRow){
		for (int inCol=startCol, outCol=0; outCol<shapeDimensions[shape][WIDTH]; inCol+=nextCol, ++outCol){
			newShape.bitmap[(outRow*shapeDimensions[shape][WIDTH])+outCol]=shapeBitmaps[shape][(inRow*shapeDimensions[shape][WIDTH])+inCol];
		}
	}
	return newShape;
}

struct shapeStruct rotateClockwise(struct shapeStruct oldShape){
	struct shapeStruct newShape;
	for (int inCol=0, outPos=0; inCol<oldShape.width; ++inCol){
		for (int inRow=oldShape.height-1; inRow>=0; --inRow, ++outPos){
			newShape.bitmap[outPos]=oldShape.bitmap[(inRow*oldShape.width)+inCol];
		}
	}
	newShape.height=oldShape.width;
	newShape.width=oldShape.height;
	newShape.colour=oldShape.colour;
	return newShape;
}

int drawShape(int xPos, int yPos, struct shapeStruct newShape, WINDOW *field){
	for (int row=0, bitmapPos=0; row<newShape.height; ++row){
		for (int col=0; col<newShape.width; ++col, ++bitmapPos){
			if (newShape.bitmap[bitmapPos]==1) mvwaddch(field,row+yPos, col+xPos, ' ' | COLOR_PAIR(newShape.colour));
		}
	}
	for (int col=0, row=newShape.height-1; col<newShape.width; ++col){
		if ( newShape.bitmap[ (row*newShape.width) + col ] == 0){
			--col;
			--row;
			continue;
		}
		else if ( mvwinch(field,row+yPos+1, col+xPos)&A_COLOR ) return FALSE;
		else row=newShape.height-1;
	}

	return TRUE;
}

int xCollision(int xPos, int yPos, struct shapeStruct newShape, WINDOW *field){
	// finish
}

void clearShape(int xPos, int yPos, struct shapeStruct newShape, WINDOW *field){
	for (int row=0, bitmapPos=0; row<newShape.height; ++row){
		for (int col=0; col<newShape.width; ++col, ++bitmapPos){
			if (newShape.bitmap[bitmapPos]==1) mvwaddch(field,row+yPos, col+xPos, ' ');
		}
	}
	return;
}
