#include <iostream>
#include <chrono>
#include <thread>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <deque>

#define GLEW_STATIC
#define GLFW_INCLUDE_NONE

#include "GL/glew.h"
#include "GLFW/glfw3.h"
#include "glm/glm.hpp"
#include "glm/gtc/matrix_transform.hpp"
#include "glm/gtx/matrix_transform_2d.hpp"
#include "glm/gtc/type_ptr.hpp"

#include "gl_loader.h"

const GLfloat draw_vert[] = {
    -1.0f,
    -1.0f,
    -1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    -1.0f,
};

const GLfloat visual_vert[] = {
    -1.0f,
    -1.0,
    0.0f,
    0.0f,
    -1.0f,
    1.0f,
    0.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    1.0f,
    -1.0f,
    1.0f,
    0.0f,
};

struct transform
{
    glm::vec2 pos = glm::vec2(0.0f);
    float scale = 2.0f;
};

unsigned int fps_limit = 60;
unsigned int iter = 10000;
int rw = 512, rh = 512;
int cx = 0, cy = 0;
transform current_tf;
float hue = 0.0f;
transform pinned_tfs[10];
bool pinned_tf_enabled[10];
std::deque<transform> undo_tfs;

bool redraw = true;
bool resize = true;
bool redraw_color = true;

void update_tf()
{
    undo_tfs.push_back(current_tf);
    redraw = true;
}

void reset_tf()
{
    update_tf();
    current_tf = transform();
}

void undo_tf()
{
    if (undo_tfs.size())
    {
        current_tf = undo_tfs.back();
        undo_tfs.pop_back();
        redraw = true;
    }
}

void key_callback(GLFWwindow *window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        float dd = mods & GLFW_MOD_SHIFT ? 0.5f : mods & GLFW_MOD_CONTROL ? 0.02f : 0.1f;
        float sm = mods & GLFW_MOD_SHIFT ? 2.0f : mods & GLFW_MOD_CONTROL ? std::pow(2.0f, 0.25) : std::sqrt(2.0f);
        // Exit
        if (key == GLFW_KEY_ESCAPE)
        {
            glfwSetWindowShouldClose(window, true);
        }
        // Movement
        else if (key == GLFW_KEY_LEFT)
        {
            current_tf.pos.x -= dd * current_tf.scale;
            redraw = true;
        }
        else if (key == GLFW_KEY_RIGHT)
        {
            update_tf();
            current_tf.pos.x += dd * current_tf.scale;
        }
        else if (key == GLFW_KEY_UP)
        {
            update_tf();
            current_tf.pos.y += dd * current_tf.scale;
        }
        else if (key == GLFW_KEY_DOWN)
        {
            update_tf();
            current_tf.pos.y -= dd * current_tf.scale;
        }
        // Zoom
        else if (key == GLFW_KEY_Z)
        {
            update_tf();
            current_tf.scale /= sm;
        }
        else if (key == GLFW_KEY_X)
        {
            update_tf();
            current_tf.scale *= sm;
        }
        // Reset transform
        else if (key == GLFW_KEY_R)
        {
            reset_tf();
        }
        // Change color
        else if (key == GLFW_KEY_T)
        {
            hue = std::fmod(hue + 0.1f, 1.0f);
            redraw_color = true;
        }
        // Bookmarks
        else if (key >= GLFW_KEY_0 && key <= GLFW_KEY_9)
        {
            int i = key - GLFW_KEY_0;
            if (mods & GLFW_MOD_CONTROL)
            {
                pinned_tfs[i] = current_tf;
                pinned_tf_enabled[i] = true;
                std::cout << "Saved bookmark " << i << std::endl;
            }
            else if (pinned_tf_enabled[i])
            {
                update_tf();
                current_tf = pinned_tfs[i];
                std::cout << "Loaded bookmark " << i << std::endl;
            }
        }
        // Undo
        else if (key == GLFW_KEY_C)
        {
            undo_tf();
        }
    }
}

void cursor_callback(GLFWwindow *window, double x, double y)
{
    cx = x;
    cy = y;
}

void window_resize_callback(GLFWwindow *window, int width, int height)
{
    rw = width;
    rh = height;
    redraw = true;
    resize = true;
}

/// Accurately sleep for the specified time.
void precise_sleep(double seconds)
{
    static double estimate = 5e-3;
    static double mean = 5e-3;
    static double m2 = 0;
    static int64_t count = 1;

    while (seconds > estimate)
    {
        auto start = std::chrono::high_resolution_clock::now();
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
        auto end = std::chrono::high_resolution_clock::now();

        double observed = (end - start).count() / 1e9;
        seconds -= observed;

        ++count;
        double delta = observed - mean;
        mean += delta / count;
        m2 += delta * (observed - mean);
        double stddev = sqrt(m2 / (count - 1));
        estimate = mean + stddev;
    }

    auto start = std::chrono::high_resolution_clock::now();
    while ((std::chrono::high_resolution_clock::now() - start).count() / 1e9 < seconds)
        ;
}

int main()
{
    std::cout << "OpenGL Fractal Viewer" << std::endl;
    glewExperimental = true;

    // Initialize GLFW

    if (!glfwInit())
    {
        std::cerr << "Error initializing glfw" << std::endl;
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    GLFWwindow *window = glfwCreateWindow(rw, rh, "", NULL, NULL);
    glfwMakeContextCurrent(window);

    auto glew_e = glewInit();
    if (glew_e != GLEW_OK)
    {
        std::cerr << "Error initializing glew: " << glewGetErrorString(glew_e) << std::endl;
        return -1;
    }

    glfwSetInputMode(window, GLFW_STICKY_KEYS, false);
    glfwSetCursorPosCallback(window, cursor_callback);
    glfwSetKeyCallback(window, key_callback);
    glfwSetWindowSizeCallback(window, window_resize_callback);
    glfwSwapInterval(1);

    GLuint va;
    glGenVertexArrays(1, &va);
    glBindVertexArray(va);

    GLuint draw_vb;
    glGenBuffers(1, &draw_vb);
    glBindBuffer(GL_ARRAY_BUFFER, draw_vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(draw_vert), draw_vert, GL_STATIC_DRAW);

    GLuint draw_fb;
    glGenFramebuffers(1, &draw_fb);
    glBindFramebuffer(GL_FRAMEBUFFER, draw_fb);

    GLuint draw_tex;
    glGenTextures(1, &draw_tex);

    GLuint visual_vb;
    glGenBuffers(1, &visual_vb);
    glBindBuffer(GL_ARRAY_BUFFER, visual_vb);
    glBufferData(GL_ARRAY_BUFFER, sizeof(visual_vert), visual_vert, GL_STATIC_DRAW);

    GLuint draw_sp = load_shaders("assets/shaders/mandelbrot.vert", "assets/shaders/mandelbrot.frag");
    GLuint visual_sp = load_shaders("assets/shaders/visual.vert", "assets/shaders/visual.frag");

    GLuint draw_tf_u = glGetUniformLocation(draw_sp, "draw_tf");
    GLuint iterations_u = glGetUniformLocation(draw_sp, "iterations");
    GLuint tex_u = glGetUniformLocation(visual_sp, "tex");
    GLuint hue_u = glGetUniformLocation(visual_sp, "hue");

    reset_tf();

    double tp = glfwGetTime();
    // Rendering loop
    while (!glfwWindowShouldClose(window))
    {
        glfwGetFramebufferSize(window, &rw, &rh);
        
        // Only redraw the fractal if there is a change to the parameters.
        if (redraw)
        {
            // Calculate fractal
            glBindFramebuffer(GL_FRAMEBUFFER, draw_fb);

            glViewport(0, 0, rw, rh);

            if (resize)
            {
                glBindTexture(GL_TEXTURE_2D, draw_tex);
                glTexImage2D(GL_TEXTURE_2D, 0, GL_R32UI, rw, rh, 0, GL_RED_INTEGER, GL_UNSIGNED_INT, 0);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
                glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

                glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, draw_tex, 0);

                GLenum dbl[] = {GL_COLOR_ATTACHMENT0};
                glDrawBuffers(1, dbl);

                if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
                {
                    std::cerr << "Framebuffer incomplete" << std::endl;
                }

                resize = false;
            }

            glm::mat3 draw_tf = glm::identity<glm::mat3>();
            float draw_s = (float)rh / rw;
            draw_tf = glm::translate(draw_tf, current_tf.pos);
            draw_tf = glm::scale(draw_tf, glm::vec2(current_tf.scale, current_tf.scale * draw_s));

            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(draw_sp);

            glBindBuffer(GL_ARRAY_BUFFER, draw_vb);

            glUniformMatrix3fv(draw_tf_u, 1, GL_FALSE, glm::value_ptr(draw_tf));
            glUniform1ui(iterations_u, iter);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 2 * sizeof(GLfloat), (void *)0);
            glEnableVertexAttribArray(0);

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            glDisableVertexAttribArray(0);

            
        }

        // Redraw colors without redrawing the fractal
        if (redraw || redraw_color)
        {
            // Draw on screen
            glBindFramebuffer(GL_FRAMEBUFFER, 0);

            glViewport(0, 0, rw, rh);

            glClear(GL_COLOR_BUFFER_BIT);

            glUseProgram(visual_sp);

            glActiveTexture(GL_TEXTURE0);
            glBindBuffer(GL_ARRAY_BUFFER, visual_vb);
            glBindTexture(GL_TEXTURE_2D, draw_tex);

            glUniform1i(tex_u, 0);
            glUniform1f(hue_u, hue);

            glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)0);
            glEnableVertexAttribArray(0);

            glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), (void *)(2 * sizeof(GLfloat)));
            glEnableVertexAttribArray(1);

            glDrawArrays(GL_TRIANGLE_FAN, 0, 4);

            glDisableVertexAttribArray(0);
            glDisableVertexAttribArray(1);

            glfwSwapBuffers(window);

            double t = glfwGetTime();
            double dt = t - tp;

            glm::vec2 p0 = current_tf.pos - glm::vec2(current_tf.scale / 2, current_tf.scale / 2 * rh / rw), p1 = current_tf.pos + glm::vec2(current_tf.scale / 2, current_tf.scale / 2 * rh / rw);
            std::stringstream title_ss;
            title_ss << std::setprecision(3) << "Fractal - [(" <<  p0[0] << ", " << p0[1] << "), (" << p1[0] << ", " << p1[1] << ")] - " << iter << " iterations - " << dt << "s";
            glfwSetWindowTitle(window, title_ss.str().c_str());

            redraw = false;
            redraw_color = false;
        }

        glfwPollEvents();

        double t = glfwGetTime();
        double dt = t - tp;

        if (1.0 / dt > fps_limit)
        {
            precise_sleep(1.0 / fps_limit - dt);
        }

        t = glfwGetTime();
        dt = t - tp;
        tp = t;

        // hue = std::fmod(hue + dt / 20.0f, 1.0f);
    }
    std::cout << "Window closed" << std::endl;
}
