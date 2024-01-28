#include "URLProxyC.h"
#include <fstream>
int main(){
    const std::string f = force_connect("https://www.basketball-reference.com/playoffs/NBA_2021.html#per_game-team", 2);
    std::cout << "Complete!" << std::endl;
    return 1;
}