#include <iostream>
#include <random>
#include <ctime>
using namespace std;
struct Item {
    int profit;
    int weight;
};
static Item items[] = {
    {60, 10},
    {100, 20},
    {120, 30},
    {30, 5},
    {90, 15},
    {20, 4},
    {40, 6}
};
static const int num_items = sizeof(items)/sizeof(items[0]);
static const int capacity = 50;
static const int population_size      = 20;
static const int max_gen       = 50;
static const int t_size = 4;
static const double mutation_probability = 0.1; 
typedef int* solution;
static mt19937 rng((unsigned)time(nullptr));
int random_int(int minVal, int maxVal) {
    uniform_int_distribution<int> dist(minVal, maxVal);
    return dist(rng);
}
double random_double() {
    uniform_real_distribution<double> dist(0.0, 1.0);
    return dist(rng);
}
solution generatesolution(int length) {
    solution indiv = new int[length];
    for(int i = 0; i < length; i++) {
        indiv[i] = random_int(0, 1);  
    }
    return indiv;
}
void freesolution(solution chr) {
    delete[] chr;
}
int fitness(const solution &chr) {
    int total_profit = 0;
    int total_weight = 0;
    for(int i = 0; i < num_items; i++) {
        if(chr[i] == 1) {
            total_profit  += items[i].profit;
            total_weight  += items[i].weight;
        }
    }
    if(total_weight > capacity) {
        return 0;
    }
    return total_profit;
}
solution* generate_population(int pop_size, int length) {
    solution* pop = new solution[pop_size];
    for(int i = 0; i < pop_size; i++) {
        pop[i] = generatesolution(length);
    }
    return pop;
}
void free_population(solution* pop, int pop_size) {
    for(int i = 0; i < pop_size; i++) {
        freesolution(pop[i]);
    }
    delete[] pop;
}
solution t_selection(solution* population, int pop_size) {
    int best_index = -1;
    int best_fit = -1;
    for(int i = 0; i < t_size; i++) {
        int idx = random_int(0, pop_size-1);
        int f = fitness(population[idx]);
        if(f > best_fit) {
            best_fit   = f;
            best_index = idx;
        }
    }
    solution best = new int[num_items];
    for(int j = 0; j < num_items; j++) {
        best[j] = population[best_index][j];
    }
    return best;
}
void crossover(solution &c1, solution &c2) {
    if(num_items <= 1) return;
    int point = random_int(1, num_items - 1);
    for(int i = point; i < num_items; i++) {
        int temp = c1[i];
        c1[i] = c2[i];
        c2[i] = temp;
    }
}
void mutate(solution &chr) {
    for(int i = 0; i < num_items; i++) {
        if(random_double() < mutation_probability) {
            chr[i] = 1 - chr[i]; // Flip
        }
    }
}
solution copysolution(const solution &src) {
    solution dest = new int[num_items];
    for(int i = 0; i < num_items; i++) {
        dest[i] = src[i];
    }
    return dest;
}

int main() {
    solution* population = generate_population(population_size, num_items);
    int best_fitness = 0;
    solution best_solution = nullptr;
    for(int gen = 0; gen < max_gen; gen++) {
        for(int i = 0; i < population_size; i++) {
            int fit = fitness(population[i]);
            if(fit > best_fitness) {
                best_fitness = fit;
                if(best_solution) {
                    freesolution(best_solution);
                }
                best_solution = copysolution(population[i]);
            }
        }
        solution* new_pop = new solution[population_size];
        int count_new = 0;
        while(count_new < population_size) {
            solution parent1 = t_selection(population, population_size);
            solution parent2 = t_selection(population, population_size);
            crossover(parent1, parent2);
            mutate(parent1);
            mutate(parent2);
            new_pop[count_new++] = parent1;
            if(count_new < population_size) {
                new_pop[count_new++] = parent2;
            }
        }
        free_population(population, population_size);
        population = new_pop;
    }
    cout << "max profit found: "  << best_fitness << endl;
    cout << "best solution found :";
    for(int i = 0; i < num_items; i++) {
        cout << best_solution[i];
    }
    cout << endl;
    int total_weight = 0;
    for(int i = 0; i < num_items; i++) {
        if(best_solution[i] == 1) {
            cout << "–item " << i << ": profit=" << items[i].profit << ", weight=" << items[i].weight << "\n";
            total_weight += items[i].weight;
        }
    }
    cout << "total weight used = " << total_weight << " (capacity=" << capacity << ")\n";
    freesolution(best_solution);
    free_population(population, population_size);
    return 0;
}
