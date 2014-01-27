#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <iostream>
using namespace std;

#define number_actions 4
#define number_lines 6
#define number_col 9
#define convergence_time 1700

const float alfa = 0.1, gamma = 0.95, eGreedy = 0.1, n = 50;
const int reward = 0, reward_goal = 1;

void gotoxy(int x, int y)
{
COORD coord;
coord.X = x; coord.Y = y;
HANDLE hStdOut = GetStdHandle(STD_OUTPUT_HANDLE);
SetConsoleCursorPosition(hStdOut, coord);
}

struct Actions_Taken
{
	int actions[number_actions];
};

struct StateLocation
{
	int i;
	int j;
};

struct State
{
	//o indice 0 do vetor é a ação A e o indice 1 a ação B
	float actions[number_actions];
};

struct BaseModel
{
	float reward;
	struct StateLocation nextStateLocation;
};

struct Model
{
	struct BaseModel model[number_actions];
};


//gravação de recompensa por episodio
void gravarRecompensas(FILE *link, float reward)
{
	if((link = fopen("reward_perEpisode_qLearning.txt", "a+t")) == NULL)
	{
		printf("Erro ao abrir arquivo para gravação!");
		getch();
		exit(1);
	}

	fprintf(link,"%.2f\n",reward);

	fclose(link);
}

void gravarAcoes(FILE *link, int actions, char name[])
{
	if((link = fopen(name, "a+t")) == NULL)
	{
		printf("Erro ao abrir arquivo para gravação!");
		getch();
		exit(1);
	}

	fprintf(link,"%d\n",actions);

	fclose(link);
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
		if(s.actions[i] > maior && maior != s.actions[i])
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

float dynaQLearning(float QValue, float maxQValue, float reward, float alfa, float gamma)
{
	return (QValue + alfa * (reward + ((gamma * maxQValue) - QValue)));
}

void QPlanning(Model gridModel[][number_col], State grid[][number_col], Actions_Taken actions_taken[][number_col], int visited_location[][number_col], float alpha, float gamma)
{
	StateLocation location;
	struct Model local_gridModel;
	int actions[number_actions];
	int action = 0;
	int i = 0;
	int count = 0;
	int vl = 0;

	while(vl != 1)
	{
		location.i = ((rand()%number_lines));
		location.j = ((rand()%number_col));

		if(visited_location[location.i][location.j] == 1)
		{
			vl = 1;
			//printf("loci: %d\n", location.i);
			//printf("locj: %d\n", location.j);
		}
	}

	for(i = 0; i < number_actions; i++)
	{
		if(actions_taken[location.i][location.j].actions[i] == 1)
		{
			actions[count] = i;
			//printf("actions[%d]: %d\n", count, actions[count]);
			count++;
		}
	}

	//printf("count: %d\n", count);

	action = actions[((rand()%count))];

	//printf("action: %d\n", action);

	local_gridModel.model[action].reward = gridModel[location.i][location.j].model[action].reward;
	local_gridModel.model[action].nextStateLocation.i = gridModel[location.i][location.j].model[action].nextStateLocation.i;
	local_gridModel.model[action].nextStateLocation.j = gridModel[location.i][location.j].model[action].nextStateLocation.j;

	//printf("nextStateI: %d\n", local_model.nextStateLocation.i);
	//printf("nextStateJ: %d\n", local_model.nextStateLocation.j);

	//getch();

	grid[location.i][location.j].actions[action] = grid[location.i][location.j].actions[action] + alpha * (local_gridModel.model[action].reward + ((gamma*grid[local_gridModel.model[action].nextStateLocation.i][local_gridModel.model[action].nextStateLocation.j].actions[action]) - grid[location.i][location.j].actions[action]));

}

float getMaxQValue(struct State s)
{
	float maxQValue = 0;
	
	maxQValue = verifyMaiorValue(s);
	
	return maxQValue;
}

void main()
{
	
	FILE *arq = NULL;
	struct Model gridModel[number_lines][number_col];
	struct State grid[number_lines][number_col];
	struct Actions_Taken actions_taken[number_lines][number_col];
	int visited_location[number_lines][number_col];
	//O actual state começa com 0,pois é o estado inicial
	//int actual_state = 0, next_state = 0;
	int count = 0, action = NULL, i = 0, j = 0, k = 0, l = 0, actions = 0;
	float maxQValue = 0, total_reward = 0;
	int x = 0, y = 0, flag_y = 0, flag_x;
	int pn = 0;
	char name[80];

	
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

				actions_taken[i][j].actions[k] = 0;
				gridModel[i][j].model[k].nextStateLocation.i = 0;
				gridModel[i][j].model[k].nextStateLocation.j = 0;
				gridModel[i][j].model[k].reward = 0;
			}
			visited_location[i][j] = 0;
		}
	}
	
	srand ( time(NULL) );
	
	i = 2;
	j = 0;

	for(l = 0; l < 30; l++)
	{
	strcpy(name, l+"nnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnnn.txt");
	while(count != convergence_time)
	{
		action = getAction(grid[i][j], eGreedy);
		//printf("acao realizada: %d\n", action);
		//if(action != -1)
		//{
			switch(action)
			{
				//Ação UP escolhida
				case 0:
					//printf("acao realizada: %d\n", action);
					//printf("i:%d j:%d\n", i, j);
					if(i - 1 >= 0)
					{
						if(grid[i-1][j].actions[action] != -999)
						{
							maxQValue = getMaxQValue(grid[i-1][j]);
							if(i-1 == 0 && j == 8)
							{
								grid[i][j].actions[action] = dynaQLearning(grid[i][j].actions[action],maxQValue,reward_goal,alfa,gamma);
								gridModel[i][j].model[action].reward = reward_goal;
							}
							else
							{
								grid[i][j].actions[action] = dynaQLearning(grid[i][j].actions[action],maxQValue,reward,alfa,gamma);
								gridModel[i][j].model[action].reward = reward;
							}
							visited_location[i][j] = 1;//boolean de estados que já foram visitados: 0-nao visitado, 1-visitado
							actions_taken[i][j].actions[action] = 1;//boolean de acoes realizadas
							gridModel[i][j].model[action].nextStateLocation.i = i-1;
							gridModel[i][j].model[action].nextStateLocation.j = j;
							i--;
							for(pn = 0; pn < n; pn++)
								QPlanning(gridModel,grid,actions_taken,visited_location,alfa, gamma);
							total_reward+=reward;
						}
					}
				break;
				
				//Ação DOWN escolhida
				case 1:
					//printf("acao realizada: %d\n", action);
					//printf("i:%d j:%d\n", i, j);
					if(i + 1 <= (number_lines-1))
					{
						if(grid[i+1][j].actions[action] != -999)
						{
							maxQValue = getMaxQValue(grid[i+1][j]);
							if(i+1 == 0 && j == 8)
							{
								grid[i][j].actions[action] = dynaQLearning(grid[i][j].actions[action],maxQValue,reward_goal,alfa,gamma);
								gridModel[i][j].model[action].reward = reward_goal;
							}
							else 
							{
								grid[i][j].actions[action] = dynaQLearning(grid[i][j].actions[action],maxQValue,reward,alfa,gamma);
								gridModel[i][j].model[action].reward = reward;
							}
							visited_location[i][j] = 1;
							actions_taken[i][j].actions[action] = 1;
							gridModel[i][j].model[action].nextStateLocation.i = i+1;
							gridModel[i][j].model[action].nextStateLocation.j = j;
							i++;
							for(pn = 0; pn < n; pn++)
								QPlanning(gridModel,grid,actions_taken,visited_location,alfa, gamma);
							total_reward+=reward;
						}
					}
				break;	

				//Ação LEFT escolhida
				case 2:
					//printf("acao realizada: %d\n", action);
					//printf("i:%d j:%d\n", i, j);
					if( j - 1 >= 0)
					{
						if(grid[i][j-1].actions[action] != -999)
						{
							maxQValue = getMaxQValue(grid[i][j-1]);
							if(i == 0 && j-1 == 8)
							{
								grid[i][j].actions[action] = dynaQLearning(grid[i][j].actions[action],maxQValue,reward_goal,alfa,gamma);
								gridModel[i][j].model[action].reward = reward_goal;
							}
							else
							{
								grid[i][j].actions[action] = dynaQLearning(grid[i][j].actions[action],maxQValue,reward,alfa,gamma);
								gridModel[i][j].model[action].reward = reward;
							}
							visited_location[i][j] = 1;
							actions_taken[i][j].actions[action] = 1;
							gridModel[i][j].model[action].nextStateLocation.i = i;
							gridModel[i][j].model[action].nextStateLocation.j = j-1;
							j--;
							for(pn = 0; pn < n; pn++)
								QPlanning(gridModel,grid,actions_taken,visited_location,alfa, gamma);
							total_reward+=reward;
						}
					}
				break;	

				//Ação RIGHT escolhida
				case 3:
					//printf("acao realizada: %d\n", action);
					//printf("i:%d j:%d\n", i, j);
					if(j + 1 <= (number_col-1))
					{
						if(grid[i][j+1].actions[action] != -999)
						{
							maxQValue = getMaxQValue(grid[i][j+1]);
							if(i == 0 && j+1 == 8)
							{
								grid[i][j].actions[action] = dynaQLearning(grid[i][j].actions[action],maxQValue,reward_goal,alfa,gamma);
								gridModel[i][j].model[action].reward = reward_goal;
							}
							else
							{
								grid[i][j].actions[action] = dynaQLearning(grid[i][j].actions[action],maxQValue,reward,alfa,gamma);
								gridModel[i][j].model[action].reward = reward;
							}
							visited_location[i][j] = 1;
							actions_taken[i][j].actions[action] = 1;
							gridModel[i][j].model[action].nextStateLocation.i = i;
							gridModel[i][j].model[action].nextStateLocation.j = j+1;
							j++;
							for(pn = 0; pn < n; pn++)
								QPlanning(gridModel,grid,actions_taken,visited_location,alfa, gamma);
							total_reward+=reward;
						}
					}
				break;	
			}

			//for(pn = 0; pn < n; pn++)
					//QPlanning(gridModel,grid,actions_taken,visited_location,alfa, gamma);
			
			actions++;
			//verificação de episodio completo
			if(i == 0 && j == 8)
			{
				count++;
				i = 2;
				j = 0;
				//for(pn = 0; pn < n; pn++)
					//QPlanning(gridModel,grid,actions_taken,visited_location,alfa, gamma);
				//gravarRecompensas(arq,total_reward);
				gravarAcoes(arq, actions, name);
				actions = 0;
				//for(pn = 0; pn < n; pn++)
					//QPlanning(gridModel,grid,actions_taken,visited_location,alfa, gamma);
				//total_reward = 0;
			}
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

				actions_taken[i][j].actions[k] = 0;
				gridModel[i][j].model[k].nextStateLocation.i = 0;
				gridModel[i][j].model[k].nextStateLocation.j = 0;
				gridModel[i][j].model[k].reward = 0;
			}
			visited_location[i][j] = 0;
		}
	}

	count = 0;
	i = 2;
	j = 0;
	actions = 0;

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
				printf("%.2f", grid[i][j].actions[0]);
				y += 1;
				gotoxy(x,y);
				printf("%.2f", grid[i][j].actions[1]);
				y += 1;
				gotoxy(x,y);
				printf("%.2f", grid[i][j].actions[2]);
				y += 1;
				gotoxy(x,y);
				printf("%.2f", grid[i][j].actions[3]);
				printf("  ");
				//getch();
				x = flag_x;
				y = flag_y;
			}
			else
			{
				gotoxy(x,y);
				//getch();
				printf("%.2f", grid[i][j].actions[0]);
				y += 1;
				gotoxy(x,y);
				printf("%.2f", grid[i][j].actions[1]);
				y += 1;
				gotoxy(x,y);
				printf("%.2f", grid[i][j].actions[2]);
				y += 1;
				gotoxy(x,y);
				printf("%.2f", grid[i][j].actions[3]);
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