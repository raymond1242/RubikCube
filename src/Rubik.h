float Colors[7][4] = {
    { 1.0f, 0.0f, 0.0f, 1.0f },//0 red
    { 0.4f, 0.0f, 0.8f, 0.7f },//1 purple
    { 1.0f, 1.0f, 0.0f, 1.0f },//2 yellow
    { 0.0f, 1.0f, 0.0f, 1.0f },//3 green
    { 1.0f, 1.0f, 1.0f, 1.0f },//4 white
    { 0.0f, 0.0f, 1.0f, 1.0f },//5 blue
    { 0.1f, 0.1f, 0.1f, 1.0f }//6 black
};

unsigned int IndexColorsCube[27][6] = {
    {5,6,6,3,4,6},{5,6,6,3,6,6},{5,6,2,3,6,6},{5,6,6,6,4,6},{5,6,6,6,6,6},{5,6,2,6,6,6},{5,0,6,6,4,6},{5,0,6,6,6,6},{5,0,2,6,6,6},
    {6,6,6,3,4,6},{6,6,6,3,6,6},{6,6,2,3,6,6},{6,6,6,6,4,6},{6,6,6,6,6,6},{6,6,2,6,6,6},{6,0,6,6,4,6},{6,0,6,6,6,6},{6,0,2,6,6,6},
    {6,6,6,3,4,1},{6,6,6,3,6,1},{6,6,2,3,6,1},{6,6,6,6,4,1},{6,6,6,6,6,1},{6,6,2,6,6,1},{6,0,6,6,4,1},{6,0,6,6,6,1},{6,0,2,6,6,1}
};

struct Cube {
    unsigned int VBO, VAO;
    float radio;
    int numFaces;
    int Shader;
    int vertexLocation;
    float vertexs[24];
    unsigned int indexs[6][6];
    float o[3] = { 0.0,0.0,0.0 };
    unsigned int IBO[6] = { 0,0,0,0,0,0 };
    unsigned int ColorFaces[6] = { 0,0,0,0,0,0 };

    Cube() {
        radio = 0.2f; numFaces = 6; Shader = 0; vertexLocation = 0; VBO = 0; VAO = 0;
        unsigned int CubeIndex[6][6] = {
            {0, 1, 2, 0, 2, 3},
            {4, 0, 3, 4, 3, 7},
            {0, 5, 1, 0, 5, 4},
            {1, 2, 6, 1, 5, 6},
            {2, 6, 7, 2, 3, 7},
            {7, 5, 4, 7, 6, 5}
        };
        for (int i = 0; i < numFaces; ++i) {
            for (int j = 0; j < numFaces; ++j) { indexs[i][j] = CubeIndex[i][j]; }
        }
    }

    void pasteColor(unsigned int temp[6]) {
        for (int i = 0; i < numFaces; ++i) {
            ColorFaces[i] = temp[i];
        }
    }

    void AllocateVertexsBuffers() {
        vertexs[0] = o[0] + radio; vertexs[1] = o[1] + radio; vertexs[2] = o[2] - radio;
        vertexs[3] = o[0] - radio; vertexs[4] = o[1] + radio; vertexs[5] = o[2] - radio;
        vertexs[6] = o[0] - radio; vertexs[7] = o[1] - radio; vertexs[8] = o[2] - radio;
        vertexs[9] = o[0] + radio; vertexs[10] = o[1] - radio; vertexs[11] = o[2] - radio;
        vertexs[12] = o[0] + radio; vertexs[13] = o[1] + radio; vertexs[14] = o[2] + radio;
        vertexs[15] = o[0] - radio; vertexs[16] = o[1] + radio; vertexs[17] = o[2] + radio;
        vertexs[18] = o[0] - radio; vertexs[19] = o[1] - radio; vertexs[20] = o[2] + radio;
        vertexs[21] = o[0] + radio; vertexs[22] = o[1] - radio; vertexs[23] = o[2] + radio;
    }

    void UpdateBuffers() {
        glBindVertexArray(VAO);
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferData(GL_ARRAY_BUFFER, sizeof(vertexs), vertexs, GL_STATIC_DRAW);
        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), 0);

        for (int i = 0; i < numFaces; ++i) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[i]);
            glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indexs[i]), indexs[i], GL_STATIC_DRAW);
        }

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        glBindVertexArray(0);
    }

    void SetUp(float x, float y, float z, unsigned int fColors[6],int ShaderProgram) {
        o[0] = x; o[1] = y; o[2] = z;
        Shader = ShaderProgram;
        pasteColor(fColors);
        vertexLocation = glGetUniformLocation(Shader, "Color");

        AllocateVertexsBuffers();
        glGenVertexArrays(1, &VAO);
        glGenBuffers(1, &VBO);
        glGenBuffers(6, IBO);
        UpdateBuffers();
    }

    void SetDistance(float x, float y, float z) {
        o[0] = x; o[1] = y; o[2] = z;
        AllocateVertexsBuffers();
        UpdateBuffers();
    }

    void draw() {
        glBindVertexArray(VAO);

        for (int i = 0; i < numFaces; ++i) {
            glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, IBO[i]);
            glUniform4f(vertexLocation, Colors[ColorFaces[i]][0], Colors[ColorFaces[i]][1], Colors[ColorFaces[i]][2], Colors[ColorFaces[i]][3]);
            glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        }
    }

    void Delete() {
        glDeleteVertexArrays(1, &VAO);
        glDeleteBuffers(1, &VBO);
        glDeleteBuffers(6, IBO);
    }
};

struct Rubik {
    double radio;
    int numCubes;
    float o[3] = { 0.0, 0.0, 0.0 };
    Cube Cubos[27];
    int Shader;
    float distance;
    Rubik() {
        radio = 0.6; numCubes = 27; Shader = 0; distance = 0.405f;
    }

    void SetUp(int ShaderProgram) {
        Shader = ShaderProgram;

        float t1 = -distance;
        for (int i = 0; i < 3; ++i) {
            float t2 = -distance;
            for (int j = 0; j < 3; ++j) {
                float t3 = -distance;
                for (int k = 0; k < 3; ++k) {
                    Cubos[(i * 9) + (j * 3) + k].SetUp(t2, t3, t1, IndexColorsCube[(i * 9) + (j * 3) + k], Shader);
                    t3 += distance;
                }
                t2 += distance;
            }
            t1 += distance;
        }
    }

    void draw() {
        for (int i = 0; i < numCubes; ++i) {
            Cubos[i].draw();
        }
    }

    void ModifyDistance(bool t) {
        if (t) {
            distance += 0.005f;
        }
        else {
            if (distance > 0.4) distance -= 0.005f;
        }

        float t1 = -distance;
        for (int i = 0; i < 3; ++i) {
            float t2 = -distance;
            for (int j = 0; j < 3; ++j) {
                float t3 = -distance;
                for (int k = 0; k < 3; ++k) {
                    Cubos[(i * 9) + (j * 3) + k].SetDistance(t2, t3, t1);
                    t3 += distance;
                }
                t2 += distance;
            }
            t1 += distance;
        }
    }

    void Delete() {
        for (int i = 0; i < numCubes; ++i) {
            Cubos[i].Delete();
        }
    }
};
