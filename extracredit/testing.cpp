#include "tools.h"
int main()
{
    srand (time(NULL));
    std::vector<int> justin;
    int i= 0;
    justin.push_back(1);
    justin.push_back(5);
    justin.push_back(10);
    justin.erase(justin.begin());
    for(int i= 0; i <  justin.size(); i++)
    {
        std::cout<<justin[i]<<std::endl;
    }
};