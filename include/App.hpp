//
// Created by gomkyung2 on 2023/05/06.
//

#ifndef RANDOMWALK_APP_HPP
#define RANDOMWALK_APP_HPP

#include <vector>

#include <GLFW/glfw3.h>
#include <glm/vec3.hpp>

class App{
public:
    App(int width, int height, const char *title);
    ~App() noexcept;

    void run();

private:
    GLFWwindow *window;
    int window_width, window_height;

    float camera_distance = 10.f;

    /**
     * Generate random walk trajectory with given step count (=> \p count) and each step size (=> \p velocity).
     * @param count Total step count.
     * @param velocity Step size. For each step, the latest point is distanced from its previous point by step size.
     * @return Generated 3-dimensional position trajectory.
     */
    static std::vector<glm::vec3> generateRandomWalkTrajectory(std::size_t count, float velocity);
};

#endif //RANDOMWALK_APP_HPP
