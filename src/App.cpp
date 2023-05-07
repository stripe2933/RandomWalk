//
// Created by gomkyung2 on 2023/05/06.
//

#include "App.hpp"

#include <stdexcept>
#include <random>

#include <glm/gtc/constants.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <opengl_utils.hpp>

constexpr const char *VERTEX_SHADER_SOURCE = R"glsl(
#version 330 core
layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
   gl_Position = projection * view * model * vec4(aPos, 1.0);
}
)glsl";

constexpr const char *FRAGMENT_SHADER_SOURCE = R"glsl(
#version 330 core

out vec4 FragColor;

uniform vec3 lineColor;

void main() {
    FragColor = vec4(lineColor, 1.0);
}
)glsl";

App::App(int width, int height, const char *title) : window_width { width }, window_height { height } {
    // Init GLFW with given parameters.
    if (!glfwInit()) {
        throw std::runtime_error { "Failed to init GLFW." };
    }
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        throw std::runtime_error { "Failed to create GLFW window." };
    }
    glfwMakeContextCurrent(window);

    // Misc.
    glfwSetWindowUserPointer(window, reinterpret_cast<void*>(this));

    // Update projection matrix's aspect ratio by window coordinate.
    glfwSetFramebufferSizeCallback(window, [](GLFWwindow *p_window, int width, int height) {
        glViewport(0, 0, width, height);

        App *app = reinterpret_cast<App*>(glfwGetWindowUserPointer(p_window));
        app->window_width = width;
        app->window_height = height;
    });

    // Update camera distance by scrolled amount.
    glfwSetScrollCallback(window, [](GLFWwindow *p_window, double scroll_x, double scroll_y){
        App *app = reinterpret_cast<App*>(glfwGetWindowUserPointer(p_window));
        app->camera_distance = std::max(1.f, app->camera_distance + static_cast<float>(scroll_y));
    });

    // Init GLEW.
    if (glewInit() != GLEW_OK) {
        throw std::runtime_error { "Failed to init GLEW." };
    }
}

App::~App() noexcept {
    glfwTerminate();
}

void App::run() {
    // Shader program.
    GLuint vertex_shader = createShader(GL_VERTEX_SHADER, VERTEX_SHADER_SOURCE);
    GLuint fragment_shader = createShader(GL_FRAGMENT_SHADER, FRAGMENT_SHADER_SOURCE);
    GLuint program = createProgram({ vertex_shader, fragment_shader });
    glDeleteShader(vertex_shader);
    glDeleteShader(fragment_shader);

    constexpr std::size_t TRAJECTORY_COUNT = 5;
    std::array<GLuint, TRAJECTORY_COUNT> trajectory_vertex_arrays {};
    glGenVertexArrays(TRAJECTORY_COUNT, trajectory_vertex_arrays.data());

    constexpr std::size_t NUM_VERTICES = 1 << 16; // Each random walk has 32,768 steps.
    std::array<GLuint, TRAJECTORY_COUNT> trajectory_vertex_buffers {};
    for (std::size_t i = 0; i < TRAJECTORY_COUNT; ++i){
        glBindVertexArray(trajectory_vertex_arrays[i]);
        auto vertices = generateRandomWalkTrajectory(NUM_VERTICES, 0.1f);
        trajectory_vertex_buffers[i] = createVertexBuffer(
                std::span { reinterpret_cast<const float *>(vertices.data()), 3 * vertices.size() },
                { 3 },
                GL_STATIC_DRAW);
    }

    std::array<glm::vec3, TRAJECTORY_COUNT> line_colors {
            glm::vec3 { 1.f, 0.f, 0.f }, // Red
            glm::vec3 { 0.f, 1.f, 0.f }, // Green
            glm::vec3 { 0.f, 0.f, 1.f }, // Blue
            glm::vec3 { 1.f, 1.f, 0.f }, // Magenta
            glm::vec3 { 0.f, 1.f, 1.f }, // Yellow
    };

    auto model = glm::identity<glm::mat4>();

    // Fetch uniform locations.
    glUseProgram(program);
    GLint model_uniform = glGetUniformLocation(program, "model");
    GLint view_uniform = glGetUniformLocation(program, "view");
    GLint projection_uniform = glGetUniformLocation(program, "projection");
    GLint line_color_uniform = glGetUniformLocation(program, "lineColor");

    std::size_t point_count = 0;
    float elapsed_time = 0.f;
    while (!glfwWindowShouldClose(window)){
        float time_delta = static_cast<float>(glfwGetTime()) - elapsed_time; // Elapsed seconds from previous frame.
        elapsed_time += time_delta; // Total elapsed seconds.

        if (point_count < NUM_VERTICES){
            point_count++;
        }

        // Update MVP matrices.
        model = glm::rotate(model, 0.2f * time_delta, glm::vec3 { 0.f, 1.f, 0.f });
        auto view = glm::lookAt(glm::vec3 { 0.f, 0.f, camera_distance }, glm::vec3 { 0.f, 0.f, 0.f }, glm::vec3 { 0.f, 1.f, 0.f });
        auto projection = glm::perspective(glm::radians(45.f),
                                           static_cast<float>(window_width) / static_cast<float>(window_height),
                                           0.1f, 100.f);

        // Clear buffers.
        glClearColor(0.f, 0.f, 0.f, 0.f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(program);
        glUniformMatrix4fv(model_uniform, 1, GL_FALSE, glm::value_ptr(model));
        glUniformMatrix4fv(view_uniform, 1, GL_FALSE, glm::value_ptr(view));
        glUniformMatrix4fv(projection_uniform, 1, GL_FALSE, glm::value_ptr(projection));

        for (std::size_t i = 0; i < TRAJECTORY_COUNT; ++i){
            auto line_color = line_colors[i];
            glUniform3f(line_color_uniform, line_color.x, line_color.y, line_color.z);

            glBindVertexArray(trajectory_vertex_arrays[i]);
            glDrawArrays(GL_LINE_STRIP, 0, 3 * point_count);
        }

        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glDeleteVertexArrays(TRAJECTORY_COUNT, trajectory_vertex_arrays.data());
    glDeleteBuffers(TRAJECTORY_COUNT, trajectory_vertex_buffers.data());

    glfwTerminate();
}

std::vector<glm::vec3> App::generateRandomWalkTrajectory(std::size_t count, float velocity) {
    std::vector<glm::vec3> trajectory;
    trajectory.reserve(count);

    static std::random_device rd;
    static std::mt19937 gen { rd() };
    std::uniform_real_distribution<float> dis { 0.f, glm::pi<float>() };

    trajectory.emplace_back(0.f, 0.f, 0.f);
    while (trajectory.size() < count){
        auto theta = 2.f * dis(gen); // [0, 2π)
        auto phi = dis(gen); // [0, π)

        auto x = velocity * std::cos(theta) * std::cos(phi);
        auto y = velocity * std::cos(theta) * std::sin(phi);
        auto z = velocity * std::sin(theta);

        trajectory.emplace_back(trajectory.back() + glm::vec3 { x, y, z });
    }

    return trajectory;
}
