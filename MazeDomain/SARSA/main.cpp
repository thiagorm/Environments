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

const float alfa = 0.8, gamma = 0.8, eGreedy = 0.9;

//grava��o de recompensa por episodio
void gravarRecompensas(FILE *link, float reward)
{
	if((link = fopen("reward_perEpisode_qLearning.txt", "a+t")) == NULL)
	{
		printf("Erro ao abrir arquivo para grava��o!");
		getch();
		exit(1);
	}

	fprintf(link,"%.2f\n",reward);

	fclose(link);
}

void gravarAcoes(FILE *link, int actions, char name[])
{
	if((link = fopen(name, "a")) == NULL)
	{
		printf("Erro ao abrir arquivo para grava��o!");
		getch();
		exit(1);
	}

	fprintf(link,"%d\n",actions);

	fclose(link);
}


struct Flags
{
	int i, j;
	int visit;
};

struct State
{
	//o indice 0 do vetor � a a��o A e o indice 1 a a��o B
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

//Essa fun��o verifica qual o maior valor no vetor das a��es
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

//Essa fun��o verifica qual a maior a��o, verificando qual o maior valor dentro do vetor a��es
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
			//se for igual os valores � necessario pegar uma a��o aleatoria
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
			//se for igual os valores � necessario pegar uma a��o aleatoria
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

// Essa fun��o seleciona a a��o a partir do m�todo semi-uniform random exploration
int getAction(struct State s, float eGreedy)
{	
	float random_eGreedy = 0;
	int random_action;
	int action = NULL;
	
	random_eGreedy = (float) rand()/(float)RAND_MAX;
	
	//Se a probabilidade de selecionar uma a��o for maior 0.2 a a��o ser� randomica
	//sen�o se for a probabilidade inversa 1-p o algoritmo escolhera a melhor a��o entre as duas existentes 
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

float SARSACalculate(float QValue, float nextQValue, float reward, float alfa, float gamma)
{
	return (QValue + alfa * (reward + ((gamma * nextQValue) - QValue)));
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
	//O actual state come�a com 0,pois � o estado inicial
	int flag_reward = 1, number_collected_flags = 0, ordinary_reward = 0, next_state_i = 0, next_state_j = 0, actions = 0, nextAction = 0;
	int count = 0, action = NULL, i = 0, j = 0, k =0, wall = -999, goal = 0;
	float nextQValue = 0;
	//com alfa sendo 1, pode-se entender as a��es como deterministicas
	int x = 0, y = 0, flag_y = 0, flag_x;
	struct Flags flag_1 = {0,0,0}, flag_2 = {0,0,0}, flag_3 = {0,0,0};

	int l=0, total_reward = 0;
	char name[80];

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

	action = getAction(states[i][j], eGreedy);

	for(l = 0; l < 10; l++)
	{
	strcpy(name, l+"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.txt");
	while(count != convergence_time)
	{
		//printf("Action: %d\n", action);
		//printf("Actual state: %d, %d\n", i, j);

		switch(action)
		{
			//UP
			case 0:

				if(verifyFlags(&flag_1, i, j) == 1 || verifyFlags(&flag_2, i, j) == 1 || verifyFlags(&flag_3, i, j) == 1)
				{
					number_collected_flags += 1;
				}

				if(i - 1 >= 0)
				{
					if(states[i-1][j].actions[action] != wall)
					{
						next_state_i = i-1;
						nextAction = getAction(states[next_state_i][j], eGreedy);
						nextQValue = states[next_state_i][j].actions[nextAction];
						if(i-1 == 0 && j == (number_col-1))
						{
							states[i][j].actions[action] = SARSACalculate(states[i][j].actions[action], nextQValue, number_collected_flags, alfa, gamma);
							total_reward += number_collected_flags;
						}
						else
						{
							states[i][j].actions[action] = SARSACalculate(states[i][j].actions[action], nextQValue, ordinary_reward, alfa, gamma);
							total_reward += ordinary_reward;
						}
						i = next_state_i;
					}
					else
						nextAction = getAction(states[i-1][j], eGreedy);
				}
				else
					nextAction = getAction(states[i-1][j], eGreedy);
				actions++;
				break;

			//DOWN
			case 1:

				if(verifyFlags(&flag_1, i, j) == 1 || verifyFlags(&flag_2, i, j) == 1 || verifyFlags(&flag_3, i, j) == 1)
				{
					number_collected_flags += 1;
				}

				if(i + 1 < number_lines)
				{
					if(states[i+1][j].actions[action] != wall)
					{
						next_state_i = i+1;
						nextAction = getAction(states[next_state_i][j], eGreedy);
						nextQValue = states[next_state_i][j].actions[nextAction];
						if(i+1 == 0 && j == (number_col-1))
						{
							states[i][j].actions[action] = SARSACalculate(states[i][j].actions[action], nextQValue, number_collected_flags, alfa, gamma);
							total_reward += number_collected_flags;
						}
						else
						{
							states[i][j].actions[action] = SARSACalculate(states[i][j].actions[action], nextQValue, ordinary_reward, alfa, gamma);
							total_reward += ordinary_reward;
						}
						i = next_state_i;
					}
					else
						nextAction = getAction(states[i+1][j], eGreedy);
				}
				else
					nextAction = getAction(states[i+1][j], eGreedy);
				actions++;
				break;

			//RIGHT
			case 2:

				if(verifyFlags(&flag_1, i, j) == 1 || verifyFlags(&flag_2, i, j) == 1 || verifyFlags(&flag_3, i, j) == 1)
				{
					number_collected_flags += 1;
				}

				if(j + 1 < number_col)
				{

					if(states[i][j+1].actions[action] != wall)
					{
						next_state_j = j+1;
						nextAction = getAction(states[i][next_state_j], eGreedy);
						nextQValue = states[i][next_state_j].actions[nextAction];
						if(i == 0 && j+1 == 6)
						{
							states[i][j].actions[action] = SARSACalculate(states[i][j].actions[action], nextQValue, number_collected_flags, alfa, gamma);
							total_reward += number_collected_flags;
						}
						else
						{
							states[i][j].actions[action] = SARSACalculate(states[i][j].actions[action], nextQValue, ordinary_reward, alfa, gamma);
							total_reward += ordinary_reward;
						}
						j = next_state_j;
					}
					else
						nextAction = getAction(states[i][j+1], eGreedy);
				}
				else
					nextAction = getAction(states[i][j+1], eGreedy);
				actions++;
				break;

			//LEFT
			case 3:

				if(verifyFlags(&flag_1, i, j) == 1 || verifyFlags(&flag_2, i, j) == 1 || verifyFlags(&flag_3, i, j) == 1)
				{
					number_collected_flags += 1;
				}

				if(j - 1 >= 0)
				{

					if(states[i][j-1].actions[action] != wall)
					{
						next_state_j = j-1;
						nextAction = getAction(states[i][next_state_j], eGreedy);
						nextQValue = states[i][next_state_j].actions[nextAction];
						if(i == 0 && j-1 == 6)
						{
							states[i][j].actions[action] = SARSACalculate(states[i][j].actions[action], nextQValue, number_collected_flags, alfa, gamma);
							total_reward += number_collected_flags;
						}
						else
						{
							states[i][j].actions[action] = SARSACalculate(states[i][j].actions[action], nextQValue, ordinary_reward, alfa, gamma);
							total_reward += ordinary_reward;
						}
						j = next_state_j;
					}
					else
						nextAction = getAction(states[i][j-1], eGreedy);
				}
				else
					nextAction = getAction(states[i][j-1], eGreedy);
				actions++;
				break;
		}

		action = nextAction;

		/*printf("i: %d\n", i);
		printf("j: %d\n", j);
		printf("action: %d\n", action);
		printf("nextAction: %d\n", nextAction);
		printf("QValue: %.2f\n", states[i][j].actions[action]);
		printf("nextQValue: %.2f\n", nextQValue);
		printf("flags: %d\n", number_collected_flags);
		printf("count: %d\n", count);
		printf("\n");
		getch();*/

		if(i == 0 && j == 6)
		{
			i = 0;
			j = 0;
			next_state_i = 0;
			next_state_j = 0;
			//printf("number collect flags: %d\n", number_collected_flags);
			//getch();
			number_collected_flags = 0;
			count++;
			setFlags(&flag_1, 0, 2, 0);
			setFlags(&flag_2, 4, 6, 0);
			setFlags(&flag_3, 5, 0, 0);
			gravarAcoes(arq, total_reward, name);
			//gravarAcoes(arq, actions, name);
			//actions = 0;
		}
			
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

		action = getAction(states[i][j], eGreedy);

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
				printf("%.5f", states[i][j].actions[0]);
				y += 1;
				gotoxy(x,y);
				printf("%.5f", states[i][j].actions[1]);
				y += 1;
				gotoxy(x,y);
				printf("%.5f", states[i][j].actions[2]);
				y += 1;
				gotoxy(x,y);
				printf("%.5f", states[i][j].actions[3]);
				printf("  ");
				//getch();
				x = flag_x;
				y = flag_y;
			}
			else
			{
				gotoxy(x,y);
				//getch();
				printf("%.5f", states[i][j].actions[0]);
				y += 1;
				gotoxy(x,y);
				printf("%.5f", states[i][j].actions[1]);
				y += 1;
				gotoxy(x,y);
				printf("%.5f", states[i][j].actions[2]);
				y += 1;
				gotoxy(x,y);
				printf("%.5f", states[i][j].actions[3]);
				printf("  ");
				//getch();
				x += 8;
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
	
	
	getch();

	return 1;
}