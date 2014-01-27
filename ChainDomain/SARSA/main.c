#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define number_actions 2
#define number_states 5
#define convergence_time 1000

const int max_reward_a = 10, min_reward_a = 0, reward_b = 2;
const float eGreedy = 1, gamma = 0.9, alfa = 0.9;

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
	float actions[2];
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
	int action = NULL;
	
	random_eGreedy = (float) (rand()/(float)RAND_MAX);
	
	//Se a probabilidade de selecionar uma ação for maior 0.2 a ação será randomica
	//senão se for a probabilidade inversa 1-p o algoritmo escolhera a melhor ação entre as duas existentes 
	if(random_eGreedy < eGreedy)
	{
		action = (int) ((rand()%2));
		//printf("Acao Random\n");
	}
	else
	{
		action = (int) verifyMaiorAction(s);
		//printf("Acao Maior\n");
	}
	
	return action;

}

float qLearningCalculate(float QValue, float nextQValue, float reward, float alfa, float gamma)
{
	return (QValue + alfa * (reward + ((gamma * nextQValue) - QValue)));
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
	struct State states[number_states];
	//O actual state começa com 0,pois é o estado inicial
	int actual_state = 0, next_state = 0;
	int count = 0, action = NULL, i = 0, j = 0, actions = 0;
	int nextAction = 0, l = 0;
	int total_reward = 0;
	char name[80];
	char name2[80];
	int alcA1 = 0;
	int alcA2 = 0;
	int alcB = 0;

	float AcaoA = 0;
	float AcaoB = 0;
	
	for(i = 0; i < number_states; i++)
	{
		for(j = 0; j < number_actions; j++)
			states[i].actions[j] = 0;
	}
	
	srand ( time(NULL) );
	
	action = getAction(states[actual_state], eGreedy);

	//for(l = 0; l < 10; l++)
	//{
	//strcpy(name, l+"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.txt");
	//strcpy(name2, l+"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb.txt");
	while(count != convergence_time)
	{
		switch(action)
		{
			//Ação A escolhida
			case 0:
				if(actual_state == 4)
				{
					next_state = actual_state;
					nextAction = getAction(states[next_state],eGreedy);
					states[actual_state].actions[action] = qLearningCalculate(states[actual_state].actions[action], states[next_state].actions[nextAction], max_reward_a, alfa, gamma);
					total_reward+= max_reward_a;// * pow(gamma, count);
					AcaoA += states[actual_state].actions[action];
					alcA1++;
				}
				else
				{
					next_state = actual_state+1;
					nextAction = getAction(states[next_state],eGreedy);
					states[actual_state].actions[action] = qLearningCalculate(states[actual_state].actions[action], states[next_state].actions[nextAction], min_reward_a, alfa, gamma);
					total_reward+= min_reward_a; //* pow(gamma, count);
					AcaoA += states[actual_state].actions[action];
					alcA2++;
				}
			break;
				
			//Ação B escolhida
			case 1:
				//Na tabela de transição, qualquer estado que ler B volta para o primeiro estado
				next_state = 0;
				nextAction = getAction(states[next_state],eGreedy);
				states[actual_state].actions[action] = qLearningCalculate(states[actual_state].actions[action], states[next_state].actions[nextAction], reward_b, alfa, gamma);
				total_reward+= reward_b;// * pow(gamma, count);
				AcaoB += states[actual_state].actions[action];
				alcB++;
			break;	
		}

		actual_state = next_state;
		action = nextAction;
		count++;

		//gravarRecompensas(arq, AcaoA, name);
		//gravarRecompensas(arq, AcaoB, name2);
		//gravarAcoes(arq, total_reward, name);
	}

	/*for(i = 0; i < number_states; i++)
	{
		for(j = 0; j < number_actions; j++)
			states[i].actions[j] = 0;
	}

	total_reward = 0;
	actions = 0;
	actual_state = 0;
	next_state = 0;
	count = 0;
	AcaoA = 0;
	AcaoB = 0;

	srand ( time(NULL) );

	action = getAction(states[actual_state], eGreedy);
	}*/
	
	
	printf("********************************************************************\n");
	printf("********************************************************************\n");
	
	for(i = 0; i < number_states; i++)
	{
		printf("Acoes do estado %d: A = %.2f; B = %.2f\n", i, states[i].actions[0], states[i].actions[1]);
		printf("----------------------------------------------------------------\n");
	}
	printf("%d\n", alcA1);
	printf("%d\n", alcA2);
	printf("%d\n", alcB);
	
	system("pause");
}