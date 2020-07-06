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

using namespace std;

constexpr auto PI = 3.14159;
constexpr auto sizeW = 700;
constexpr auto sizeH = 550;

int mouseoldx, mouseoldy; // For mouse motion

GLuint ShaderProgram; // shaders

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
    //int mouseoldx, mouseoldy; // For mouse motion
    GLdouble getUpz(GLdouble y, GLdouble z) { return -y / z; }

    void updateModelView() {
        Up[2] = getUpz(Eye[1], Eye[2]);
        ModelView = glm::lookAt(glm::vec3(Eye[0], Eye[1], Eye[2]), glm::vec3(Center[0], Center[1], Center[2]), glm::vec3(Up[0], Up[1], Up[2]));
        glUniformMatrix4fv(ModelViewPos, 1, GL_FALSE, glm::value_ptr(ModelView));
        //glm::mat4 temp(1.0);
        //ModelView = glm::rotate(temp, glm::radians(10.0f), gl::vec3(0.0, 1.0, 0.0));
	//
    }

    void Move(int i, int j, int N) {
        GLdouble tempI = cosAngle[N] * Eye[i] - sinAngle[N] * Eye[j];
        GLdouble tempJ = sinAngle[N] * Eye[i] + cosAngle[N] * Eye[j];
        Eye[i] = tempI;
        Eye[j] = tempJ;
        updateModelView();
    }

    void MoveUp() {
        glm::mat4 temp(1.0);
        ModelView = glm::rotate(temp, glm::radians(1.0f), glm::vec3(0.0, 0.0, 1.0));
        glUniformMatrix4fv(ModelViewPos, 1, GL_FALSE, glm::value_ptr(ModelView));
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

        angle = 1;
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

Scenario* scene;

void mouse_callback(GLFWwindow* window, int button, int action, int mods) {
    if (button == GLFW_MOUSE_BUTTON_LEFT) {
        if (action == GLFW_PRESS) {
            double x, y;
            glfwGetCursorPos(window, &x, &y);
            mouseoldx = (int)x;
            mouseoldy = (int)y; // so we can move wrt x , y
        }
    }
    else if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
        scene->ResetPositions();
    }
}

void mousedrag_callback(GLFWwindow* window, double x, double y) {
    if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
        int yloc = (int)y - mouseoldy;	// We will use the y coord to zoom in/out
        int xloc = (int)x - mouseoldx;
        if (yloc) {
            (yloc < 0) ? scene->Move(2, 1, 1) : scene->Move(2, 1, 0);
        }
        if (xloc) {
            (xloc > 0) ? scene->Move(2, 0, 1) : scene->Move(2, 0, 0);
        }

        mouseoldy = (int)y;
        mouseoldx = (int)x;
    }
}

/* Defines what to do when various keys are pressed */
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);

/* Reshapes the window appropriately */
void reshape_callback(GLFWwindow* window, int w, int h);

int main() {

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
    glfwSetMouseButtonCallback(window, mouse_callback);		//glutMouseFunc(mouse) ;
    glfwSetCursorPosCallback(window, mousedrag_callback);		//glutMotionFunc(mousedrag) ;

    // First scene render
    reshape_callback(window, sizeW, sizeH);

    // render loop
    // Start the main code
    while (!glfwWindowShouldClose(window)) //glutMainLoop(); 
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        scene->Draw();
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
    scene->Delete();
    glfwDestroyWindow(window);
    // glfw: terminate, clearing all previously allocated GLFW resources.
    glfwTerminate();
    return 0;   /* ANSI C requires main to return int. */

}

void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods) {
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
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
        //scene->MoveUp();
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        scene->Move(2, 1, 1);
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        scene->ResetPositions();
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        scene->Spanning(true);
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        scene->Spanning(false);
    }


    if (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        scene->rb.giro1();
    }
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
        scene->rb.giro2();
    }
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        scene->rb.giro3();
    }
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        scene->rb.giro4();
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS) {
        scene->rb.giro5();
    }
    if (glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        scene->rb.giro6();
    }
    if (glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        scene->rb.giro7();
    }
    if (glfwGetKey(window, GLFW_KEY_I) == GLFW_PRESS) {
        scene->rb.giro8();
    }
    if (glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        scene->rb.giro9();
    }


    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        scene->rb.giro1inv();
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
        scene->rb.giro2inv();
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        scene->rb.giro3inv();
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        scene->rb.giro4inv();
    }
    if (glfwGetKey(window, GLFW_KEY_G) == GLFW_PRESS) {
        scene->rb.giro5inv();
    }
    if (glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        scene->rb.giro6inv();
    }
    if (glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        scene->rb.giro7inv();
    }
    if (glfwGetKey(window, GLFW_KEY_K) == GLFW_PRESS) {
        scene->rb.giro8inv();
    }
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        scene->rb.giro9inv();
    }
}

void reshape_callback(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    if (h > 0) {
        scene->Projection = glm::perspective(glm::radians(45.0f), (GLfloat)w / (GLfloat)h, 1.0f, 100.0f);
        glUniformMatrix4fv(scene->ProjectionPos, 1, GL_FALSE, &scene->Projection[0][0]);
    }

}