#include <iostream>
#include <omp.h>
#include <random>
#include <set>
#include <typeinfo>
#include "./shunting-yard.h"

using namespace std;
inline static bool is_free_variable(char c)
{
        return (((c >= 'a') && (c <= 'z')) || ((c >= 'A') && (c <= 'Z')));
        
}
set<char> register_free_variables(string s){
    set<char> free_vars;
    for(char c: s){
        if(is_free_variable(c)){
            free_vars.insert(c);
        }
    }
    return free_vars;
}
std::uniform_int_distribution<> number_generator(int sz){
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    //shifting 1 by sz bits is the same as 2^sz.
    std::uniform_int_distribution<> distrib(0, 1 << sz);

    return distrib;
}

//this function will be called in parallel.
void set_vars(TokenMap vars, set<char> free_vars){
    std::random_device rd;  //Will be used to obtain a seed for the random number engine
    std::mt19937 gen(rd()); //Standard mersenne_twister_engine seeded with rd()
    //shifting 1 by sz bits is the same as 2^sz.
    std::uniform_int_distribution<> distrib(0, 1 << free_vars.size());
    for(char v : free_vars){
        int i = 0;
        int value = (distrib(gen) & (1 << i)) >> i ;
        vars[string(1,v)] = value ? true : false;
        i++;
    }
}

int main(){
    string s;
    std::getline (std::cin, s);

    set<char> free_vars = register_free_variables(s);
    int free_vars_n = free_vars.size();
    TokenMap vars;
    /*
    cout << "Printing free_vars assignments" << endl;
    for(char v: free_vars){
        cout << "free var: " << v << ", it's value: " << vars[string(1, v)] << endl;
    }*/
    calculator c;
    bool success = false;
    #pragma omp parallel private(vars, c) shared(success)
    for(;!success;){
        int threadcount = omp_get_thread_num();

        set_vars(vars, free_vars);
        vars["0"] = false;
        vars["1"] = true;
        cout << "Printing free_vars assignments" << endl;
        cout << threadcount << endl;
        for(char v: free_vars){
            cout << "free var: " << v << ", it's value: " << vars[string(1, v)] << endl;
        }
        if(c.calculate(s.c_str(), vars).asBool()){
            #pragma omp critical
            success = true;
            cout << threadcount << endl;
            for(char v: free_vars){
                cout << "free var: " << v << ", it's value: " << vars[string(1, v)] << endl;
            }
            #pragma omp cancel parallel
        }
    }
    
    //say s = "a && b && c && d"
    //tokenmap has to have a corresponding definition for each thing.
    //
    //std::cout << typeid(calculator::calculate(s.c_str(), vars)).name() << std::endl;

    cout << "Ran successfully." << endl;
}


