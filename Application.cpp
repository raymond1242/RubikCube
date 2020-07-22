
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
#include <stdlib.h>
#include <time.h>
#include <cmath>
#include "Cube.h"
#include "Cross.h"
#include "Corners.h"
#include "Edges.h"
#include "OLL.h"
#include "PLL.h"
using namespace std;

constexpr auto PI = 3.14159;
constexpr auto sizeW = 700;
constexpr auto sizeH = 550;

int mouseoldx, mouseoldy; // For mouse motion

GLuint ShaderProgram; // shaders

void mouse_callback(GLFWwindow* window, int button, int action, int mods);
void mousedrag_callback(GLFWwindow* window, double x, double y);
void key_callback(GLFWwindow* window, int key, int scancode, int action, int mods);
void reshape_callback(GLFWwindow* window, int w, int h);

enum {
    giro1, giro2, giro3, giro4, giro5, giro6, giro7, giro8, giro9, giro10, giro11, giro12, giro13,
    giro14, giro15, giro16, giro17, giro18
};


std::string format(std::string s) {
    std::string formatted;

    for (int i = 0; i < s.length(); ++i) {
        if (s[i] == '\'') {
            formatted += s[i - 1];
            formatted += s[i - 1];
        }
        else if (s[i] == '2') {
            formatted += s[i - 1];
        }
        else if (s[i] == ' ') {

        }
        else {
            formatted += s[i];
        }
    }

    return formatted;

}

string TestSolve(string argString) {
    cout <<endl<< "Secuencia a resolver : " << argString << endl;
    int scrambleNum = 0;
    Cube myCube(false);
    string res = "";
    ++scrambleNum;
    std::string scramble = format(argString);
    //std::cout << "Scramble #" << scrambleNum << ": ";
    myCube.moves(scramble);
    myCube.xd = true;
    Cross::solveCross(myCube);
    //std::cout << "Cross solved" << std::endl;
    Corners::solveCorners(myCube);
    //std::cout << "Corners solved" << std::endl;
    Edges::solveEdges(myCube);
    //std::cout << "Edges solved" << std::endl;;
    OLL::solveOLL(myCube);
    //std::cout << "OLL solved" << std::endl;
    PLL::solvePLL(myCube);
    //std::cout << "PLL solved" << std::endl;
    cout << "Solucion : " << myCube.mov << endl << endl;
    return myCube.mov;
    //    string Cubo = randomize(); //Obtenemos un cubo random -> "UUU...R...F..D..L..B.."
      //  cout << Cubo << endl;
        //string solution = get_solution(Cubo); //Obtenemos la solución de un cubo dado -> ["F","R2",..,"B'"]
        //co/ut << solution << endl;
}

void GetShaders() {
    std::string vertex_shader_path;
    std::string fragment_shader_path;
    GLuint vertexshader, fragmentshader;

#ifdef __unix__         
    vertex_shader_path = "//home//manuel//Documents//Projects//OpenGL//GLFW_GLAD_GLUT_GLEW_cmake_project//src//mytest1_glut//shaders//nop.vert";
    fragment_shader_path = "//home//manuel//Documents//Projects//OpenGL//GLFW_GLAD_GLUT_GLEW_cmake_project//src//mytest1_glut//shaders//nop.frag";

#elif defined(_WIN32) || defined(WIN32) 
    vertex_shader_path = "C://Users//Raymond//Desktop//CG-Library//src//Rubikv4//nop.vert";
    fragment_shader_path = "C://Users//Raymond//Desktop//CG-Library//src//Rubikv4//nop.frag";
                    
#endif

    // Initialize the shader program
    vertexshader = initshaders(GL_VERTEX_SHADER, vertex_shader_path.c_str());
    fragmentshader = initshaders(GL_FRAGMENT_SHADER, fragment_shader_path.c_str());

    ShaderProgram = initprogram(vertexshader, fragmentshader);
}

struct Scenario {

    double Eye[3];
    double Up[3];
    double Center[3];
    GLuint ProjectionPos, ModelViewPos; // Locations of uniform variables
    glm::mat4 Projection, ModelView; // The mvp matrices themselves
    GLdouble angle, cosAngle[2], sinAngle[2];
    int Shader;
    Rubik rb;

    GLdouble getUpz(GLdouble y, GLdouble z) { return -y / z; }

    Scenario() : Eye{0,0,2.828427}, Up{0,1,-1}, Center{0,0,0}, ProjectionPos(0), ModelViewPos(0),
                 Projection(1.0f), ModelView(1.0f), angle(0), cosAngle{0,0}, sinAngle{0,0}, Shader(0)
    {
        GetShaders();
        Shader = ShaderProgram;
        rb.SetUp(Shader);
        angle = 1;
        GLdouble angleRadian = angle * PI / 180;
        cosAngle[0] = cos(angleRadian); sinAngle[0] = sin(angleRadian);
        cosAngle[1] = cos(-angleRadian); sinAngle[1] = sin(-angleRadian);

        glClearColor(0.2f, 0.2f, 0.2f, 0.8f);

        Projection = glm::mat4(1.0f);

        Up[2] = getUpz(Eye[1], Eye[2]);
        ModelView = glm::lookAt(glm::vec3(Eye[0], Eye[1], Eye[2]), glm::vec3(Center[0], Center[1], Center[2]), glm::vec3(Up[0], Up[1], Up[2]));
        ProjectionPos = glGetUniformLocation(Shader, "projection");
        ModelViewPos = glGetUniformLocation(Shader, "modelview");

        glUniformMatrix4fv(ProjectionPos, 1, GL_FALSE, glm::value_ptr(Projection));
        glUniformMatrix4fv(ModelViewPos, 1, GL_FALSE, glm::value_ptr(ModelView));
    }

    void updateModelView() 
    {
        Up[2] = getUpz(Eye[1], Eye[2]);
        ModelView = glm::lookAt(glm::vec3(Eye[0], Eye[1], Eye[2]), glm::vec3(Center[0], Center[1], Center[2]), glm::vec3(Up[0], Up[1], Up[2]));
        glUniformMatrix4fv(ModelViewPos, 1, GL_FALSE, glm::value_ptr(ModelView));
    }

    void Move(int i, int j, int N) 
    {
        GLdouble tempI = cosAngle[N] * Eye[i] - sinAngle[N] * Eye[j];
        GLdouble tempJ = sinAngle[N] * Eye[i] + cosAngle[N] * Eye[j];
        Eye[i] = tempI;
        Eye[j] = tempJ;
        updateModelView();
    }
    void Zoom(int flag)
    {
        Eye[2] += (flag) ? -0.05 : 0.05;
        updateModelView();
    }

    void ResetPositions() 
    {
        Eye[0] = 0.0; Eye[1] = 0.0; Eye[2] = 2.828427;
        Up[0] = 0.0; Up[1] = 1.0; Up[2] = -1.0;
        updateModelView();
    }

    void Draw() { rb.draw(); }
};

void sol123(Scenario* scene, int op);
Scenario* scene;

float anim = 0;
int girox  = 0;
bool mover = false;
bool bloquear = false;
float dislocar = 0.0;

string randomizex() {
    string res = "";
    srand(time(NULL));
    string pos = "URFDLB";
    for (int i = 0; i < 40; ++i) {
        string xx;
        int d = rand() % 6;
        xx = pos[d % 6];
        res += xx;
    }
    return res;
}
void menu() {
    cout << endl << endl << endl << endl;
    cout << "Algoritmo usado por el Solucionador : Principiante" << endl;
    cout << "" << endl;
    cout << "Q : Giro Cara roja (antihorario)" << endl;
    cout << "A : Giro Cara roja (horario)" << endl;
    cout << "E : Giro Cara amarilla (horario)" << endl;
    cout << "D : Giro Cara amarilla (antihorario)" << endl;
    cout << "R : Giro Cara verde (antihorario)" << endl;
    cout << "F : Giro Cara verde (horario)" << endl;
    cout << "Y : Giro Cara morada (horario)" << endl;
    cout << "H : Giro Cara morada (antihorario)" << endl;
    cout << "J : Giro Cara blanca (horario)" << endl;
    cout << "U : Giro Cara blanca (antihorario)" << endl;
    cout << "O : Giro Cara azul (horario)" << endl;
    cout << "L : Giro Cara azul (antihorario)" << endl;
    cout << endl;
    cout << "Z : Zoom Out" << endl;
    cout << "X : Zoom In" << endl;
    cout << "N : Reiniciar posicion de la camara" << endl;
    cout << "Flechas/Mouse : Movimiento de la Camara" << endl;
    cout << "4 : Resolver el Cubo" << endl;
    return;
}
float sol = false;

string resolver(string solution,vector<int> &movements) {
    string res;
    vector<std::string> tokens;
    movements.clear();

    for (auto i = strtok(&solution[0], " "); i != NULL; i = strtok(NULL, " "))
        tokens.push_back(i);

    for (int i = 0; i < tokens.size(); ++i) {
        string temp = tokens[i];
        if (temp == "R") {
            movements.push_back(6);
        }
        else if (temp == "R'") {
            movements.push_back(15);
        }
        else if (temp == "L") {
            movements.push_back(13);
        }
        else if (temp == "L'") {
            movements.push_back(4);
        }
        else if (temp == "U") {
            movements.push_back(9);
        }
        else if (temp == "U'") {
            movements.push_back(18);
        }
        else if (temp == "D") {
            movements.push_back(16);
        }
        else if (temp == "D'") {
            movements.push_back(7);
        }
        else if (temp == "F") {
            movements.push_back(10);
        }
        else if (temp == "F'") {
            movements.push_back(1);
        }
        else if (temp == "B") {
            movements.push_back(3);
        }
        else if (temp == "B'") {
            movements.push_back(12);
        }
        else if (temp == "R2") {
            movements.push_back(6);
            movements.push_back(6);
        }
        else if (temp == "L2") {
            movements.push_back(13);
            movements.push_back(13);
        }
        else if (temp == "U2") {
            movements.push_back(9);
            movements.push_back(9);
        }
        else if (temp == "D2") {
            movements.push_back(16);
            movements.push_back(16);
        }
        else if (temp == "F2") {
            movements.push_back(10);
            movements.push_back(10);
        }
        else if (temp == "B2") {
            movements.push_back(3);
            movements.push_back(3);
        }
    }

    return res;
}
string Cubo = "";
int solucionar = 0;
bool res = true;

vector<int> movements;
int main()
{
    cout << "Observacion: Al ser el agoritmo principiante, puede demorar bastantes movimientos, aunque se hagan pocos movimientos en el desordenamiento de este.\n";
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

    glEnable(GL_DEPTH_TEST);

    Cubo = randomizex(); 
    
   

    scene = new Scenario();
    cout << endl;
    int opcion=0;
    for (int i = 0; i < Cubo.length(); ++i) {
        char temp = Cubo[i];
        opcion++;
        switch (temp) {
        case ('R'):
            scene->rb.giro6();
            scene->rb.animacion(6, 90);
            break;
        case ('L'):
            scene->rb.giro4inv();
            scene->rb.animacion(13, 90);
            break;
        case ('U'):
            scene->rb.giro9();
            scene->rb.animacion(9, 90);
            break;
        case ('D'):
            scene->rb.giro7inv();
            scene->rb.animacion(16, 90);
            break;
        case ('F'):
            scene->rb.giro1inv();
            scene->rb.animacion(10, 90);
            break;
        case ('B'):
            scene->rb.giro3();
            scene->rb.animacion(3, 90);
            break;
        default:
            break;
        }
    }
    
    /*
    R -> 6
    L -> 4inv
    U -> 9
    D -> 7inv
    F -> 1inv
    B -> 3
    */
    
    glfwSetFramebufferSizeCallback(window, reshape_callback);	//glutReshapeFunc(reshape) ;
    glfwSetKeyCallback(window, key_callback);					//glutKeyboardFunc(keyboard);
    glfwSetMouseButtonCallback(window, mouse_callback);		//glutMouseFunc(mouse) ;
    glfwSetCursorPosCallback(window, mousedrag_callback);		//glutMotionFunc(mousedrag) ;
    // First scene render
    reshape_callback(window, sizeW, sizeH);
    // Start Main Code Render Loop

    /*vector<std::string> tokens;

    for (auto i = strtok(&solution[0], " "); i != NULL; i = strtok(NULL, " "))
        tokens.push_back(i);

    for (int i = 0; i < tokens.size(); ++i) {
        string temp = tokens[i];
        if (temp == "R") {
            movements.push_back(6);
        }
        else if (temp == "R'") {
            movements.push_back(15);
        }
        else if (temp == "L") {
            movements.push_back(13);
        }
        else if (temp == "L'") {
            movements.push_back(4);
        }
        else if (temp == "U") {
            movements.push_back(9);
        }
        else if (temp == "U'") {
            movements.push_back(18);
        }
        else if (temp == "D") {
            movements.push_back(16);
        }
        else if (temp == "D'") {
            movements.push_back(7);
        }
        else if (temp == "F") {
            movements.push_back(10);
        }
        else if (temp == "F'") {
            movements.push_back(1);
        }
        else if (temp == "B") {
            movements.push_back(3);
        }
        else if (temp == "B'") {
            movements.push_back(12);
        }
        else if (temp == "R2") {
            movements.push_back(6);
            movements.push_back(6);
        }
        else if (temp == "L2") {
            movements.push_back(13);
            movements.push_back(13);
        }
        else if (temp == "U2") {
            movements.push_back(9);
            movements.push_back(9);
        }
        else if (temp == "D2") {
            movements.push_back(16);
            movements.push_back(16);
        }
        else if (temp == "F2") {
            movements.push_back(10);
            movements.push_back(10);
        }
        else if (temp == "B2") {
            movements.push_back(3);
            movements.push_back(3);
        }
    }
    */

    menu();
    while (!glfwWindowShouldClose(window)) //glutMainLoop(); 
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        if (!bloquear && solucionar != movements.size() && sol) {
            sol123(scene, movements[solucionar]);
            //cout << movements[solucionar] << " ";
            solucionar++;
            
        }
    
        if (anim == 90.0) {
            anim = 0.0;
            mover = false;
            girox = 0;
            bloquear = false;
        }
        if (mover) {
            anim = anim + speed;
        }

        scene->rb.animacion(girox, speed);

        scene->Draw();
        // glfw: swap buffers and poll IO events (keys pressed/released, mouse moved etc.)
        glfwSwapBuffers(window);
        glfwPollEvents();
    }
  //  cout << scene->rb.movimientos << endl;
    glfwDestroyWindow(window);
    glfwTerminate();
    return 0;
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
    }
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) {
        scene->Move(2, 1, 1);
    }
    if (glfwGetKey(window, GLFW_KEY_Z) == GLFW_PRESS) {
        scene->Zoom(0);
    }
    if (glfwGetKey(window, GLFW_KEY_X) == GLFW_PRESS) {
        scene->Zoom(1);
    }
    if (glfwGetKey(window, GLFW_KEY_N) == GLFW_PRESS) {
        scene->ResetPositions();
    }
    if (glfwGetKey(window, GLFW_KEY_B) == GLFW_PRESS) {
        if (dislocar <= 1.0) {
            dislocar += 0.005f;
            scene->rb.dislocar(dislocar);
        }
    }
    if (glfwGetKey(window, GLFW_KEY_V) == GLFW_PRESS) {
        if (dislocar >= 0.0) {
            dislocar -= 0.005f;
            scene->rb.dislocar(dislocar);
        }
    }

    if (bloquear == false && glfwGetKey(window, GLFW_KEY_4) == GLFW_PRESS) {
        sol = true;
        solucionar = 0;
        //cout << endl << endl << Cubo << endl << endl;
        string solution = TestSolve(Cubo);
        resolver(solution, movements);
        Cubo = "";
    }

    /*
    R -> 6
    L -> 4inv
    U -> 9
    D -> 7inv
    F -> 1inv
    B -> 3
    */
    if (bloquear ) return;
    if (bloquear==false && glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) {
        scene->rb.giro1();
        Cubo += "FFF";
        mover=true; girox = 1;
        bloquear = true;
    }
    if (bloquear == false && glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) {
        scene->rb.giro3();
        Cubo += "B";
        mover=true; girox = 3;
        bloquear = true;
    }
    if (bloquear == false && glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS) {
        scene->rb.giro4();
        Cubo += "LLL";
        mover=true; girox = 4;
        bloquear = true;
    }
    if (bloquear == false && glfwGetKey(window, GLFW_KEY_Y) == GLFW_PRESS) {
        scene->rb.giro6();
        Cubo += "R";
        mover=true; girox = 6;
        bloquear = true;
    }
    if (bloquear == false && glfwGetKey(window, GLFW_KEY_U) == GLFW_PRESS) {
        scene->rb.giro7();
        mover=true; girox = 7;
        bloquear = true;
        Cubo += "DDD";
    }
    if (bloquear == false && glfwGetKey(window, GLFW_KEY_O) == GLFW_PRESS) {
        scene->rb.giro9();
        Cubo += "U";
        mover=true; girox = 9;
        bloquear = true;
    }


    if (bloquear == false && glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
        scene->rb.giro1inv();
        Cubo += "F";
        mover=true; girox = 10;
        bloquear = true;
    }
    if (bloquear == false && glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
        scene->rb.giro3inv();
        Cubo += "BBB";
        mover=true; girox = 12;
        bloquear = true;
    }
    if (bloquear == false && glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS) {
        scene->rb.giro4inv();
        Cubo += "L";
        mover=true; girox = 13;
        bloquear = true;
    }
    if (bloquear == false && glfwGetKey(window, GLFW_KEY_H) == GLFW_PRESS) {
        scene->rb.giro6inv();
        Cubo += "RRR";
        mover=true; girox = 15;
        bloquear = true;
    }
    if (bloquear == false && glfwGetKey(window, GLFW_KEY_J) == GLFW_PRESS) {
        scene->rb.giro7inv();
        Cubo += "D";
        mover=true; girox = 16;
        bloquear = true;
    }
    if (bloquear == false && glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS) {
        scene->rb.giro9inv();
        Cubo += "UUU";
        mover=true; girox = 18;
        bloquear = true;
    }
}

void reshape_callback(GLFWwindow* window, int w, int h) {
    glViewport(0, 0, (GLsizei)w, (GLsizei)h);
    if (h > 0) {
        scene->Projection = glm::perspective(glm::radians(45.0f), (GLfloat)w / (GLfloat)h, 1.0f, 100.0f);
        glUniformMatrix4fv(scene->ProjectionPos, 1, GL_FALSE, &scene->Projection[0][0]);
    }

}

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
        int yloc = (int)y - mouseoldy;  // We will use the y coord to zoom in/out
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


void sol123(Scenario* scene,int op) {
    switch (op)
    {
    case 6:
        scene->rb.giro6();
        mover = true; girox = 6;
        bloquear = true;
        break;
    case 15:
        scene->rb.giro6inv();
        mover = true; girox = 15;
        bloquear = true;
        break;
    case 13:
        scene->rb.giro4inv();
        mover = true; girox = 13;
        bloquear = true;
        break;
    case 4:
        scene->rb.giro4();
        mover = true; girox = 4;
        bloquear = true;
        break;
    case 9:
        scene->rb.giro9();
        mover = true; girox = 9;
        bloquear = true;
        break;
    case 18:
        scene->rb.giro9inv();
        mover = true; girox = 18;
        bloquear = true;
        break;
    case 16:
        scene->rb.giro7inv();
        mover = true; girox = 16;
        bloquear = true;
        break;
    case 7:
        scene->rb.giro7();
        mover = true; girox = 7;
        bloquear = true;
        break;
    case 10:
        scene->rb.giro1inv();
        mover = true; girox = 10;
        bloquear = true;
        break;
    case 1:
        scene->rb.giro1();
        mover = true; girox = 1;
        bloquear = true;
        break;
    case 3:
        scene->rb.giro3();
        mover = true; girox = 3;
        bloquear = true;
        break;
    case 12:
        scene->rb.giro3inv();
        mover = true; girox = 12;
        bloquear = true;
        break;
    default:
        break;
    }
    return;
}