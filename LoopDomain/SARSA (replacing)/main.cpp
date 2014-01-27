#include <stdio.h>
#include <stdlib.h>
#include <conio.h>
#include <time.h>
#include <string.h>

#define number_actions 2
#define number_states 9
#define convergence_time 3000

float const max_reward = 2, reward = 0, min_reward = 1;
//com alfa sendo 1, pode-se entender as ações como deterministicas
float alfa = 1, gamma = 0.99, eGreedy = 0.8, lambda = 0.1;

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

void elegibilityTraces(float QValue, float reward, float gamma, struct State z[], int actual_state, int action, struct State grid[], float alfa, float lambda, int nextAction,
	int next_state)
{
	float delta = 0;
	int m = 0;
	int n = 0;
	int k = 0;

	//printf("AStar:%d\n", AStar);
	//printf("nextAction:%d\n", nextAction);
	//getch();
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

	//if(grid[3].actions[0] < -0.1 || grid[4].actions[0] < -0.1)
	//{
	/*printf("Action: %d\n", action);
	printf("actual_state: %d\n", actual_state);
	printf("next_state: %d\n", next_state);
	printf("nextAction: %d\n", nextAction);
	printf("AStar: %d\n", AStar);
	for(m = 0; m < number_states; m++)
	{
		printf("Acoes do estado %d: A = %.2f; B = %.2f\n", m, grid[m].actions[0], grid[m].actions[1]);
		printf("----------------------------------------------------------------\n");
	}
	printf("\n");

	getch();*/
	//}

	//printf("\\\\\\\\\\\\\\\\\\\\\\\n");
	//getch();
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
	int count = 0, action = NULL, i = 0, j = 0, nextAction = 0;
	int l = 0,  total_reward = 0;
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
		//Se o estado atual for zero e ação escolhida for "A"
		if(actual_state == 0 && action == 0)
		{
			next_state = actual_state+1;
			nextAction = getAction(states[next_state],eGreedy);
			elegibilityTraces(states[actual_state].actions[action],reward,gamma,z,actual_state,action,states,alfa,lambda,nextAction,next_state);
			total_reward += reward;
		}
		//Se o estado atual for zero e a ação escolhoda for "B"
		else if(actual_state == 0 && action == 1)
		{
			next_state = actual_state+5;
			nextAction = getAction(states[next_state],eGreedy);
			elegibilityTraces(states[actual_state].actions[action],reward,gamma,z,actual_state,action,states,alfa,lambda,nextAction,next_state);
			total_reward += reward;
		}
		//Se for qualquer estado do loop direito ou o loop dos estados entre 1 e 4 faça
		else if(actual_state == 1 || actual_state == 2 || actual_state == 3 || actual_state == 4)
		{
			if(actual_state == 4)
			{
				next_state = 0;
				nextAction = getAction(states[next_state],eGreedy);
				elegibilityTraces(states[actual_state].actions[0],min_reward,gamma,z,actual_state,0,states,alfa,lambda,nextAction,next_state);
				elegibilityTraces(states[actual_state].actions[1],min_reward,gamma,z,actual_state,1,states,alfa,lambda,nextAction,next_state);
				total_reward += min_reward;
			}
			else
			{
				next_state = actual_state+1;
				nextAction = getAction(states[next_state],eGreedy);
				elegibilityTraces(states[actual_state].actions[0],reward,gamma,z,actual_state,0,states,alfa,lambda,nextAction,next_state);
				elegibilityTraces(states[actual_state].actions[1],reward,gamma,z,actual_state,1,states,alfa,lambda,nextAction,next_state);
				total_reward += reward;
			}
				
				
		}
		//Se for qualquer estado do loop esquerdo ou o loop dos estados entre 5 e 8
		else if(actual_state == 5 || actual_state == 6 || actual_state == 7 || actual_state == 8)
		{
				
			if(actual_state == 8)
			{
				next_state = 0;
				nextAction = getAction(states[next_state],eGreedy);
				elegibilityTraces(states[actual_state].actions[0],max_reward,gamma,z,actual_state,0,states,alfa,lambda,nextAction,next_state);
				elegibilityTraces(states[actual_state].actions[1],max_reward,gamma,z,actual_state,1,states,alfa,lambda,nextAction,next_state);
				total_reward += max_reward;
			}
			else
			{
				if(action == 0)
				{
					next_state = 0;
					nextAction = getAction(states[next_state],eGreedy);
					elegibilityTraces(states[actual_state].actions[action],reward,gamma,z,actual_state,action,states,alfa,lambda,nextAction,next_state);
					total_reward += reward;
				}
				else if(action == 1)
				{
					next_state = actual_state+1;
					nextAction = getAction(states[next_state],eGreedy);
					elegibilityTraces(states[actual_state].actions[action],reward,gamma,z,actual_state,action,states,alfa,lambda,nextAction,next_state);
					total_reward += reward;
				}
			}
				
		}
		
		/*if(actual_state == 8)
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
		//getch();
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
		printf("Acoes do estado %d: A = %f; B = %f\n", i, states[i].actions[0], states[i].actions[1]);
		printf("----------------------------------------------------------------\n");
	}
	
	
	getch();
}