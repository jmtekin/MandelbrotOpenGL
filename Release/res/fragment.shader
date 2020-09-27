#version 330 core
in vec4 gl_FragCoord;

out vec4 FragColor;
out float gl_FragDepth;

#define MAX_ITERATIONS 1000

uniform vec4 color_ranges;
uniform float center_x;
uniform float center_y;
uniform float zoom;

int get_iterations()
{
    // Translate screen coordinates into coordinates in complex plane
    float real = ((gl_FragCoord.x / 1080 - 1) * zoom + center_x) * 3.0;
    float imag = ((gl_FragCoord.y / 1080 - 0.5) * zoom + center_y) * 3.0;

    int iterations = 0;
    float const_real = real;
    float const_imag = imag;

    // Run f(z) = z^2 + c, where c is a complex number, until output is bigger than 4 or until 500 iterations
    while (iterations < MAX_ITERATIONS)
    {
        float tmp_real = real;
        real = (real * real - imag * imag) + const_real;
        imag = (2.0 * tmp_real * imag) + const_imag;

        float dist = real * real + imag * imag;

        if (dist > 4.0)
            break;

        ++iterations;
    }
    return iterations;
}

vec4 return_color()
{
    int iter = get_iterations();
    if (iter == MAX_ITERATIONS)

    {
        gl_FragDepth = 0.0f;
        return vec4(0.0f, 0.0f, 0.0f, 1.0f);
    }

    // Turning number of iterations into color value. The more iterations the lighter the color meaning uncertain C-values become lighter creating the light rim around the mandelbrot set.
    float iterations = float(iter) / MAX_ITERATIONS;
    gl_FragDepth = iterations;

    vec4 color_0 = vec4(0.0f, 0.0f, 0.0f, 1.0f);
    vec4 color_1 = vec4(0.0f, 0.2f, 0.5f, 1.0f);
    vec4 color_2 = vec4(1.0f, 0.8f, 0.0f, 1.0f);
    vec4 color_3 = vec4(1.0f, 0.0f, 0.4f, 1.0f);

    float fraction = 0.0f;

    if (iterations < color_ranges[1])
    {
        fraction = (iterations - color_ranges[0]) / (color_ranges[1] - color_ranges[0]);
        return mix(color_0, color_1, fraction);
    }
    else if (iterations < color_ranges[2])
    {
        fraction = (iterations - color_ranges[1]) / (color_ranges[2] - color_ranges[1]);
        return mix(color_1, color_2, fraction);
    }
    else
    {
        fraction = (iterations - color_ranges[2]) / (color_ranges[3] - color_ranges[2]);
        return mix(color_2, color_3, fraction);
    }
}

void main()
{
    FragColor = return_color();
}