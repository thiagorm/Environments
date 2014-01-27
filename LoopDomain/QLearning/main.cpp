#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <string.h>
#include <math.h>

#define number_actions 2
#define number_states 9
#define convergence_time 1000

const float max_reward = 2, reward = 0, min_reward = 1;
const float eGreedy = 0.2, gamma = 0.9, alfa = 0.9;

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
	
	//Se a probabilidade de selecionar uma ação for maior 0.2 a ação será randomica
	//senão se for a probabilidade inversa 1-p o algoritmo escolhera a melhor ação entre as duas existentes 
	if(random_eGreedy < eGreedy)
	{
		action = ((rand()%2));
		//printf("Acao Random\n");
	}
	else
	{
		action = verifyMaiorAction(s);
		//printf("Acao Maior\n");
	}
	
	return action;

}

float qLearningCalculate(float Qvalue, float maxQValue, float reward, float alfa, float gamma)
{
	return (Qvalue + alfa * (reward + ((gamma * maxQValue) - Qvalue)));
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
	int count = 0, action = NULL, i = 0, j = 0;
	float maxQValue = 0;
	int l = 0, total_reward = 0;
	char name[80];
	char name2[80];

	float AcaoA = 0;
	float AcaoB = 0;

	for(i = 0; i < number_states; i++)
	{
		for(j = 0; j < number_actions; j++)
			states[i].actions[j] = 0;
	}
	
	srand ( time(NULL) );
	
	
	//for(l = 0; l < 8; l++)
	//{
	//strcpy(name, l+"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.txt");
	//strcpy(name2, l+"bbbbbbbbbbbbbbbbbbbbbbbbbbbbbbbb.txt");
	while(count != convergence_time)
	{
		action = getAction(states[actual_state], eGreedy);
		
		
		//Se o estado atual for zero e ação escolhida for "A"
		if(actual_state == 0 && action == 0)
		{
			next_state = actual_state+1;
			maxQValue = getMaxQValue(states[next_state]);
			states[actual_state].actions[action] = qLearningCalculate(states[actual_state].actions[action], maxQValue, reward, alfa, gamma);
			total_reward += reward; //* pow(gamma, count); 
			AcaoA = states[actual_state].actions[action];
		}
		//Se o estado atual for zero e a ação escolhoda for "B"
		else if(actual_state == 0 && action == 1)
		{
			next_state = actual_state+5;
			maxQValue = getMaxQValue(states[next_state]);
			states[actual_state].actions[action] = qLearningCalculate(states[actual_state].actions[action], maxQValue, reward, alfa, gamma);
			total_reward += reward; //* pow(gamma, count); 
			AcaoB = states[actual_state].actions[action];
		}
		//Se for qualquer estado do loop direito ou o loop dos estados entre 1 e 4 faça
		else if(actual_state == 1 || actual_state == 2 || actual_state == 3 || actual_state == 4)
		{
			if(actual_state == 4)
			{
				next_state = 0;
				maxQValue = getMaxQValue(states[next_state]);
				states[actual_state].actions[0] = qLearningCalculate(states[actual_state].actions[0], maxQValue, min_reward, alfa, gamma);
				states[actual_state].actions[1] = qLearningCalculate(states[actual_state].actions[1], maxQValue, min_reward, alfa, gamma);
				total_reward += min_reward; //* pow(gamma, count); 
				AcaoA = states[actual_state].actions[0];
				AcaoB = states[actual_state].actions[1];
			}
			else
			{
				next_state = actual_state+1;
				maxQValue = getMaxQValue(states[next_state]);
				states[actual_state].actions[0] = qLearningCalculate(states[actual_state].actions[0], maxQValue, reward, alfa, gamma);
				states[actual_state].actions[1] = qLearningCalculate(states[actual_state].actions[1], maxQValue, reward, alfa, gamma);
				total_reward += reward; //* pow(gamma, count); 
				AcaoA = states[actual_state].actions[0];
				AcaoB = states[actual_state].actions[1];
			}
				
				
		}
		//Se for qualquer estado do loop esquerdo ou o loop dos estados entre 5 e 8
		else if(actual_state == 5 || actual_state == 6 || actual_state == 7 || actual_state == 8)
		{
				
			if(actual_state == 8)
			{
				next_state = 0;
				maxQValue = getMaxQValue(states[next_state]);
				states[actual_state].actions[0] = qLearningCalculate(states[actual_state].actions[0], maxQValue, max_reward, alfa, gamma);
				states[actual_state].actions[1] = qLearningCalculate(states[actual_state].actions[1], maxQValue, max_reward, alfa, gamma);
				total_reward += max_reward; //* pow(gamma, count); 
				AcaoA = states[actual_state].actions[0];
				AcaoB = states[actual_state].actions[1];
			}
			else
			{
				if(action == 0)
				{
					next_state = 0;
					maxQValue = getMaxQValue(states[next_state]);
					states[actual_state].actions[action] = qLearningCalculate(states[actual_state].actions[action], maxQValue, reward, alfa, gamma);
					total_reward += reward; //* pow(gamma, count); 
					AcaoA = states[actual_state].actions[action];
				}
				else if(action == 1)
				{
					next_state = actual_state+1;
					maxQValue = getMaxQValue(states[next_state]);
					states[actual_state].actions[action] = qLearningCalculate(states[actual_state].actions[action], maxQValue, reward, alfa, gamma);
					total_reward += reward; //* pow(gamma, count); 
					AcaoB = states[actual_state].actions[action];
				}
			}
				
		}
		
		/*if(actual_state == 8)
		{
			count++;
			actual_state = 0;
			next_state = 0;
			//gravarRecompensas(arq,total_reward);
			//gravarAcoes(arq, actions);
			//actions = 0;
			//total_reward = 0;
		}*/

		actual_state = next_state;
		count++;
		//getch();
		//gravarAcoes(arq, total_reward, name);
		//gravarRecompensas(arq, AcaoA, name);
		//gravarRecompensas(arq, AcaoB, name2);
	}

	
	/*for(i = 0; i < number_states; i++)
	{
		for(j = 0; j < number_actions; j++)
			states[i].actions[j] = 0;
	}

	total_reward = 0;
	actual_state = 0;
	next_state = 0;
	count = 0;
	AcaoA = 0;
	AcaoB = 0;

	srand ( time(NULL) );
		
	}*/
	
	
	printf("********************************************************************\n");
	printf("********************************************************************\n");
	
	for(i = 0; i < number_states; i++)
	{
		printf("Acoes do estado %d: A = %f; B = %f\n", i, states[i].actions[0], states[i].actions[1]);
		printf("----------------------------------------------------------------\n");
	}
	
	
	getch();
}