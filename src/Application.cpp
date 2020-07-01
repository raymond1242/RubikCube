/***************************************************************************/
/* This is a simple demo program written for CSE 167 by Ravi Ramamoorthi   */
/* This program corresponds to the first OpenGL lecture.                   */
/*                                                                         */
/* Successive lectures/iterations make this program more complex.          */
/* This is the first simple program to draw a ground plane allowing zooming*/
/* The intent is to show how to draw a simple scene.                       */
/* Modified in August 2016 by Hoang Tran to exclusively use modern OpenGL  */
/***************************************************************************/

#include <stdio.h>
#include <assert.h>
#include <stdlib.h>

#include <glad/glad.h>
#include <glfw/glfw3.h>
#include "Rubik.h"
// Usage of degrees is deprecated. Use radians for glm functions.
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include "shaders.h"
#include <cmath>

constexpr auto PI = 3.14159;
constexpr auto sizeW = 700;
constexpr auto sizeH = 550;

int mouseoldx, mouseoldy; // For mouse motion
//GLdouble eyeloc = 2.0; // Where to look from; initially 0 -2, 2
GLdouble eyeY = 0.0;
GLdouble eyeZ = 2.828427;
GLdouble eyeX = 0.0;

GLdouble UpX = 0.0;
GLdouble UpY = 1.0;
GLdouble UpZ = -1.0;

GLdouble originX = 0.0;
GLdouble originY = 0.0;
GLdouble originZ = 0.0;

GLuint ShaderProgram; // shaders
GLuint projectionPos, modelviewPos; // Locations of uniform variables
glm::mat4 projection, modelview; // The mvp matrices themselves

GLdouble angle = 5;
GLdouble Nangle = -5;
GLdouble cosAngle = cos(angle * PI / 180);
GLdouble sinAngle = sin(angle * PI / 180);
GLdouble cosNangle = cos(Nangle * PI / 180);
GLdouble sinNangle = sin(Nangle * PI / 180);

void GetShaders() {
    std::string vertex_shader_path;
    std::string fragment_shader_path;
    GLuint vertexshader, fragmentshader;

#ifdef __unix__         
    vertex_shader_path = "//home//manuel//Documents//Projects//OpenGL//GLFW_GLAD_GLUT_GLEW_cmake_project//src//mytest1_glut//shaders//nop.vert";
    fragment_shader_path = "//home//manuel//Documents//Projects//OpenGL//GLFW_GLAD_GLUT_GLEW_cmake_project//src//mytest1_glut//shaders//nop.frag";

#elif defined(_WIN32) || defined(WIN32) 
    vertex_shader_path = "C://Users//Raymond//Documents//UCSP//OpenGl_Shaders//shaders//nop.vert";
    fragment_shader_path = "C://Users//Raymond//Documents//UCSP//OpenGl_Shaders//shaders//nop.frag";
#endif

    // Initialize the shader program
    vertexshader = initshaders(GL_VERTEX_SHADER, vertex_shader_path.c_str());
    fragmentshader = initshaders(GL_FRAGMENT_SHADER, fragment_shader_path.c_str());

    ShaderProgram = initprogram(vertexshader, fragmentshader);
}



struct Scenario {
    double Eye[3] = { 0.0, 0.0, 2.828427 };
    double Up[3] = { 0.0,1.0,-1.0 };
    double Center[3] = { 0.0,0.0,0.0 };

    GLuint ProjectionPos, ModelViewPos; // Locations of uniform variables
    glm::mat4 Projection, ModelView; // The mvp matrices themselves

    GLdouble angle, cosAngle[2], sinAngle[2];

    int Shader;
    Rubik rb;

    GLdouble getUpz(GLdouble y, GLdouble z) { return -y / z; }

    void updateModelView() {
        Up[2] = getUpz(Eye[1], Eye[2]);
        ModelView = glm::lookAt(glm::vec3(Eye[0], Eye[1], Eye[2]), glm::vec3(Center[0], Center[1], Center[2]), glm::vec3(Up[0], Up[1], Up[2]));
        glUniformMatrix4fv(ModelViewPos, 1, GL_FALSE, &ModelView[0][0]);
    }

    void Move(int i, int j, int N) {
        GLdouble tempI = cosAngle[N] * Eye[i] - sinAngle[N] * Eye[j];
        GLdouble tempJ = sinAngle[N] * Eye[i] + cosAngle[N] * Eye[j];
        Eye[i] = tempI;
        Eye[j] = tempJ;
        updateModelView();
    }

    void ResetPositions() {
        Eye[0] = 0.0; Eye[1] = 0.0; Eye[2] = 2.828427;
        Up[0] = 0.0; Up[1] = 1.0; Up[2] = -1.0;
        updateModelView();
    }

    Scenario() {
        GetShaders();
        Shader = ShaderProgram;
        rb.SetUp(Shader);

        angle = 5;
        GLdouble angleRadian = angle * PI / 180;
        cosAngle[0] = cos(angleRadian); sinAngle[0] = sin(angleRadian);
        cosAngle[1] = cos(-angleRadian); sinAngle[1] = sin(-angleRadian);

        glClearColor(0.2f, 0.2f, 0.2f, 0.8f);

        Projection = glm::mat4(1.0f);
        glEnable(GL_DEPTH_TEST);

        Up[2] = getUpz(Eye[1], Eye[2]);
        ModelView = glm::lookAt(glm::vec3(Eye[0], Eye[1], Eye[2]), glm::vec3(Center[0], Center[1], Center[2]), glm::vec3(Up[0], Up[1], Up[2]));

        ProjectionPos = glGetUniformLocation(Shader, "projection");
        ModelViewPos = glGetUniformLocation(Shader, "modelview");

        glUniformMatrix4fv(ProjectionPos, 1, GL_FALSE, &Projection[0][0]);
        glUniformMatrix4fv(ModelViewPos, 1, GL_FALSE, &ModelView[0][0]);
    }

    void Spanning(bool t) { rb.ModifyDistance(t); }
    void Draw() { rb.draw(); }
    void Delete() { rb.Delete(); }
};

GLdouble getUpz(GLdouble y, GLdouble z) {
    return -y / z;
}

Scenario *scene;

//Rubik test;

void updateModelView() {
    UpZ = getUpz(eyeY, eyeZ);
    modelview = glm::lookAt(glm::vec3(eyeX, eyeY, eyeZ), glm::vec3(originX, originY, originZ), glm::vec3(UpX, UpY, UpZ));
    glUniformMatrix4fv(modelviewPos, 1, GL_FALSE, &modelview[0][0]);
}
/* Defines what to do when various keys are pressed */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

/* Reshapes the window appropriately */
void reshape_callback(GLFWwindow* window, int w, int h);

void init(void)
{
    glClearColor(0.2f, 0.2f, 0.2f, 0.2f);

    /* initialize viewing values  */
    projection = glm::mat4(1.0f); // The identity matrix
    glEnable(GL_DEPTH_TEST);

    // Think about this.  Why is the up vector not normalized?
    UpZ = getUpz(eyeY, eyeZ);
    modelview = glm::lookAt(glm::vec3(eyeX, eyeY, eyeZ), glm::vec3(originX, originY, originZ), glm::vec3(UpX, UpY, UpZ));

    // Now create the buffer objects to be used in the scene later
    // Remember to delete all the VAOs and VBOs that you create when the program terminates!

    GetShaders();

    //test.SetUp(ShaderProgram);

    // Get the positions of the uniform variables
    projectionPos = glGetUniformLocation(ShaderProgram, "projection");
    modelviewPos = glGetUniformLocation(ShaderProgram, "modelview");
    // Pass the projection and modelview matrices to the shader
    glUniformMatrix4fv(projectionPos, 1, GL_FALSE, &projection[0][0]);
    glUniformMatrix4fv(modelviewPos, 1, GL_FALSE, &modelview[0][0]);

}

int main()
{
    // Requests the type of buffers (Single, RGB).
    // Think about what buffers you would need...

    glfwInit(); //glutInit(&argc, argv);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

    // glfw window creation
    GLFWwindow* window = glfwCreateWindow(sizeW, sizeH, " Rubick Cube ", NULL, NULL);

    if (window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    glfwSetWindowPos(window, 100, 100);

    // glad: load all OpenGL function pointers
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    scene = new Scenario();

    glfwSetFramebufferSizeCallback(window, reshape_callback);	//glutReshapeFunc(reshape) ;
    glfwSetKeyCallback(window, key_callback);					//glutKeyboardFunc(keyboard);
    //glfwSetMouseButtonCallback(window, mouse_callback);		//glutMouseFunc(mouse) ;
    //glfwSetCursorPosCallback(window, mousedrag_callback);		//glutMotionFunc(mousedrag) ;

    // First scene render
    reshape_callback(window, sizeW, sizeH);

    // render loop
    // Start the main code
    while (!glfwWindowShouldClose(window)) //glutMainLoop(); 
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene->Draw();
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        // -------------------------------------------------------------------------------
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    scene->Delete();
    glfwDestroyWindow(window);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;   /* ANSI C requires main to return int. */

}

void key_callback(GLFWwindow * window, int key, int scancode, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        scene->Delete();
        glfwSetWindowShouldClose(window, true);
    }
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) {
        scene->Move(2, 0, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) {
        scene->Move(2, 0, 1);
    }
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) {
        scene->Move(2, 1, 0);
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        scene->Move(2, 1, 1);
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        scene->ResetPositions();
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        scene->Spanning(true);
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        scene->Spanning(false);
    }
}

void reshape_callback(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    if (h > 0) {
        scene->Projection = glm::perspective(45.0f / 180.0f * glm::pi<float>(), (GLfloat)w / (GLfloat)h, 1.0f, 100.0f);
        glUniformMatrix4fv(scene->ProjectionPos, 1, GL_FALSE, &scene->Projection[0][0]);
    }

}