#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <iostream>
using namespace std;

#define number_actions 4
#define number_lines 6
#define number_col 7
#define convergence_time 20000

const float eGreedy = 0.9, gamma = 0.8, alfa = 0.8;

//gravação de recompensa por episodio
void gravarRecompensas(FILE *link, float r, char name[])
{
	if((link = fopen(name, "a")) == NULL)
	{
		printf("Erro ao abrir arquivo para gravação!");
		getch();
		exit(1);
	}

	fprintf(link,"%.2f\n",r);

	fclose(link);
}

void gravarAcoes(FILE *link, int actions, char name[])
{
	if((link = fopen(name, "a")) == NULL)
	{
		printf("Erro ao abrir arquivo para gravação!");
		getch();
		exit(1);
	}

	fprintf(link,"%d\n",actions);

	fclose(link);
}

BOOL MaximizeConsole()
{
    BOOL bRet = FALSE;
    HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
    if(hStdOut != INVALID_HANDLE_VALUE && hStdOut != NULL)
    {
        CONSOLE_FONT_INFO cfi = {0};
        CONSOLE_SCREEN_BUFFER_INFO csbi = {0};
        if(GetCurrentConsoleFont(hStdOut, FALSE, &cfi) && GetConsoleScreenBufferInfo(hStdOut, &csbi))
        {
            HDC hdcScreen = GetDC(NULL);
            if(hdcScreen)
            {
                int xScreen = GetDeviceCaps(hdcScreen, HORZRES);
                // get number of characters that would fit across
                SHORT newConsoleX = xScreen / cfi.dwFontSize.X;
                int yScreen = GetDeviceCaps(hdcScreen, VERTRES);
                // get number of characters that would fit down
                SHORT newConsoleY = yScreen / cfi.dwFontSize.Y;
                // ensure we don't make any dimensions smaller
                csbi.dwSize.X = max(csbi.dwSize.X, newConsoleX);
                csbi.dwSize.Y = max(csbi.dwSize.Y, newConsoleY);
                if(SetConsoleScreenBufferSize(hStdOut, csbi.dwSize))
                {
                    bRet = ShowWindow(GetConsoleWindow(), SW_MAXIMIZE);
                }
                ReleaseDC(NULL, hdcScreen);
            }
        }
    }
    return bRet;
}

struct Flags
{
	int i, j;
	int visit;
};

struct State
{
	//o indice 0 do vetor é a ação A e o indice 1 a ação B
	//actions[0] = up; actions[1] = down; actions[2] = right; actions[3] = left;
	float actions[number_actions];
};

void gotoxy(int x, int y)
{
COORD coord;
coord.X = x; coord.Y = y;
HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
SetConsoleCursorPosition(hStdOut, coord);
}

//Essa função verifica qual o maior valor no vetor das ações
float verifyMaiorValue(struct State s)
{
	float maior = 0;
	int i = 0;
	
	maior = s.actions[0];
	
	for(i = 1; i < number_actions; i++)
	{
		if(s.actions[i] > maior)
		{
			maior = s.actions[i];
		}
	}
	
	return maior;
}

//Essa função verifica qual a maior ação, verificando qual o maior valor dentro do vetor ações
int verifyMaiorAction(struct State s)
{
	/*int i;
	float maior = 0;
	int action = -1;
	
	maior = s.actions[0];
	
	for(i = 1; i < number_actions; i++)
	{
		if(s.actions[i] > maior)// && maior != s.actions[i])
		{
			maior = s.actions[i];
			action = i;
		}
		else
		{
			//action = -1;
			//se for igual os valores é necessario pegar uma ação aleatoria
			action = ((rand()%4));
		}
	}
	
	return action;*/

	int i = 0;
	float maior = 0;
	int action = 0;
	int countMax = 0, count = 0;
	int numberActions[number_actions];
	int boolean = 0;
	
	maior = s.actions[0];
	numberActions[countMax] = action;

	for(i = 1; i < number_actions; i++)
	{
		if(s.actions[i] > maior)// && maior != s.actions[i])
		{
			maior = s.actions[i];
			action = i;
			countMax = 0;
			numberActions[countMax] = action;
			boolean = 0;
		}
		else if(s.actions[i] == maior)
		{
			//action = -1;
			//se for igual os valores é necessario pegar uma ação aleatoria
			//action = ((rand()%2)+1);
			countMax++;
			action = i;
			numberActions[countMax] = action;
			boolean = 1;
			//printf("enter\n");
		}
	}

	countMax++;
	//printf("count max: %d\n", countMax);

	if(boolean == 1)
	{
		int randomIndex = ((rand()%countMax));
		action = numberActions[randomIndex];
	}

	/*if(action == -1)
	{
		action = (int) ((rand()%2)+1);
	}*/
	

	return action;
	
}

// Essa função seleciona a ação a partir do método semi-uniform random exploration
int getAction(struct State s, float eGreedy)
{	
	float random_eGreedy = 0;
	int random_action;
	int action = NULL;
	
	random_eGreedy = (float) rand()/(float)RAND_MAX;
	
	//Se a probabilidade de selecionar uma ação for maior 0.2 a ação será randomica
	//senão se for a probabilidade inversa 1-p o algoritmo escolhera a melhor ação entre as duas existentes 
	if(random_eGreedy < eGreedy)
	{
		action = ((rand()%4));
		//printf("Acao Random\n");
	}
	else
	{
		action = verifyMaiorAction(s);
		//printf("Acao Maior\n");
	}
	
	return action;

}

float qLearningCalculate(float QValue, float maxQValue, float reward, float alfa, float gamma)
{
	return (QValue + alfa * (reward + ((gamma * maxQValue) - QValue)));
}

float getMaxQValue(struct State s)
{
	float maxQValue = 0;
	
	maxQValue = verifyMaiorValue(s);
	
	return maxQValue;
}

int verifyFlags(struct Flags *flag, int i, int j)
{
	int valueBack = 0;

	if(flag->i == i && flag->j == j)
	{
		if(flag->visit == 0)
			valueBack = 1;

		flag->visit = 1;

	}
	else valueBack = 0;

	return valueBack;
}

void setFlags(struct Flags *flag, int i, int j, int visit)
{
	flag->i = i;
	flag->j = j;
	flag->visit = visit;
}

int main()
{
	FILE *arq = NULL;
	struct State states[number_lines][number_col];
	//O actual state começa com 0,pois é o estado inicial
	int flag_reward = 1, number_collected_flags = 0, ordinary_reward = 0, next_state_i = 0, next_state_j = 0, actions = 0;
	int count = 0, action = NULL, i = 0, j = 0, k =0, wall = -999, goal = 0;
	float maxQValue = 0;
	//com alfa sendo 1, pode-se entender as ações como deterministicas
	int x = 0, y = 0, flag_y = 0, flag_x;
	struct Flags flag_1 = {0,0,0}, flag_2 = {0,0,0}, flag_3 = {0,0,0};

	int l=0, total_reward = 0;
	char name[80];
	char name2[80];
	char name3[80];
	char name4[80];

	MaximizeConsole();

	float AcaoUp = 0, AcaoDown = 0, AcaoLeft = 0, AcaoRight = 0;

	int flag0 = 0;

	setFlags(&flag_1, 0, 2, 0);
	setFlags(&flag_2, 4, 6, 0);
	setFlags(&flag_3, 5, 0, 0);

	//printf("flag_1 i, j, visit: %d %d %d\n", flag_1.i, flag_1.j, flag_1.visit);
	//printf("flag_2 i, j, visit: %d %d %d\n", flag_2.i, flag_2.j, flag_2.visit);
	//printf("flag_3 i, j, visit: %d %d %d\n", flag_3.i, flag_3.j, flag_3.visit);
	
	for(i = 0; i < number_lines; i++)
	{
		for(j = 0; j < number_col; j++)
		{
			for(k = 0; k < number_actions; k++)
			{
				if((i == 0 || i == 1)  && (j == 1 || j == 4))
					states[i][j].actions[k] = wall;
				else if(i == 3 && (j == 0 || j == 1 || j == 5 || j == 6))
					states[i][j].actions[k] = wall;
				else if(i == 5 && j == 6)
					states[i][j].actions[k] = wall;
				else if(i == 0 && j == 6)
					states[i][j].actions[k] = goal;
				else
					states[i][j].actions[k] = 0;
			}
		}
			
	}
	
	srand ( time(NULL) );
	
	i = 0;
	j = 0;

	for(l = 0; l < 10; l++)
	{
	strcpy(name, l+"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.txt");
	strcpy(name2, l+"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb.txt");
	strcpy(name3, l+"cccccccccccccccccccccccccccccccc.txt");
	strcpy(name4, l+"dddddddddddddddddddddddddddddddddd.txt");
	while(count != convergence_time)
	{
		//printf("Action: %d\n", action);
		//printf("Actual state: %d, %d\n", i, j);
		
		action = getAction(states[i][j], eGreedy);

		switch(action)
		{
			//UP
			case 0:
				actions++;
				if(verifyFlags(&flag_1, i, j) == 1 || verifyFlags(&flag_2, i, j) == 1 || verifyFlags(&flag_3, i, j) == 1)
				{
					number_collected_flags += 1;
				}

				if(i - 1 >= 0)
				{
					if(states[i-1][j].actions[action] != wall)
					{
						next_state_i = i-1;
						maxQValue = getMaxQValue(states[next_state_i][j]);
						if(i-1 == 0 && j == (number_col-1))
						{
							states[i][j].actions[action] = qLearningCalculate(states[i][j].actions[action], maxQValue, number_collected_flags, alfa, gamma);
							total_reward += number_collected_flags;
							AcaoUp = states[i][j].actions[action];
						}
						else
						{
							states[i][j].actions[action] = qLearningCalculate(states[i][j].actions[action], maxQValue, ordinary_reward, alfa, gamma);
							total_reward += ordinary_reward;
							AcaoUp = states[i][j].actions[action];
						}
						i = next_state_i;
					}
				}
				
				break;

			//DOWN
			case 1:
				actions++;
				if(verifyFlags(&flag_1, i, j) == 1 || verifyFlags(&flag_2, i, j) == 1 || verifyFlags(&flag_3, i, j) == 1)
				{
					number_collected_flags += 1;
				}

				if(i + 1 < number_lines)
				{

					if(states[i+1][j].actions[action] != wall)
					{
						next_state_i = i+1;
						maxQValue = getMaxQValue(states[next_state_i][j]);
						if(i+1 == 0 && j == (number_col-1))
						{
							states[i][j].actions[action] = qLearningCalculate(states[i][j].actions[action], maxQValue, number_collected_flags, alfa, gamma);
							total_reward += number_collected_flags;
							AcaoDown = states[i][j].actions[action];
						}
						else
						{
							states[i][j].actions[action] = qLearningCalculate(states[i][j].actions[action], maxQValue, ordinary_reward, alfa, gamma);
							total_reward +=ordinary_reward;
							AcaoDown = states[i][j].actions[action];
						}
						i = next_state_i;
					}
				}
				
				break;

			//RIGHT
			case 2:
				actions++;
				if(verifyFlags(&flag_1, i, j) == 1 || verifyFlags(&flag_2, i, j) == 1 || verifyFlags(&flag_3, i, j) == 1)
				{
					number_collected_flags += 1;
				}

				if(j + 1 < number_col)
				{
					if(states[i][j+1].actions[action] != wall)
					{
						next_state_j = j+1;
						maxQValue = getMaxQValue(states[i][next_state_j]);
						if(i == 0 && j+1 == 6)
						{
							states[i][j].actions[action] = qLearningCalculate(states[i][j].actions[action], maxQValue, number_collected_flags, alfa, gamma);
							total_reward += number_collected_flags;
							AcaoRight = states[i][j].actions[action];
						}
						else
						{
							states[i][j].actions[action] = qLearningCalculate(states[i][j].actions[action], maxQValue, ordinary_reward, alfa, gamma);
							total_reward += ordinary_reward;
							AcaoRight = states[i][j].actions[action];
						}
						j = next_state_j;
					}
				}
				
				break;

			//LEFT
			case 3:
				actions++;
				if(verifyFlags(&flag_1, i, j) == 1 || verifyFlags(&flag_2, i, j) == 1 || verifyFlags(&flag_3, i, j) == 1)
				{
					number_collected_flags += 1;
				}

				if(j - 1 >= 0)
				{
					if(states[i][j-1].actions[action] != wall)
					{
						next_state_j = j-1;
						maxQValue = getMaxQValue(states[i][next_state_j]);
						if(i == 0 && j-1 == 6)
						{
							states[i][j].actions[action] = qLearningCalculate(states[i][j].actions[action], maxQValue, number_collected_flags, alfa, gamma);
							total_reward += number_collected_flags;
							AcaoLeft = states[i][j].actions[action];
						}
						else
						{
							states[i][j].actions[action] = qLearningCalculate(states[i][j].actions[action], maxQValue, ordinary_reward, alfa, gamma);
							total_reward += ordinary_reward;
							AcaoLeft = states[i][j].actions[action];
						}
						j = next_state_j;
					}
				}
				
				break;
		}

		if(i == 0 && j == 6)
		{
			i=0;
			j=0;
			next_state_i = 0;
			next_state_j = 0;
			if(number_collected_flags == 0)
				flag0++;
			number_collected_flags = 0;
			count++;
			setFlags(&flag_1, 0, 2, 0);
			setFlags(&flag_2, 4, 6, 0);
			setFlags(&flag_3, 5, 0, 0);
			gravarAcoes(arq, total_reward, name);
			//gravarAcoes(arq, actions, name);
			//actions = 0;
		}

		//gravarRecompensas(arq, AcaoUp, name);
		//gravarRecompensas(arq, AcaoDown, name2);
		//gravarRecompensas(arq, AcaoRight, name3);
		//gravarRecompensas(arq, AcaoLeft, name4);
		//count++;
			
		//printf("count: %d\n", count);
		//printf("-----------------------------------------------------------\n");
		//getch();
	}
		for(int i = 0; i < number_lines; i++)
		{
			for(int j = 0; j < number_col; j++)
			{
				for(int k = 0; k < number_actions; k++)
				{
					if((i == 0 || i == 1)  && (j == 1 || j == 4))
						states[i][j].actions[k] = wall;
					else if(i == 3 && (j == 0 || j == 1 || j == 5 || j == 6))
						states[i][j].actions[k] = wall;
					else if(i == 5 && j == 6)
						states[i][j].actions[k] = wall;
					else if(i == 0 && j == 6)
						states[i][j].actions[k] = goal;
					else
						states[i][j].actions[k] = 0;
				}
			}
			
		}


		i=0;
		j=0;
		next_state_i = 0;
		next_state_j = 0;
		count = 0;
		number_collected_flags = 0;
		setFlags(&flag_1, 0, 2, 0);
		setFlags(&flag_2, 4, 6, 0);
		setFlags(&flag_3, 5, 0, 0);
		actions = 0;
		total_reward = 0;
		AcaoDown = 0;
		AcaoLeft = 0;
		AcaoRight = 0;
		AcaoUp = 0;

		printf("%d\n", l);

	}

	for(i = 0; i < number_lines; i++)
	{
		for(j = 0; j < number_col; j++)
		{
			if(j == (number_col-1))
			{
				flag_y += 6;
				flag_x = 0;
				gotoxy(x,y);
				//getch();
				printf("%.15f", states[i][j].actions[0]);
				y += 1;
				gotoxy(x,y);
				printf("%.15f", states[i][j].actions[1]);
				y += 1;
				gotoxy(x,y);
				printf("%.15f", states[i][j].actions[2]);
				y += 1;
				gotoxy(x,y);
				printf("%.15f", states[i][j].actions[3]);
				printf("  ");
				//getch();
				x = flag_x;
				y = flag_y;
			}
			else
			{
				gotoxy(x,y);
				//getch();
				printf("%.15f", states[i][j].actions[0]);
				y += 1;
				gotoxy(x,y);
				printf("%.15f", states[i][j].actions[1]);
				y += 1;
				gotoxy(x,y);
				printf("%.15f", states[i][j].actions[2]);
				y += 1;
				gotoxy(x,y);
				printf("%.15f", states[i][j].actions[3]);
				printf("  ");
				//getch();
				x += 20;
				y = flag_y;
			}
		}
	}

	flag_y += 6;
	flag_x = 0;

	x = flag_x;
	y = flag_y;

	gotoxy(x,y);
	
	
	printf("********************************************************************\n");
	printf("********************************************************************\n");
	printf("actions: %d\n", actions);
	printf("flag0: %d\n", flag0);
	
	
	getch();

	return 1;
}