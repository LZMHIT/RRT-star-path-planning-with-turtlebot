#include <iostream>
#include "project3/Map_manager.hpp"

using namespace std;

int main( int argc, char** argv )
{
    Map_manager m;
    m.show_image();

    std::cout<< m.get_state(140,185);


    return 0;
}