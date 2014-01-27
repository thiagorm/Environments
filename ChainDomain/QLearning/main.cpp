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
const float eGreedy = 0.8, gamma = 0.99, alfa = 0.8;
const float temperature = 0.3;

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
	struct State states[number_states];
	//O actual state começa com 0,pois é o estado inicial
	int actual_state = 0, next_state = 0, actions = 0;
	int count = 0, action = NULL, i = 0, j = 0;
	float  total_reward_to_go = 0;
	float maxQValue = 0;
	float total_reward = 0;
	int l = 0, teste = 0, acaoA = 0, acaoB = 0, maiorAcao = 0;
	int tr = 0;
	float somaQA = 0;
	float somaQB = 0;
	char name[80];
	char name2[80];
	int alcA1 = 0;
	int alcA2 = 0;
	int alcB = 0;
	
	for(i = 0; i < number_states; i++)
	{
		for(j = 0; j < number_actions; j++)
			states[i].actions[j] = 0;
	}
	
	srand ( time(NULL) );

	//for(l = 0; l < 8; l++)
	//{
	//strcpy(name, l+"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.txt");
	//strcpy(name2, l+"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb.txt");
	while(count != convergence_time)
	{
		action = getAction(states[actual_state], eGreedy);
		//action = getActionSoftMax(states[actual_state]);

		switch(action)
		{
			//Ação A escolhida
			case 0:
				if(actual_state == 4)
				{
					next_state = actual_state;
					maxQValue = getMaxQValue(states[next_state]);
					states[actual_state].actions[action] = qLearningCalculate(states[actual_state].actions[action], maxQValue, max_reward_a, alfa, gamma);
					total_reward += max_reward_a * pow(gamma, count);
					actions += max_reward_a;
					somaQA = states[actual_state].actions[action];
					alcA1++;
				}
				else
				{
					next_state = actual_state+1;
					maxQValue = getMaxQValue(states[next_state]);
					states[actual_state].actions[action] = qLearningCalculate(states[actual_state].actions[action], maxQValue, min_reward_a, alfa, gamma);
					total_reward += min_reward_a * pow(gamma, count);
					actions += min_reward_a;
					somaQA = states[actual_state].actions[action];
					alcA2++;
				}
			break;
				
			//Ação B escolhida
			case 1:
				//Na tabela de transição, qualquer estado que ler B volta para o primeiro estado
				next_state = 0;
				maxQValue = getMaxQValue(states[next_state]);
				states[actual_state].actions[action] = qLearningCalculate(states[actual_state].actions[action], maxQValue, reward_b, alfa, gamma);
				total_reward += reward_b * pow(gamma, count);
				actions += reward_b;
				somaQB = states[actual_state].actions[action];
				alcB++;
			break;	
		}

		/*if(actual_state == 4)
		{
			count++;
			gravarRecompensas(arq, total_reward, name);
			total_reward = 0;
			//gravarAcoes(arq, actions, name);
			//actions = 0;
		}*/

		actual_state = next_state;
		count++;


		//gravarRecompensas(arq, somaQA, name);
		//gravarRecompensas(arq, somaQB, name2);
		//gravarAcoes(arq, actions, name2);

	}

	//gravarAcoes(arq, actions, "arq.txt");
	
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
	teste = 0;
	somaQA = 0;
	somaQB = 0;

	srand ( time(NULL) );

	}*/
	
	
	printf("********************************************************************\n");
	printf("********************************************************************\n");
	
	for(i = 0; i < number_states; i++)
	{
		printf("Acoes do estado %d: A = %f; B = %f\n", i, states[i].actions[0], states[i].actions[1]);
		printf("----------------------------------------------------------------\n");
	}

	//printf("%d\n", actions);
	printf("%d\n", alcA1);
	printf("%d\n", alcA2);
	printf("%d\n", alcB);
	
	system("pause");
}