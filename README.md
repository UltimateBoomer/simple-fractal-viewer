# Simple Fractal Viewer

This application renders the Mandelbrot fractal using OpenGL shaders.

![](screenshot.png?raw=true)

# Features

## Moving

Use the arrow keys to move around and `z`/`x` to zoom in/out. `Ctrl` modifier reduces the movement step, and `Shift` modifier increases the movement step.

## Changing colors

Press `T` to rotate the color scheme.

## Bookmarking

Use `CTRL+[0-9]` to create a bookmark for the current position. Then, input the corresponding number to restore the position.

# Build

To build Simple Fractal Viewer, install `GLFW`, `GLEW`, `glm` devel libraries and build using cmake.