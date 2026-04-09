#include <stdio.h>
#include <stdlib.h>
#include <limits.h>
#include <windows.h>
#include <conio.h>
#include <time.h>
#include <stdint.h>
#define GRID_SYMBOL '*'
#define SNAKE_SYMBOL 'O'
#define CHERRY_SYMBOL '@'
#define ROW 20
#define COL 20



typedef struct snake snake;
typedef struct piece piece;
typedef enum direction direction;
typedef enum state state;
typedef uint8_t uint8;
typedef uint32_t uint32;


int max_score = 0;

struct piece {
	int c;
	int r;
	struct piece* next;
};

enum state{
	EMPTY,
	SNAKE,
	CHERRY
};

enum direction{
	LEFT,
	RIGHT,
	UP,
	DOWN
};

struct snake {
    struct piece* head;
    struct piece* tail;
    int size;
    enum direction dir;
    enum direction nextD;
};


int MAX(int a, int b)
{
	if (a>=b)
		return a;
	return b;
}

uint32 generate_random_number()
{
	uint32 ran = 0;
	ran|=(rand()<<16);
	ran|=rand();
	return ran;
}


int hit_wall(snake* s)
{
	if (s->head->r==-1)
		return 1;
	if (s->head->r==ROW )
		return 1;
	if (s->head->c==-1)
		return 1;
	if (s->head->c==COL )
		return 1;
	return 0;
}




void draw(uint8 arr[][COL], int size)
{	
	for (int i=0;i<ROW;i++)
	{
		for(int k=0;k<COL;k++)
			switch(arr[i][k])
			{
				case EMPTY:
				{
					printf("%c",GRID_SYMBOL);
					break;
				}
				case SNAKE:
				{
					printf("\x1b[32m%c\x1b[0m", SNAKE_SYMBOL);
					break;
				}
				case CHERRY:
				{
					printf("\x1b[31m%c\x1b[0m", CHERRY_SYMBOL);
					break;
				}

			}
		printf("\n");
	}
	printf("score: %d\n", size*10);
	printf("max score: %d\n", max_score*10);
}


void update_snake(snake* bas)
{
	
	switch (bas->dir)
	{
		case UP:
		{
			bas->tail->c=bas->head->c;
			bas->tail->r=bas->head->r-1;
			break;
		}
		case DOWN:
		{
			bas->tail->c=bas->head->c;
			bas->tail->r=bas->head->r+1;
			break;
		}
		case LEFT:
		{
			bas->tail->c=bas->head->c-1;
			bas->tail->r=bas->head->r;
			break;
		}
		case RIGHT:
		{
			bas->tail->c=bas->head->c+1;
			bas->tail->r=bas->head->r;
			break;
		}
	}
	piece *new_head = bas->tail;
	bas->tail=bas->tail->next;
	bas->head->next=new_head;
	bas->head=new_head;
	bas->head->next=NULL;
	

}



void next(snake *s)
{
	direction d=s->dir;
	char last;
	char ch = 0;
	while (kbhit()) 
	{
		ch = getch();
    	if (ch == 'w' || ch == 'a' || ch == 's' || ch == 'd')
        	last = ch;
	}
	switch (last)
	{
		case 'w':
		{
			s->dir=UP;
			break;
		}
		case 'd':
		{
			s->dir=RIGHT;
			break;
		}
		case 's':
		{
			s->dir=DOWN;
			break;
		}
		case 'a':
		{
			s->dir=LEFT;
			break;
		}
		default:
		{
			s->dir=s->dir;
			break;
		}
	}
	if (((d == UP && s->dir == DOWN) ||
    	(d == DOWN && s->dir == UP) ||
    	(d == LEFT && s->dir == RIGHT) ||
    	(d == RIGHT && s->dir == LEFT)))
		{
    		s->dir = d;
		}
}

int ate_cherry(snake* s, int cherry[2])
{
	if (s->head->r==cherry[0] && s->head->c==cherry[1])
		return 1;
	return 0;
}
void free_snake(piece* c)
{
	while (c)
	{
		piece *p=c->next;
		free(c);
		c = p;
	}
}

void update_head(snake* bas)
{
	switch (bas->dir)
	{
		case UP:
		{
			bas->head->r-=1;
			break;
		}
		case DOWN:
		{
			bas->head->r+=1;
			break;
		}
		case LEFT:
		{
			bas->head->c-=1;
			break;
		}
		case RIGHT:
		{
			bas->head->c+=1;
			break;
		}
	}
}

void initialize(snake* bas, piece* h, piece* t,uint8 arr[][COL])
{
	h->next=NULL;
	h->c=COL/2;
	h->r=ROW/2;
	t->r=ROW/2;
	t->c=COL/2 -1;
	t->next=h;
	bas->head=h;
	bas->tail=t;
	bas->size=2;
	for (int i=0;i<ROW;i++)
	{
		for(int k=0;k<COL;k++)
			arr[i][k]=EMPTY;	
	}
	arr[ROW/2][ROW/2]=SNAKE;
	arr[ROW/2][ROW/2-1]=SNAKE;
}


void end_game(snake* bas)
{
	fflush(stdout);
	printf("\033[%dB", ROW);
	printf("game over, final score %d\n",bas->size*10);
	if( bas->size>max_score)
		printf("new max score!!\n");
	fflush(stdout);
}




piece* add_piece(snake* bas)
{
	piece* newT=malloc(sizeof(piece));
	newT->r=bas->tail->r;
	newT->c=bas->tail->c;
	bas->size+=1;
	return newT;
}

void cherry_opt(uint8 arr[][COL], int cherry[])
{
	int ind;
	while (1)
	{
		ind=generate_random_number();
		ind%=COL*ROW;
		if(arr[ind/COL][ind%COL]==EMPTY)
		{
			cherry[0]=ind/COL;
			cherry[1]=ind%COL;
			break;
		}
	}
}

int set_cherry(uint8 arr[][COL],  snake* bas, piece* newT, int cherry[], int speed)
{
	arr[newT->r][newT->c]=SNAKE;
	newT->next=bas->tail;
	bas->tail=newT;
	cherry_opt(arr, cherry);
	arr[cherry[0]][cherry[1]]=CHERRY;
	if (bas->size%10==0 && speed>10)
		speed-=10;
	return speed;
}


void end_game_question(int* ans, char* ch)
{
	  if (kbhit())
    {
        *ch = getch();
        if (*ch == 'r' || *ch == 'q')
            *ans = 0;
    }
}


int main()
{
	srand(time(NULL));   
	snake bas = {0};
	restart:
	free_snake(bas.tail);
	bas.tail = NULL;
	bas.head = NULL;
	bas.dir = RIGHT;
	piece *h=malloc(sizeof(piece));
	if (!h)
	{
		printf("allocation failed\n");
		return 0;		
	}
	piece* t =malloc(sizeof(piece));
	if (!t)
	{
		printf("allocation failed\n");
		return 0;
	}
	int cherry[2];
	char buffer[1024];
    // Set stdout to fully buffered with a 1024-byte buffer
    setvbuf(stdout, buffer, _IOFBF, sizeof(buffer));
	uint8 arr[ROW][COL];
	initialize(&bas,h,t,arr);
	int ate=0;  
	char ch;
	int speed = 200;
	bas.size=2;
	cherry_opt(arr,cherry);

	arr[cherry[0]][cherry[1]]=CHERRY;
	piece* newT;
	while (1)
	{
		if (ate_cherry(&bas,cherry))
		{
			newT=add_piece(&bas);
			if (!newT)
			{
				printf("couldnt allocate the memory for the snake\n");
				fflush(stdout);
				end_game(&bas);
				max_score=MAX(bas.size, max_score);
				int ans=1;
				printf("to close the game press q. to restart press r\n");
				fflush(stdout);
				while(ans) end_game_question(&ans, &ch);
				if (ch=='q') goto endgame;
				if (ch=='r') goto restart;
				else if (ch=='q') goto endgame;
			}
			ate=1;
		}
		
		arr[bas.tail->r][bas.tail->c]=EMPTY;
		update_snake(&bas);



		if (hit_wall(&bas) || arr[bas.head->r][bas.head->c]==SNAKE)
		{	
			end_game(&bas);
			max_score=MAX(bas.size, max_score);
			int ans=1;
			printf("to close the game press q. to restart press r\n");
			fflush(stdout);
			while(ans) end_game_question(&ans, &ch);	
			if (ch=='q') goto endgame;
			if (ch=='r') goto restart;
			else if (ch=='q') goto endgame;
		}
		arr[bas.head->r][bas.head->c]=SNAKE;

	


		if (ate)
		{
			speed=set_cherry(arr, &bas,newT,cherry, speed);
			ate=0;
		}
		draw(arr, bas.size);
		printf("\033[%dA", ROW+2);
		fflush(stdout);
		Sleep(speed);
		next(&bas);
	}
	endgame:
	printf("\033[%dB", ROW);
	fflush(stdout);
	free_snake(bas.tail);
	return 0;

}