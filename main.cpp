#include"LcVulkan.hpp"
const int width = 1000;
const int height = 780;
const std::string title="LcWindow";

int main(void){

    LcWindow window(LcWindowInfo{width,height,title});


    window.Run();

    return 0;
}