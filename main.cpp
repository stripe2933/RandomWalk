#include <iostream>

#include <App.hpp>

int main() {
    try{
        App app { 640, 480, "Random Walk" };
        app.run();
    }
    catch (const std::exception &ex){
        std::cerr << "[ERROR] " << ex.what() << '\n';
        return 1;
    }

    return 0;
}
