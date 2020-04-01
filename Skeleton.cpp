//=============================================================================================
// Mintaprogram: Z�ld h�romsz�g. Ervenyes 2019. osztol.
//
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat, BOM kihuzando.
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni a printf-et kiveve
// - Mashonnan atvett programresszleteket forrasmegjeloles nelkul felhasznalni es
// - felesleges programsorokat a beadott programban hagyni!!!!!!!
// - felesleges kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan OpenGL fuggvenyek hasznalhatok, amelyek az oran a feladatkiadasig elhangzottak
// A keretben nem szereplo GLUT fuggvenyek tiltottak.
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    :
// Neptun :
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem.
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem,
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================
#include "framework.h"

// vertex shader in GLSL: It is a Raw string (C++11) since it contains new line characters
const char* const vertexSource = R"(
    #version 330                // Shader 3.3
    precision highp float;        // normal floats, makes no difference on desktop computers

    uniform mat4 MVP;            // uniform variable, the Model-View-Projection transformation matrix
    layout(location = 0) in vec2 vp;    // Varying input: vp = vertex position is expected in attrib array 0

    void main() {
        gl_Position = vec4(vp.x, vp.y, 0, 1) * MVP;        // transform vp from modeling space to normalized device space
    }
)";

// fragment shader in GLSL
const char* const fragmentSource = R"(
    #version 330            // Shader 3.3
    precision highp float;    // normal floats, makes no difference on desktop computers
    
    uniform vec3 color;        // uniform variable, the color of the primitive
    out vec4 outColor;        // computed color of the current pixel

    void main() {
        outColor = vec4(color, 1);    // computed color is the color of the primitive
    }
)";

GPUProgram gpuProgram;
unsigned int vao;
const static int tessellationLevel = 500;

class Circle {
    const static int numberOfVertices = tessellationLevel;
    unsigned int vao, vbo;
    vec2 vertices[numberOfVertices];
public:
    void Create(vec2 center, float radius, float from = 0.0f, float length = 2.0f * M_PI ) {
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);

        for(int i = 0; i < numberOfVertices; i++) {
            float fi = from + i * length / numberOfVertices;
            vertices[i] = vec2(cosf(fi) * radius, sinf(fi) * radius) + center;
        }
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * numberOfVertices, vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, reinterpret_cast<void*>(0)); 		     // stride, offset: tightly packed
    }

    void Draw(GLenum mode, vec3 color) {
        int location = glGetUniformLocation(gpuProgram.getId(), "color");
        glUniform3f(location, color.x, color.y, color.z); 
        float MVPtransf[4][4] = {
                            1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1};

        location = glGetUniformLocation(gpuProgram.getId(), "MVP"); 
        glUniformMatrix4fv(location, 1, GL_TRUE, &MVPtransf[0][0]);
        glBindVertexArray(vao);
        glDrawArrays(mode, 0, numberOfVertices);
    }
    vec2* GetVertices()
    {
        return vertices;
    }
};

class SiriusTriangle
{
    vec2 points[3];
    Circle circles[3];
    int numberOfPoints = 0;
    bool calculated = false;
    unsigned int vao, vbo;
    vec2 vertices[3 * tessellationLevel + 1];

    vec2 FindCircleCenter(vec2 p1, vec2 p2) 
    {
        vec2 c;
        c.y = (
                0.5f - 
                (p2.x + p1.y * p1.y * p2.x) / (2.0f * p1.x) + 
                (p2.x * p2.x + p2.y * p2.y - p1.x * p2.x) / 2.0f
              ) / (p2.y - p1.y * p2.x / p1.x);
        c.x =  (1.0f + p1.y*p1.y) / (2.0f * p1.x) + p1.x / 2.0f - (p1.y * c.y) / p1.x;
        return c;
    }
    float CalculateCircleRadius(vec2 center)
    {
        return sqrt(length(center)*length(center)-1.0f);
    }
    float ChooseFromRadian(float a, float b)
    {
        return fabs(a-b) > M_PI ? b : a;
    }
    float ChooseToRadian(float a, float b)
    {
        return fabs(a-b) > M_PI ? a : b;
    }
public:
    void AddPoint(float cX, float cY)
    {
        if(numberOfPoints < 3) {
            points[numberOfPoints++] = vec2(cX, cY);
        }
        if (numberOfPoints == 3 && !calculated)
        {
            calculated = true;
            vec2 A = points[0];
            vec2 B = points[1];
            vec2 C = points[2];

            vec2 aCenter = FindCircleCenter(A,B);
            vec2 bCenter = FindCircleCenter(B,C);
            vec2 cCenter = FindCircleCenter(C,A);

            float aRadius = CalculateCircleRadius(aCenter);
            float bRadius = CalculateCircleRadius(bCenter);
            float cRadius = CalculateCircleRadius(cCenter);

            float aFi1 = atan2(B.y - aCenter.y, B.x - aCenter.x);
            float aFi2 = atan2(A.y - aCenter.y, A.x - aCenter.x);

            float bFi1 = atan2(C.y - bCenter.y, C.x - bCenter.x);
            float bFi2 = atan2(B.y - bCenter.y, B.x - bCenter.x);
            
            float cFi1 =  atan2(A.y - cCenter.y, A.x - cCenter.x);
            float cFi2 =  atan2(C.y - cCenter.y, C.x - cCenter.x);
            
            float aFrom = ChooseFromRadian(aFi1, aFi2);
            float aTo = ChooseToRadian(aFi1, aFi2);
            float bFrom = ChooseFromRadian(bFi1, bFi2);
            float bTo = ChooseToRadian(bFi1, bFi2);
            float cFrom = ChooseFromRadian(cFi1, cFi2);
            float cTo = ChooseToRadian(cFi1, cFi2);

            circles[0].Create(aCenter, aRadius, aFrom, (aTo - aFrom));
            circles[1].Create(bCenter, bRadius, bFrom, (bTo - bFrom));
            circles[2].Create(cCenter, cRadius, cFrom, (cTo - cFrom));

            printf("Az 'a' oldal hossza: %3.2f\n", aRadius*fabs(aTo - aFrom));
            printf("A 'b' oldal hossza: %3.2f\n", aRadius*fabs(bTo - bFrom));
            printf("A 'c' oldal hossza: %3.2f\n", aRadius*fabs(cTo - cFrom));
        }
    }

    void Draw()
    {
        vec2 centerOfTriangle = vec2(0.0f, 0.0f);
        for(int i = 0; i < 3; i++)
        {
            centerOfTriangle.x = centerOfTriangle.x + points[i].x;
            centerOfTriangle.y = centerOfTriangle.y + points[i].y;
        }
        centerOfTriangle.x = centerOfTriangle.x / 3;
        centerOfTriangle.y = centerOfTriangle.y / 3;
        
        vertices[0] = centerOfTriangle;
        for(int n = 0; n < 3; n++)
        {
            vec2* aCircleVertices = new vec2[tessellationLevel];
            aCircleVertices = circles[n].GetVertices();
            int j = 0;
            for(int i = n * tessellationLevel + 1; i <= (n + 1) * tessellationLevel; i++ && j++)
            {
                vertices[i] = aCircleVertices[tessellationLevel-j];
            }
        }
        
        glGenVertexArrays(1, &vao);
        glBindVertexArray(vao);

        glGenBuffers(1, &vbo);
        glBindBuffer(GL_ARRAY_BUFFER, vbo);
        
        glBufferData(GL_ARRAY_BUFFER, sizeof(vec2) * 3 * tessellationLevel + 1, vertices, GL_STATIC_DRAW);

        glEnableVertexAttribArray(0);
        glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 2, reinterpret_cast<void*>(0)); 		     // stride, offset: tightly packed
    
        int location = glGetUniformLocation(gpuProgram.getId(), "color");
        glUniform3f(location, 0.0f, 1.0f, 0.0f); 
        float MVPtransf[4][4] = {
                            1, 0, 0, 0,
                            0, 1, 0, 0,
                            0, 0, 1, 0,
                            0, 0, 0, 1};

        location = glGetUniformLocation(gpuProgram.getId(), "MVP"); 
        glUniformMatrix4fv(location, 1, GL_TRUE, &MVPtransf[0][0]);
        glBindVertexArray(vao);
        glDrawArrays(GL_TRIANGLE_FAN, 0, 3 * tessellationLevel + 1);
        
        for(int i = 0; i < 3; i++)
        {
            circles[i].Draw(GL_LINE_STRIP, vec3(1.0f, 0.0f, 0.0f));
        }
    }
};

Circle unitCircle;
SiriusTriangle siriusTriangle;

void onInitialization()
{
    glViewport(0, 0, windowWidth, windowHeight);
    unitCircle.Create(vec2(0.0f,0.0f), 1.0f);
    gpuProgram.create(vertexSource, fragmentSource, "outColor");
}

void onDisplay()
{
    glClearColor(0, 0, 0, 0);
    glClear(GL_COLOR_BUFFER_BIT);
    unitCircle.Draw(GL_TRIANGLE_FAN, vec3(0.8f, 0.9f, 0.9f));
    siriusTriangle.Draw();
    glutSwapBuffers();
}

void onMouse(int button, int state, int pX, int pY)
{ 
    float cX = 2.0f * pX / windowWidth - 1;
    float cY = 1.0f - 2.0f * pY / windowHeight;

    if (state == GLUT_UP && button == GLUT_LEFT_BUTTON) 
    {
        siriusTriangle.AddPoint(cX, cY);
        glutPostRedisplay();
    }
}

void onKeyboard(unsigned char key, int pX, int pY) {}
void onKeyboardUp(unsigned char key, int pX, int pY) {}
void onMouseMotion(int pX, int pY) {}
void onIdle() {}
