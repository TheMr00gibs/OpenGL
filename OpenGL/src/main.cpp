#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>

struct ShaderProgramSource
{
    std::string VertexSource;
    std::string FragmentSource;
};

static ShaderProgramSource ParseShader(const std::string& filepath) 
{
    std::ifstream stream(filepath);
    
    enum class ShaderType
    {
        NONE = -1, VERTEX = 0, FRAGMENT = 1
    };

    std::string line;
    std::stringstream ss[2];
    ShaderType type = ShaderType::NONE;

    while(getline(stream, line))
    {
        if (line.find("#shader") != std::string::npos)
        {
            if (line.find("vertex") != std::string::npos)
            {
                type = ShaderType::VERTEX;
            }
            else if (line.find("fragment") != std::string::npos)
            {
                type = ShaderType::FRAGMENT;
            }
        }
        else
        {
            ss[(int)type] << line << '\n';
        }
    }

    return { ss[0].str(), ss[1].str() };
}


static unsigned int CompileShader(unsigned int type, const std::string& source)
{
    unsigned int id = glCreateShader(type);
    const char* src = source.c_str(); // $source[0] work to
    glShaderSource(id, 1, &src, nullptr);
    glCompileShader(id);

    int result;
    glGetShaderiv(id, GL_COMPILE_STATUS, &result);
    if (result == GL_FALSE)
    {
        int length;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &length);
        char* message = (char*)alloca(length * sizeof(char));
        glGetShaderInfoLog(id, length, &length, message);
        std::cout << "Failed to compile " << (type == GL_VERTEX_SHADER ? "vertex" : "fragment") << "shader" << std::endl;
        std::cout << message << std::endl;
        glDeleteShader(id);
        return 0;
    }

    return id;
}

static unsigned int CreateShader(const std::string& vertexShader, const std::string& fragmentShader)
{
    unsigned int program = glCreateProgram();
    unsigned int vs = CompileShader(GL_VERTEX_SHADER, vertexShader);
    unsigned int fs = CompileShader(GL_FRAGMENT_SHADER, fragmentShader);

    glAttachShader(program, vs);
    glAttachShader(program, fs);
    glLinkProgram(program);
    glValidateProgram(program);

    glDeleteShader(vs); // Delete shader intermediate obj
    glDeleteShader(fs); // Delete shader intermediate obj

    return program;
}

int main(void)
{
    GLFWwindow* window;

    /* Initialize the library */
    if (!glfwInit())
        return -1;

    /* Create a windowed mode window and its OpenGL context */
    window = glfwCreateWindow(640, 480, "Hello World", NULL, NULL);
    if (!window)
    {
        glfwTerminate();
        return -1;
    }

    /* Make the window's context current */
    glfwMakeContextCurrent(window);

    if (glewInit() != GLEW_OK) 
    {
        std::cout << "Error!" << std::endl;
    }
    std::cout << glGetString(GL_VERSION) << std::endl;


    float positions[] = {
        -0.5f, -0.5f, // 0
         0.5f, -0.5f, // 1
         0.5f,  0.5f, // 2
        -0.5f,  0.5f, // 3
    };

    unsigned int indices[] = {
        0, 1, 2,
        2, 3, 0
    };

    unsigned int buffer;
    // Declare an unsigned variable to store the buffer ID (generated by OpenGL).

    glGenBuffers(1, &buffer);
    // Generate a buffer and store its unique ID in the `buffer` variable.
    // The first argument (`1`) specifies the number of buffers to generate.
    // OpenGL assigns a unique identifier for the buffer.

    glBindBuffer(GL_ARRAY_BUFFER, buffer);
    // Bind the generated buffer to the target `GL_ARRAY_BUFFER`.
    // This means that all subsequent buffer-related operations will affect this bound buffer.
    // `GL_ARRAY_BUFFER` is used to store data such as vertex positions, colors, etc.

    glBufferData(GL_ARRAY_BUFFER, 6 * 2 * sizeof(float), positions, GL_STATIC_DRAW);
    // Allocate memory for the bound buffer and copy the provided data (`positions`) into it.
    // - The first argument, `GL_ARRAY_BUFFER`, confirms the type of the bound buffer.
    // - The second argument, `6 * sizeof(float)`, specifies the size of the data in bytes (6 floats in this case).
    // - The third argument, `positions`, is a pointer to the data to be copied into the buffer.
    // - The last argument, `GL_STATIC_DRAW`, tells OpenGL that the data will mostly be used for drawing and won't change frequently.

    glEnableVertexAttribArray(0);
    // Enable the vertex attribute at index `0`.
    // - This prepares the attribute for use in the vertex shader.
    // - The index `0` corresponds to a layout location in the shader, such as `layout(location = 0)`.


    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, 0);
    // Define how the data in the buffer is interpreted for the vertex attribute at index `0`.
    // - The first argument (`0`) specifies the attribute index.
    // - The second argument (`2`) is the number of components per vertex (e.g., `x` and `y` for 2D positions).
    // - The third argument (`GL_FLOAT`) specifies the data type of each component (here `float`).
    // - The fourth argument (`GL_FALSE`) indicates whether the data should be normalized (no normalization in this case).
    // - The fifth argument (`sizeof(float) * 2`) is the stride, or the total size (in bytes) of one vertex's data.
    // - The sixth argument (`0`) is the offset in the buffer where the data for this attribute starts (no offset here).

    // CREATE INDEX BUFFER OBJECT (IBO)
    unsigned int ibo;
    glGenBuffers(1, &ibo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ibo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, 6 * sizeof(unsigned int), indices, GL_STATIC_DRAW);
    
    ShaderProgramSource source = ParseShader("res/shaders/Basic.shader");
    std::cout << "VERTEX" << std::endl;
    std::cout << source.VertexSource << std::endl;
    std::cout << "FRAGMENT" << std::endl;
    std::cout << source.FragmentSource << std::endl;
    

    unsigned int shader = CreateShader(source.VertexSource, source.FragmentSource);
    glUseProgram(shader);


    /* Loop until the user closes the window */
    while (!glfwWindowShouldClose(window))
    {
        /* Render here */
        glClear(GL_COLOR_BUFFER_BIT);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, nullptr); // Draw call
        
        /* Swap front and back buffers */
        glfwSwapBuffers(window);

        /* Poll for and process events */
        glfwPollEvents();
    }
    glDeleteProgram(shader);

    glfwTerminate();
    return 0;
}