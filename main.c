//tarea 2
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX 100

struct tank {
	int col;
	int row;
	int dir;	// 0=up, 1=down, 2=left, 3=right
	int hp;
};

struct shoot {
	int col;
	int row;
};

void cls() {
	system("clear");
}

// --------------------------------------------------------------------------- //
// --------------------------------------------------------------------------- //

int map[MAX][MAX];
int rows=0; 
int cols=0;
struct tank t1, t2;

// --------------------------------------------------------------------------- //
// --------------------------------------------------------------------------- //

void loadMap(const char *filename)
{
    FILE *fp = fopen(filename, "r");
    if (fp == NULL)
    {
        printf("Error: archivo no existe.\n");
        exit(1);
    }

    char line[256];

    while (fgets(line, sizeof(line), fp))
    {
    	cols = 0;
        for (int i = 0; line[i] != '\0' && line[i] != '\n'; i++)
        {
            if (line[i] >= '0' && line[i] <= '9')
			{
                if (line[i] == '3') 
                {
                	t1.col = cols;
                	t1.row = rows;
                	t1.dir = 0;
                	t1.hp  = 3;
                	map[rows][cols] = 0;
                }
                else if (line[i] == '4') 
                {
                	t2.col = cols;
                	t2.row = rows;
                	t2.dir = 0;
                	t2.hp  = 3;
                	map[rows][cols] = 0;
                }
                else 
                {
                	map[rows][cols] = line[i] - '0';
                }
                (cols)++;
            }
        }

        (rows)++;
    }

    fclose(fp);
}

void printHUD(int turn)
{
    printf("Turn: %d\n", turn);
    printf("T3 HP: %d   T4 HP: %d\n", t1.hp, t2.hp);
	printf("T3 dir: %d   T4 dir: %d\n", t1.dir, t2.dir);

    if (turn % 2 == 1)
        printf("Turn of \033[1;36m3\033[0m\n");
    else
        printf("Turn of \033[1;35m4\033[0m\n");

    printf("Select: (u)p, (d)own, (l)eft, (r)ight, (s)hoot\n");
}

void printMap()
{
    for (int i = 0; i < rows; i++) {
        for (int j = 0; j < cols; j++) {

            if (t1.row == i && t1.col == j)
                printf("\033[1;36m3\033[0m ");
            else if (t2.row == i && t2.col == j)
                printf("\033[1;35m4\033[0m ");
            else if (map[i][j] == 1)
                printf("\033[1;43m1\033[0m ");
            else if (map[i][j] == 2)
                printf("\033[1;41m2\033[0m ");
            else
                printf("\033[1;30m%d\033[0m ", map[i][j]);
        }
        printf("\n");
    }
}

void moveTank(int turn, char moveDir)
{
	struct tank out;
	struct tank con; //CONtrary tank
	
	if(turn%2==1)
	{
		out = t1;
		con = t2;
	}
	else
	{
		out = t2;
		con = t1;
	}

	switch (moveDir)
	{
		case 'U': case 'u':
			out.dir = 0;
			if (out.row - 1 > 0 && map[out.row - 1][out.col] == 0)
			{
				if(out.row-1 == con.row && out.col == con.col)
					break;
				else
					out.row--;
			}
			break;

		case 'D': case 'd':
			out.dir = 1;
			if (out.row + 1 < rows && map[out.row + 1][out.col] == 0)
			{
				if(out.row+1 == con.row && out.col == con.col)
					break;
				else
					out.row++;
			}
			break;
	
		case 'L': case 'l':
			out.dir = 2;
			if (out.col - 1 > 0 && map[out.row][out.col - 1] == 0)
			{
				if(out.row == con.row && out.col-1 == con.col)
					break;
				else
					out.col--;
			}
			break;
	
		case 'R': case 'r':
			out.dir = 3;
			if (out.col + 1 < cols && map[out.row][out.col + 1] == 0)
			{
				if(out.row == con.row && out.col+1 == con.col)
					break;
				else
					out.col++;
			}
			break;
	
		default:
			break;
	}

	if(turn%2==1)
		t1 = out;
	else
		t2 = out;
}

void shoot(int sRow, int sCol, int sDir, int shooter)
{
	struct shoot s;
	s.row = sRow;
	s.col = sCol;
	
	while(true)
	{
		switch (sDir)
		{
			case 0: //up
				s.row--;
				break;
				
			case 1://down
				s.row++;
				break;
			
			case 2://left
				s.col--;
				break;
			
			case 3://right
				s.col++;
				break;
			
		}
		if(map[s.row][s.col] == 2)
			break;
		else if(map[s.row][s.col] == 1)
		{
			map[s.row][s.col] = 0;
			break;
		}
		else
		{
			if(shooter == 1)
				if(s.row == t2.row && s.col == t2.col)
				{
					t2.hp--;
					break;
				}
			if(shooter == 2)
				if(s.row == t1.row && s.col == t1.col)
				{
					t1.hp--;
					break;
				}
		}
	}
}

// --------------------------------------------------------------------------- //
// --------------------------------------------------------------------------- //

int win = 0;

int main()
{
	loadMap("mapa.txt");

	for(int turn = 0; turn <= 300; turn++)
	{
		char move;
		
		printHUD(turn);
		printMap();
		
		scanf(" %c", &move);
		if(move == 's' || move == 'S')
		{
			if(turn%2==1)
				shoot(t1.row, t1.col, t1.dir, 1);
			else
				shoot(t2.row, t2.col, t2.dir, 2);
		}
		else
			moveTank(turn, move);

		if(t1.hp == 0)
		{
			win = 1;
			printf("\nGANA EL JUGADOR 2\n");
			break;
		}
		if(t2.hp == 0)
		{
			win = 1;
			printf("\nGANA EL JUGADOR 1\n");
			break;
		}
	}

	if(!win)
	{
		if(t1.hp > t2.hp)
			printf("\nGANA EL JUGADOR 2\n");
		if(t2.hp > t1.hp)
			printf("\nGANA EL JUGADOR 1\n");
		if(t1.hp == t2.hp)
			printf("\nEMPATE\n");
	}
}
