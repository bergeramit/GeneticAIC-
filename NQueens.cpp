// Genetic5.cpp : Defines the entry point for the console application.
//


#include <iostream>					// for cout etc.
#include <vector>					// for vector class
#include <string>					// for string class
#include <algorithm>				// for sort algorithm
#include <time.h>					// for random seed
#include <math.h>					// for abs()

#define GA_POPSIZE		2048		// ga population size
#define GA_MAXITER		16384		// maximum iterations
#define GA_ELITRATE		0.10f		// elitism rate
#define GA_MUTATIONRATE	0.25f		// mutation rate
#define GA_MUTATION		RAND_MAX * GA_MUTATIONRATE
#define GA_TARGET		std::string("Hello world!")
#define GA_NQueensNum 20
#define GA_ageT 10 //5
using namespace std;				// polluting global namespace, but hey...

struct ga_struct
{
	int str[GA_NQueensNum];						// the string
	unsigned int fitness;			// its fitness
    int age;
};

typedef vector<ga_struct> ga_vector;// for brevity

void init_population(ga_vector &population,
					 ga_vector &buffer )
{
    int randnum[GA_NQueensNum];
    int random;
    int switcher=0;
    int saveTop = GA_NQueensNum;
	for (int i=0; i<GA_POPSIZE; i++) {
        //cout << "number of gene " << i << "\n";
		ga_struct citizen;
		citizen.fitness = 0;
        citizen.age = 0;
		//random populate
		for(int j=0;j<GA_NQueensNum;++j){
            randnum[j]=j+1;
		}
		saveTop = GA_NQueensNum;
		//acually getting random permutatiuon
		for(int j=0;j<GA_NQueensNum;++j){
            random = rand()%saveTop;
            citizen.str[j]= randnum[random];
            if(random == saveTop-1){
                saveTop--;
            }else{
                switcher = randnum[random];
                randnum[random] = randnum[saveTop-1];
                randnum[saveTop-1] = switcher;
                saveTop--;
            }
		}

		population.push_back(citizen);
	}

	buffer.resize(GA_POPSIZE);
}
bool isSafe(ga_struct &board, int col)
{
    int i;
    /* Check this row on left side
    for (i = 0; i < GA_NQueensNum; i++){
        if (board.str[col] == board.str[i] && i != col){
            return false;
        }
    }*/
    for (i = col+1; i < GA_NQueensNum; i++)
        if ((board.str[col]+i-col == board.str[i] || board.str[col]-i+col == board.str[i]) && board.str[col]-i+col>0 && board.str[col]+i-col>0){
            return false;
        }

    for (i = col-1; i >=0; i--)
        if ((board.str[col]+i-col == board.str[i] || board.str[col]-i+col == board.str[i]) && board.str[col]-i+col>0 && board.str[col]+i-col>0){
            return false;
        }
    return true;
}

void calc_fitness(ga_vector &population)
{
	unsigned int fitness=0;
	for (int i=0; i<GA_POPSIZE; i++) {
	    fitness = 0;
        //cout << "\n gene " << i << " ";
        //for (int j=0; j<GA_NQueensNum; j++){
         //cout << population[i].str[j] << " ";
        //}
        for (int j=0; j<GA_NQueensNum; j++){
            if(!isSafe(population[i],j)){
                fitness+=10;
            }
        }
        //cout << "fitness " << fitness;
		population[i].fitness = fitness;
	}
}

bool fitness_sort(ga_struct x, ga_struct y)
{ return (x.fitness < y.fitness); }

inline void sort_by_fitness(ga_vector &population)
{ sort(population.begin(), population.end(), fitness_sort); }

int elitism(ga_vector &population,
				ga_vector &buffer, int esize )
{
    int count=0;
	for (int i=0; i<esize; i++) {
        if(population[i].age <= GA_ageT ){
            for(int j=0;j<GA_NQueensNum;++j){
                buffer[i].str[j] = population[i].str[j];
            }
		buffer[i].fitness = population[i].fitness;
        buffer[i].age = population[i].age+1;
        count++;
        }
	}
    return count;
}

void mutate(ga_struct &member)
{
	int firstindexRand = rand()% (GA_NQueensNum);
	int secondindexRand = rand()% (GA_NQueensNum);
	int switcher = member.str[firstindexRand];
	member.str[firstindexRand] = member.str[secondindexRand];
	member.str[secondindexRand] = switcher;
    member.age = 0;
}

void mutate_inverse(ga_struct &member)
{
	int firstindexRand = rand()% (GA_NQueensNum);
	int secondindexRand = rand()% (GA_NQueensNum- firstindexRand);
	secondindexRand= firstindexRand+secondindexRand;
    //int vec_size = secondindexRand-firstindexRand+1;
    vector<int> inverse;
    for(int i = secondindexRand; i>= firstindexRand; i--)
    {
        inverse.push_back(member.str[i]);
    }

    int j=0;
    for(int i = firstindexRand; i <= secondindexRand; i++)
    {
        member.str[i] = inverse[j];
        j++;
    }
    member.age=0;

}

void elitismNoAge(ga_vector &population,
				ga_vector &buffer, int esize )
{
	for (int i=0; i<esize; i++) {
            for(int j=0;j<GA_NQueensNum;++j){
                buffer[i].str[j] = population[i].str[j];
            }
		buffer[i].fitness = population[i].fitness;
        buffer[i].age = population[i].age;
        }
}





void mate(ga_vector &population, ga_vector &buffer)
{
	int esize = GA_POPSIZE * GA_ELITRATE;
	int i1, i2;
    int numOfQueensToRandom=0;
	int numofgenessuccess = elitism(population, buffer, esize);
    ga_vector RWS_Scale;
    RWS_Scale.resize(GA_POPSIZE);
    int w=0;
    elitismNoAge(population,RWS_Scale,GA_POPSIZE);
    double sum=0;
    double RWSfittness[GA_POPSIZE];
    for(int i=0;i<GA_POPSIZE;++i){
        double save = RWS_Scale[i].fitness+1;
        RWSfittness[i] = sqrt(1/save);
        sum += RWSfittness[i];
        //cout << "check scale "<< save << " \n";
    }
    //cout << "Sum: "<< sum << " \n";
    for(int i=0;i<GA_POPSIZE;++i){
        RWSfittness[i] = (RWSfittness[i]/sum)*100;
        //cout << "for board "<< i << "we give precentage: " << RWSfittness[i] <<"% \n";
    }
	// Mate the rest
	for (int i = numofgenessuccess; i<GA_POPSIZE; i++) {
		//i1 = rand() % (GA_POPSIZE / 2);
		//i2 = rand() % (GA_POPSIZE / 2);
        //while not same spicies
        int randomgene[2];
    for(int z=0;z<2;++z){
        int randum = rand()%101;
        double sum=0;
        for(int q=0;q<GA_POPSIZE;++q){
            sum += RWSfittness[q];
            if(randum-sum <=0){
                randomgene[z] = q;
                break;
            }
        }
    }
    i1 = randomgene[0];
	i2 = randomgene[1];


		int randnum[GA_NQueensNum];
        int XO[GA_NQueensNum/2];
        for(int j=0;j<GA_NQueensNum;++j){
            randnum[j]=j+1;
		}
		int saveTop = GA_NQueensNum;
		int random,switcher;
		for(int j=0;j<GA_NQueensNum/2;++j){
            random = rand()%saveTop;
            XO[j]= randnum[random];
            if(random == saveTop-1){
                saveTop--;
            }else{
                switcher = randnum[random];
                randnum[random] = randnum[saveTop-1];
                randnum[saveTop-1] = switcher;
                saveTop--;
            }
		}


		for(int z=0; z<GA_NQueensNum; ++z){
            buffer[i].str[z] = -1;
		}

        for(int j=0; j<GA_NQueensNum/2; ++j){
            for(int z=0; z<GA_NQueensNum; ++z){
                if(XO[j] == population[i1].str[z]){
                    buffer[i].str[z] = XO[j];
                }
            }
        }

        for(int j=0;j<GA_NQueensNum;++j){
            int q=0,check=0;
            int numToCheck = population[i2].str[j];
            for(int z=0; z<GA_NQueensNum; ++z){
                if(numToCheck == buffer[i].str[z]){
                    check=1;
                }
            }
            if(check == 0){
               while(buffer[i].str[q] != -1 && q<GA_NQueensNum){
                    q++;
               }
                if(q != GA_NQueensNum){
                    buffer[i].str[q] = numToCheck;
                }
            }
        }
		if (rand() < GA_MUTATION) mutate(buffer[i]);
	}
}

void pmx_mate(ga_vector &population, ga_vector &buffer)
{
	int esize = GA_POPSIZE * GA_ELITRATE;
	int spos, i1, i2;

	elitism(population, buffer, esize);

	for (int i=esize; i<GA_POPSIZE; i++) {
		i1 = rand() % (GA_POPSIZE / 2);
		i2 = rand() % (GA_POPSIZE / 2);
		spos = rand() % GA_NQueensNum;

		for(int j=0; j<GA_NQueensNum; j++)
        {
            if(population[i1].str[j]==population[i2].str[spos])
                buffer[i].str[j]=population[i1].str[spos];
            else
                buffer[i].str[j]=population[i1].str[j];
        }
        buffer[i].str[spos]=population[i2].str[spos];

        if (rand() < GA_MUTATION) mutate_inverse(buffer[i]);
	}
}

inline void print_best(ga_vector &gav)
{
    cout << "Best Board:";
    for(int i=0;i<GA_NQueensNum;++i){
        cout << " " << gav[0].str[i];
    }
    cout << "\n";
}

inline void swap(ga_vector *&population,
				 ga_vector *&buffer)
{ ga_vector *temp = population; population = buffer; buffer = temp; }

int main()
{
	srand(unsigned(time(NULL)));

	ga_vector pop_alpha, pop_beta;
	ga_vector *population, *buffer;
    int iteration = 0;
	init_population(pop_alpha, pop_beta);
	population = &pop_alpha;
	buffer = &pop_beta;

	for (int i=0; i<GA_MAXITER; i++) { //GA_MAXITER
		calc_fitness(*population);		// calculate fitness
		sort_by_fitness(*population);	// sort them
		print_best(*population);		// print the best one
        iteration = i;
		if ((*population)[0].fitness == 0) break;

		mate(*population, *buffer);		// mate the population together
		swap(population, buffer);		// swap buffers
	}
    cout << "stopped after iteration: " << iteration;
	return 0;
}
