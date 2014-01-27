#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <iostream>
#include <string.h>
using namespace std;

#define number_actions 4
#define number_lines 6
#define number_col 9
#define convergence_time 20000

const float alfa = 0.1, gamma = 0.95, eGreedy = 0.1, lambda = 0.01;
const int reward = 0, reward_goal = 1;

void gotoxy(int x, int y)
{
COORD coord;
coord.X = x; coord.Y = y;
HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
SetConsoleCursorPosition(hStdOut, coord);
}

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


struct State
{
	//o indice 0 do vetor é a ação A e o indice 1 a ação B
	float actions[number_actions];
};

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

	//printf("%.1f\n", random_eGreedy);
	
	//Se a probabilidade de selecionar uma ação for maior 0.2 a ação será randomica
	//senão se for a probabilidade inversa 1-p o algoritmo escolhera a melhor ação entre as duas existentes 
	if(eGreedy > random_eGreedy)
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

bool verifyGoal(int i, int j)
{
	if(i == 0 && j == 8)
		return true;
	else
		return false;
}

float getMaxQValue(struct State s)
{
	float maxQValue = 0;
	
	maxQValue = verifyMaiorValue(s);
	
	return maxQValue;
}

void elegibilityTraces(float value_actual_state, float maxQValue, float reward, float gamma, struct State z[][number_col], int i, int j, int action, struct State grid[][number_col], float alfa, float lambda, int nextAction, int AStar,
	int nexti, int nextj)
{
	float max_a = 0;
	float delta = 0;
	int m = 0;
	int n = 0;
	int k = 0;

	if(nextAction == AStar)
		AStar = nextAction;

	//printf("AStar:%d\n", AStar);
	//printf("nextAction:%d\n", nextAction);
	//getch();

	max_a = maxQValue;
	delta = reward + ((gamma * grid[nexti][nextj].actions[AStar]) - value_actual_state);
	z[i][j].actions[action] = 1;

	for(m = 0; m < number_lines; m++)
	{
		for(n = 0; n < number_col; n++)
		{
			/*printf("m:%d\n",m);
			printf("n:%d\n",n);
			if(!verifyGoal(m, n))
			{
				printf("heyyyyyyyyyyyyyy\n");
			}
			getch();*/
			if(!verifyGoal(m,n))
			{
				//printf("m:%d\n",m);
				//printf("n:%d\n",n);
				//getch();
				for(k = 0; k < number_actions; k++)
				{
					if(grid[m][n].actions[k] != -999)
					{
						grid[m][n].actions[k] = grid[m][n].actions[k] + alfa * delta * z[m][n].actions[k]; 

						if(nextAction == AStar)
						{
							z[m][n].actions[k] = gamma * lambda * z[m][n].actions[k];
						}
						else
						{
							z[m][n].actions[k] = 0;
						}
					}
				}
			}
		}
	}

	//printf("\\\\\\\\\\\\\\\\\\\\\\\n");
	//getch();
}

void main()
{
	
	FILE *arq = NULL;
	struct State grid[number_lines][number_col];
	struct State z[number_lines][number_col];
	//O actual state começa com 0,pois é o estado inicial
	//int actual_state = 0, next_state = 0;
	int count = 0, action = NULL, i = 0, j = 0, k = 0, l = 0, actions = 0;
	float maxQValue = 0, total_reward = 0;
	int x = 0, y = 0, flag_y = 0, flag_x;
	char name[80], n;
	int nextAction = 0;
	int Astar = 0;
	int nextStatei = 0;
	int nextStatej = 0;
	
	for(i = 0; i < number_lines; i++)
	{
		for(j = 0; j < number_col; j++)
		{
			for(k = 0; k < number_actions; k++)
			{
				if((i == 1 && j == 2) || (i == 2 && j == 2) || (i == 3 && j == 2))
					grid[i][j].actions[k] = -999;
				else if(i == 4 && j == 5)
					grid[i][j].actions[k] = -999;
				else if((i == 0 && j == 7) || (i == 1 && j == 7) || (i == 2 && j == 7))
					grid[i][j].actions[k] = -999;
				//else if(i == 0 && j == 8)
					//grid[i][j].actions[k] = 100;
				else
					grid[i][j].actions[k] = 0;

					z[i][j].actions[k] = 0;
			}
		}
	}
	
	srand ( time(NULL) );
	
	i = 2;
	j = 0;
	action = getAction(grid[i][j], eGreedy);
	for(l = 0; l < 10; l++)
	{
	strcpy(name, l+"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.txt");
	while(count != convergence_time)
	{
		//printf("action: %d\n", action);
		//if(action != -1)
		//{
			switch(action)
			{
				//Ação UP escolhida
				case 0:
					if(i - 1 >= 0)
					{
						if(grid[i-1][j].actions[action] != -999)
						{
							maxQValue = getMaxQValue(grid[i-1][j]);
							nextAction = getAction(grid[i-1][j],eGreedy);
							Astar = verifyMaiorAction(grid[i-1][j]);
							nextStatei = i-1;
							nextStatej = j;
							if(i-1 == 0 && j == 8)
							{
								elegibilityTraces(grid[i][j].actions[action],maxQValue,reward_goal,gamma,z,i,j,action,grid,alfa,lambda, nextAction, Astar, nextStatei, nextStatej);
								total_reward+=reward_goal * gamma;
							}
							else
							{
								elegibilityTraces(grid[i][j].actions[action],maxQValue,reward,gamma,z,i,j,action,grid,alfa,lambda, nextAction, Astar, nextStatei, nextStatej);
								total_reward+=reward * gamma;
							}
							i--;
						}
						else
							nextAction = getAction(grid[i-1][j],eGreedy);
					}
					else
						nextAction = getAction(grid[i-1][j],eGreedy);
					actions++;
				break;
				
				//Ação DOWN escolhida
				case 1:

					if(i + 1 <= (number_lines-1))
					{
						if(grid[i+1][j].actions[action] != -999)
						{
							maxQValue = getMaxQValue(grid[i+1][j]);
							nextAction = getAction(grid[i+1][j],eGreedy);
							Astar = verifyMaiorAction(grid[i+1][j]);
							nextStatei = i+1;
							nextStatej = j;
							if(i+1 == 0 && j == 8)
							{
								elegibilityTraces(grid[i][j].actions[action],maxQValue,reward_goal,gamma,z,i,j,action,grid,alfa,lambda, nextAction, Astar, nextStatei, nextStatej);
								total_reward+=reward_goal * gamma;
							}
							else
							{
								elegibilityTraces(grid[i][j].actions[action],maxQValue,reward,gamma,z,i,j,action,grid,alfa,lambda, nextAction, Astar, nextStatei, nextStatej);
								total_reward+=reward * gamma;
							}
							i++;
						}
						else
							nextAction = getAction(grid[i+1][j],eGreedy);
					}
					else
						nextAction = getAction(grid[i+1][j],eGreedy);
					actions++;
				break;	

				//Ação LEFT escolhida
				case 2:

					if( j - 1 >= 0)
					{
						if(grid[i][j-1].actions[action] != -999)
						{
							maxQValue = getMaxQValue(grid[i][j-1]);
							nextAction = getAction(grid[i][j-1],eGreedy);
							Astar = verifyMaiorAction(grid[i][j-1]);
							nextStatei = i;
							nextStatej = j-1;
							if(i == 0 && j-1 == 8)
							{
								elegibilityTraces(grid[i][j].actions[action],maxQValue,reward_goal,gamma,z,i,j,action,grid,alfa,lambda, nextAction, Astar, nextStatei, nextStatej);
								total_reward+=reward_goal * gamma;
							}
							else
							{
								elegibilityTraces(grid[i][j].actions[action],maxQValue,reward,gamma,z,i,j,action,grid,alfa,lambda, nextAction, Astar, nextStatei, nextStatej);
								total_reward+=reward * gamma;
							}
							j--;
						}
						else
							nextAction = getAction(grid[i][j-1],eGreedy);
					}
					else
						nextAction = getAction(grid[i][j-1],eGreedy);
					actions++;
				break;	

				//Ação RIGHT escolhida
				case 3:

					if(j + 1 <= (number_col-1))
					{
						if(grid[i][j+1].actions[action] != -999)
						{
							maxQValue = getMaxQValue(grid[i][j+1]);
							nextAction = getAction(grid[i][j+1],eGreedy);
							Astar = verifyMaiorAction(grid[i][j+1]);
							nextStatei = i;
							nextStatej = j+1;
							if(i == 0 && j+1 == 8)
							{
								elegibilityTraces(grid[i][j].actions[action],maxQValue,reward_goal,gamma,z,i,j,action,grid,alfa,lambda, nextAction, Astar, nextStatei, nextStatej);
								total_reward+=reward_goal * gamma;
							}
							else
							{
								elegibilityTraces(grid[i][j].actions[action],maxQValue,reward,gamma,z,i,j,action,grid,alfa,lambda, nextAction, Astar, nextStatei, nextStatej);
								total_reward+=reward * gamma;
							}
							j++;
						}
						else
							nextAction = getAction(grid[i][j+1],eGreedy);
					}
					else
						nextAction = getAction(grid[i][j+1],eGreedy);
					actions++;
				break;	
			}
			
			action=nextAction;
			//verificação de episodio completo
			/*if(i == 0 && j == 8)
			{
				for(i = 0; i < number_lines; i++)
				{
					for(j = 0; j < number_col; j++)
					{
						for(k = 0; k < number_actions; k++)
						{
							z[i][j].actions[k] = 0;
						}
					}
				}
				count++;
				//gravarRecompensas(arq,total_reward);
				gravarAcoes(arq, actions, name);
				actions = 0;
				//total_reward = 0;
				i = 2;
				j = 0;
			}*/
			count++;
			gravarRecompensas(arq, total_reward, name);
		/*printf("%d\n", count);
		printf("%d\n", action);
		printf("%d\n", i);
		printf("%d\n", j);
		printf("\n");
		getch();*/
	}


	for(i = 0; i < number_lines; i++)
	{
		for(j = 0; j < number_col; j++)
		{
			for(k = 0; k < number_actions; k++)
			{
				if((i == 1 && j == 2) || (i == 2 && j == 2) || (i == 3 && j == 2))
					grid[i][j].actions[k] = -999;
				else if(i == 4 && j == 5)
					grid[i][j].actions[k] = -999;
				else if((i == 0 && j == 7) || (i == 1 && j == 7) || (i == 2 && j == 7))
					grid[i][j].actions[k] = -999;
				//else if(i == 0 && j == 8)
					//grid[i][j].actions[k] = 1;
				else
					grid[i][j].actions[k] = 0;

			}
		}
	}

	count = 0;
	i = 2;
	j = 0;
	actions = 0;
	total_reward = 0;
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
				printf("%.4f", grid[i][j].actions[0]);
				y += 1;
				gotoxy(x,y);
				printf("%.4f", grid[i][j].actions[1]);
				y += 1;
				gotoxy(x,y);
				printf("%.4f", grid[i][j].actions[2]);
				y += 1;
				gotoxy(x,y);
				printf("%.4f", grid[i][j].actions[3]);
				printf("  ");
				//getch();
				x = flag_x;
				y = flag_y;
			}
			else
			{
				gotoxy(x,y);
				//getch();
				printf("%.4f", grid[i][j].actions[0]);
				y += 1;
				gotoxy(x,y);
				printf("%.4f", grid[i][j].actions[1]);
				y += 1;
				gotoxy(x,y);
				printf("%.4f", grid[i][j].actions[2]);
				y += 1;
				gotoxy(x,y);
				printf("%.4f", grid[i][j].actions[3]);
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
	
	
	system("pause");
}