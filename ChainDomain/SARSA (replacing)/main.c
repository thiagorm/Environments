#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <string.h>

#define number_actions 2
#define number_states 5
#define convergence_time 3000

const int max_reward_a = 10, min_reward_a = 0, reward_b = 2;
const float alfa = 0.8, gamma = 0.99, eGreedy = 0.8, lambda = 0.1;

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

void elegibilityTraces(float QValue, float reward, float gamma, struct State z[], int actual_state, int action, struct State grid[], float alfa, float lambda, int nextAction,
	int next_state)
{
	float delta = 0;
	int m = 0;
	int n = 0;
	int k = 0;

	delta = reward + ((gamma * grid[next_state].actions[nextAction]) - QValue);
	z[actual_state].actions[action] = 1;

	for(n = 0; n < number_states; n++)
	{
		for(k = 0; k < number_actions; k++)
		{
			grid[n].actions[k] = grid[n].actions[k] + alfa * delta * z[n].actions[k]; 

			z[n].actions[k] = gamma * lambda * z[n].actions[k];
		}
	}
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
	struct State z[number_states];
	//O actual state começa com 0,pois é o estado inicial
	int actual_state = 0, next_state = 0;
	int count = 0, action = NULL, i = 0, j = 0, actions = 0, Astar = 0, nextAction = 0;
	int total_reward = 0, l = 0;
	char name[80];

	
	for(i = 0; i < number_states; i++)
	{
		for(j = 0; j < number_actions; j++)
		{
			states[i].actions[j] = 0;
			z[i].actions[j] = 0;
		}
	}
	
	srand ( time(NULL) );
	
	action = getAction(states[actual_state], eGreedy);

	for(l = 0; l < 20; l++)
	{
	strcpy(name, l+"aaaaaaaaaaaaaaaaaaaaaaaaaaaaaaa.txt");
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
					elegibilityTraces(states[actual_state].actions[action],max_reward_a,gamma,z,actual_state,action,states,alfa,lambda,nextAction,next_state);
					total_reward+= max_reward_a;
				}
				else
				{
					next_state = actual_state+1;
					nextAction = getAction(states[next_state],eGreedy);
					elegibilityTraces(states[actual_state].actions[action],min_reward_a,gamma,z,actual_state,action,states,alfa,lambda,nextAction,next_state);
					total_reward+= min_reward_a;
				}
			break;
				
			//Ação B escolhida
			case 1:
				//Na tabela de transição, qualquer estado que ler B volta para o primeiro estado
				next_state = 0;
				nextAction = getAction(states[next_state],eGreedy);
				elegibilityTraces(states[actual_state].actions[action], reward_b,gamma,z,actual_state,action,states,alfa,lambda,nextAction,next_state);
				total_reward+= reward_b;
			break;	
		}
			
		//verificação de episodio completo
		/*if(actual_state == 4)
		{
			for(i = 0; i < number_states; i++)
			{
				for(j = 0; j < number_actions; j++)
				{
					z[i].actions[j] = 0;
				}
			}
			count++;
			actual_state = 0;
			next_state = 0;
			//gravarRecompensas(arq,total_reward);
			//gravarAcoes(arq, actions);
			//actions = 0;
			//total_reward = 0;
		}*/

		actual_state = next_state;
		action = nextAction;
		count++;
		gravarAcoes(arq, total_reward, name);
	}

	for(i = 0; i < number_states; i++)
	{
		for(j = 0; j < number_actions; j++)
		{
			states[i].actions[j] = 0;
			z[i].actions[j] = 0;
		}
	}

	total_reward = 0;
	actions = 0;
	actual_state = 0;
	next_state = 0;
	count = 0;

	srand ( time(NULL) );

	action = getAction(states[actual_state], eGreedy);
	}
	
	
	printf("********************************************************************\n");
	printf("********************************************************************\n");
	
	for(i = 0; i < number_states; i++)
	{
		printf("Acoes do estado %d: A = %.2f; B = %.2f\n", i, states[i].actions[0], states[i].actions[1]);
		printf("----------------------------------------------------------------\n");
	}
	
	
	system("pause");
}