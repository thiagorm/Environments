#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <windows.h>
#include <iostream>
#include <string.h>
#include <cmath>
using namespace std;

#define number_actions 4
#define number_lines 6
#define number_col 9
#define convergence_time 1700

const float eGreedy = 0.2, gamma = 0.1, alfa = 0.1;
const float delta = (1/number_actions), sigma = 0.3;
const float temperature = 0.05;
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
		if(s.actions[i] > maior)
		{
			maior = s.actions[i];
			action = i;
			countMax = 0;
			numberActions[countMax] = action;
			boolean = 0;
		}
		else if(s.actions[i] == maior)
		{
			countMax++;
			action = i;
			numberActions[countMax] = action;
			boolean = 1;
		}
	}

	countMax++;

	if(boolean == 1)
	{
		int randomIndex = ((rand()%countMax));
		action = numberActions[randomIndex];
	}

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



int getActionEgreedyVDBE(struct State epsilonValues[][number_col], int i, int j, int a, State s)
{

	float epsilon = epsilonValues[i][j].actions[a];

	float random_eGreedy = 0;
	int action = NULL;
	
	random_eGreedy = (float) rand()/(float)RAND_MAX;

	if(random_eGreedy > epsilon)
	{
		action = verifyMaiorAction(s);
		//printf("Acao Maior\n");
	}
	else
	{
		action = ((rand()%4));
		//printf("Acao Random\n");
	}
	
	return action;
}

void calculateEpsilonValue(float QValue, float nextQValue, struct State epsilonValues[][number_col], int i, int j, int action)
{
	float epsilonValue = epsilonValues[i][j].actions[action];

	float deltaQ = exp((-(abs(nextQValue - QValue)))/sigma);
	float FQ = (1.0 - deltaQ) / (1.0 + deltaQ);
	float epsilon = delta * FQ + (1.0 - delta) * epsilonValue;

	epsilonValues[i][j].actions[action] = epsilon;
}


int getActionSoftMax(State s)
{

	/*printf("s[0]: %.2f\n", s.actions[0]);
	printf("s[1]: %.2f\n", s.actions[1]);
	printf("s[2]: %.2f\n", s.actions[2]);
	printf("s[3]: %.2f\n", s.actions[3]);*/

	double prob[number_actions];

	double sumProb = 0;
	float randomAction = 0;
	double offset = 0;

	for(int i = 0; i < number_actions; i++)
	{
		prob[i] = exp(s.actions[i]/temperature);
		sumProb += prob[i];
	}

	for(int i = 0; i < number_actions; i++)
	{
		prob[i] = prob[i] / sumProb;
	}

	randomAction = (float) rand()/(float)RAND_MAX;

	/*printf("random action: %.2f\n", randomAction);
	printf("sumProb: %.2f\n", sumProb);
	printf("offset: %.2f\n", offset);
	printf("prob[0]: %.2f\n", prob[0]);
	printf("prob[1]: %.2f\n", prob[1]);
	printf("prob[2]: %.2f\n", prob[2]);
	printf("prob[3]: %.2f\n", prob[3]);*/
	//getch();

	for(int i = 0; i < number_actions; i++)
	{
		if((randomAction >= offset) && (randomAction <= (offset + prob[i])))
		{
			return i;
		}
		offset += prob[i];
	}

	return -1;
}

float qLearningCalculate(float QValue, float maxQValue, float reward, float alfa, float gamma)
{
	return (QValue + alfa * (reward + ((gamma * maxQValue) - QValue)));
	//return (1-alfa) * (actual_state + alfa * (reward + (gamma * maxQValue)));
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
	struct State grid[number_lines][number_col];
	struct State epsilonValues[number_lines][number_col];
	//O actual state começa com 0,pois é o estado inicial
	//int actual_state = 0, next_state = 0;
	int count = 0, action = NULL, i = 0, j = 0, k = 0, l = 0, actions = 0, total_reward = 0;
	float maxQValue = 0;
	int x = 0, y = 0, flag_y = 0, flag_x;
	
	char name[80];
	char name2[80];
	char name3[80];
	char name4[80], n = 0;

	float AcaoUp = 0, AcaoDown = 0, AcaoLeft = 0, AcaoRight = 0;
	
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
				else if(i == 0 && j == 8)
					grid[i][j].actions[k] = 0;
				else
					grid[i][j].actions[k] = 0;

			}
		}
	}
	
	srand ( time(NULL) );
	
	i = 2;
	j = 0;

	for(l = 0; l < 5; l++)
	{
	strcpy(name, l+"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.txt");
	strcpy(name2, l+"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb.txt");
	strcpy(name3, l+"ccccccccccccccccccccccccccccccccc.txt");
	strcpy(name4, l+"dddddddddddddddddddddddddddddddddd.txt");
	while(count != convergence_time)
	{
		//action = getAction(grid[i][j], eGreedy);
		
		//action = getActionEgreedyVDBE(epsilonValues, i, j, action, grid[i][j]);

		action = getAction(grid[i][j], eGreedy);

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

							//calculateEpsilonValue(grid[i][j].actions[action], grid[i-1][j].actions[action], epsilonValues, i, j, action);

							maxQValue = getMaxQValue(grid[i-1][j]);
							if(i-1 == 0 && j == 8)
							{
								grid[i][j].actions[action] = qLearningCalculate(grid[i][j].actions[action],maxQValue,reward_goal,alfa,gamma);
								total_reward+=reward_goal; //* pow(gamma, count);
								AcaoUp += grid[i][j].actions[action];
							}
							else
							{
								grid[i][j].actions[action] = qLearningCalculate(grid[i][j].actions[action],maxQValue,reward,alfa,gamma);
								total_reward+=reward; //* pow(gamma, count);
								AcaoUp += grid[i][j].actions[action];
							}
							i--;
						}
					}
					actions++;
				break;
				
				//Ação DOWN escolhida
				case 1:

					if(i + 1 <= (number_lines-1))
					{
						if(grid[i+1][j].actions[action] != -999)
						{
							//calculateEpsilonValue(grid[i][j].actions[action], grid[i+1][j].actions[action], epsilonValues, i, j, action);

							maxQValue = getMaxQValue(grid[i+1][j]);
							if(i+1 == 0 && j == 8)
							{
								grid[i][j].actions[action] = qLearningCalculate(grid[i][j].actions[action],maxQValue,reward_goal,alfa,gamma);
								total_reward+=reward_goal;// * pow(gamma, count);
								AcaoDown += grid[i][j].actions[action];
							}
							else 
							{
								grid[i][j].actions[action] = qLearningCalculate(grid[i][j].actions[action],maxQValue,reward,alfa,gamma);
								total_reward+=reward;// * pow(gamma, count);
								AcaoDown += grid[i][j].actions[action];
							}
							i++;
						}
					}
					actions++;
				break;	

				//Ação LEFT escolhida
				case 2:

					if( j - 1 >= 0)
					{
						if(grid[i][j-1].actions[action] != -999)
						{
							//calculateEpsilonValue(grid[i][j].actions[action], grid[i][j-1].actions[action], epsilonValues, i, j, action);

							maxQValue = getMaxQValue(grid[i][j-1]);
							if(i == 0 && j-1 == 8)
							{
								grid[i][j].actions[action] = qLearningCalculate(grid[i][j].actions[action],maxQValue,reward_goal,alfa,gamma);
								total_reward+=reward_goal;// * pow(gamma, count);
								AcaoLeft += grid[i][j].actions[action];
							}
							else
							{
								grid[i][j].actions[action] = qLearningCalculate(grid[i][j].actions[action],maxQValue,reward,alfa,gamma);
								total_reward+=reward;// * pow(gamma, count);
								AcaoLeft += grid[i][j].actions[action];
							}
							j--;
						}
					}
					actions++;
				break;	

				//Ação RIGHT escolhida
				case 3:

					if(j + 1 <= (number_col-1))
					{
						if(grid[i][j+1].actions[action] != -999)
						{
							//calculateEpsilonValue(grid[i][j].actions[action], grid[i][j+1].actions[action], epsilonValues, i, j, action);

							maxQValue = getMaxQValue(grid[i][j+1]);
							if(i == 0 && j+1 == 8)
							{
								grid[i][j].actions[action] = qLearningCalculate(grid[i][j].actions[action],maxQValue,reward_goal,alfa,gamma);
								total_reward+=reward_goal;// * pow(gamma, count);
								AcaoRight += grid[i][j].actions[action];
							}
							else
							{
								grid[i][j].actions[action] = qLearningCalculate(grid[i][j].actions[action],maxQValue,reward,alfa,gamma);
								total_reward+=reward;// * pow(gamma, count);
								AcaoRight += grid[i][j].actions[action];
							}
							j++;
						}
					}
					actions++;
				break;	
			}
			
			//verificação de episodio completo
			if(i == 0 && j == 8)
			{
				//count++;
				i = 2;
				j = 0;
				count++;
				//gravarAcoes(arq, actions, name);
				//actions = 0;
				//total_reward = 0;
			}
			//gravarAcoes(arq, total_reward, name);
			gravarRecompensas(arq, AcaoUp, name);
			gravarRecompensas(arq, AcaoDown, name2);
			gravarRecompensas(arq, AcaoLeft, name3);
			gravarRecompensas(arq, AcaoRight, name4);
			//printf("%d\n", count);
		//}
		/*printf("%d\n", count);
		printf("%d\n", action);
		printf("%d\n", i);
		printf("%d\n", j);
		printf("\n");
		if(action == -1)
		getch();*/
		//printf("%d\n", count);
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
				else if(i == 0 && j == 8)
					grid[i][j].actions[k] = 1;
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
	AcaoUp = 0;
	AcaoDown = 0;
	AcaoRight = 0;
	AcaoLeft = 0;

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