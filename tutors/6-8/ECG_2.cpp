#include <stdio.h>
#include <string.h>
#include <assert.h>
#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "Header.h"

/*  
*   Область буфера памяти, созданная в области памяти графической карты, которая используется для 
*   хранения различных типов информации атрибутов вершин, таких как координаты вершин, векторы 
*   вершин и данные цвета вершин. Во время рендеринга различные атрибутные данные вершин могут быть
*   взяты непосредственно из VBO. Поскольку VBO находится в видеопамяти, а не в памяти, ему не нужно   
*   передавать данные из CPU, и эффективность обработки выше.
*/
int cycle;

GLuint VBO; // "Объект буфера вершин"

GLuint gWorldLocation;
// Вершиный шейдер решает, где рисовать граф 
static const char* pVS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
layout (location = 0) in vec3 Position;                                             \n\
                                                                                    \n\
uniform mat4 gWorld;                                                                \n\
                                                                                    \n\
out vec4 Color;                                                                     \n\
void main()                                                                         \n\
{                                                                                   \n\
    gl_Position = gWorld * vec4(Position, 1.0);                                     \n\
    Color = vec4(clamp(Position, 0.1, 2.0), 1.0);                                   \n\
}";
// Фрагментный шейдер определяет цвет графа, который будет нарисован
static const char* pFS = "                                                          \n\
#version 330                                                                        \n\
                                                                                    \n\
in vec4 Color;                                                                      \n\
out vec4 FragColor;                                                                 \n\
                                                                                    \n\
void main()                                                                         \n\
{                                                                                   \n\
    FragColor = Color;                                                              \n\
}";

static void RenderSceneCB()
{
    glClear(GL_COLOR_BUFFER_BIT);

    static float Scale = 0.0f;//Переменная, которая будет изменятся 

    Scale += 0.0005f;//Скорость изменения 

    Matrix4f World;//Переменная структуры матрицы
    
    if ((Scale / 6) >= 0)cycle = 1;
    if ((Scale / 6) >= 1)cycle = 2;
    if ((Scale / 6) >= 2)cycle = 3;
    // Для перемещения используется формула 
    //{ x+v1}
    //{ y+v2}
    //{ z+v3}
    //{ w+1 }
   
    //Для вращения используется формула 
    // x двигается по косинусу, y по синусу 
    //при увеличение угла x уменьшается, y увеличивается
    // x2 = x1cosa2 - y1sina2 
    // y2 = y1sina2 + x1cosa2
    // scs
    cycle = 1;
    if(cycle == 1)
    {
    World.m[0][0] = fabs(sinf(Scale)); World.m[0][1] = 0.0f; World.m[0][2] = 0.0f; World.m[0][3] = 0.0f;
    World.m[1][0] = 0.0f; World.m[1][1] = fabs(sinf(Scale)); World.m[1][2] = 0.0f; World.m[1][3] = 0.0f;
    World.m[2][0] = 0.0f; World.m[2][1] = 0.0f; World.m[2][2] = fabs(sinf(Scale)); World.m[2][3] = 0.0f;
    World.m[3][0] = 0.0f; World.m[3][1] = 0.0f; World.m[3][2] = 0.0f; World.m[3][3] = 1.0f;
    }
    if (cycle == 2)
    {
        World.m[0][0] = 1.0f; World.m[0][1] = 0.0f; World.m[0][2] = 0.0f; World.m[0][3] = sinf(Scale);
        World.m[1][0] = 0.0f; World.m[1][1] = 1.0f; World.m[1][2] = 0.0f; World.m[1][3] = cosf(Scale);
        World.m[2][0] = 0.0f; World.m[2][1] = 0.0f; World.m[2][2] = 1.0f; World.m[2][3] = 0.0f;
        World.m[3][0] = 0.0f; World.m[3][1] = 0.0f; World.m[3][2] = 0.0f; World.m[3][3] = 1.0f;
    }
    if (cycle == 3)
    {
        World.m[0][0] = cosf(Scale); World.m[0][1] = -sinf(Scale); World.m[0][2] = 0.0f; World.m[0][3] = 0.0f;
        World.m[1][0] = sinf(Scale); World.m[1][1] = cosf(Scale); World.m[1][2] = 0.0f; World.m[1][3] = 0.0f;
        World.m[2][0] = 0.0f; World.m[2][1] = 0.0f; World.m[2][2] = 0.0f; World.m[2][3] = 0.0f;
        World.m[3][0] = 0.0f; World.m[3][1] = 0.0f; World.m[3][2] = 0.0f; World.m[3][3] = 1.0f;
    }
    // Передача значения переменной в шейдер
    // 1 - Определяет местоположение юниформ - значения, которое нужно изменить.
    // 2 - Кол-во матриц, которое будет изменено
    // 3 - Указывает, следует ли транспонировать матрицу при загрузке значений в юниформ-переменную.
    // 4 - Указатель на массив , который будет использоваться для обновления указанной юниформ-переменной.
    glUniformMatrix4fv(gWorldLocation, 1, GL_TRUE, &World.m[0][0]);

    //Включить универсальный массив атрибутов вершин
    glEnableVertexAttribArray(0);   //Задаем индекс универсального атрибута вершины,
                                    //который необходимо включить или отключить.
    
     //Связка "Объект буфера вершин" с назначенным идентификатором (В нашем случае GL_ARRAY_BUFFER)
    // GL_ARRAY_BUFFER, который используется для привязки массива вершин.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);

    //  Cообщаем OpenGL'у, как интерпретировать данные вершин
    //  1 - положение атрибута вершин, 2 - размер атрибута вершин
    //  3 - тип данных, 4 - нормализовать атрибуты перед использованием в конвейере
    // 5 - размер шага определяющий интервал между последовательными атрибутами 
    // вершин, 6 - cмещение данных о положении от начала буфера
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);

    //Визуализация примитивов из данных массива
    // 1 - Указываем, какие примитивы отображать, 2 - задаем начальный индекс во
    // включенных массивах, 3 - указываем количество отображаемых индексов.
    glDrawArrays(GL_TRIANGLES, 0, 3);

    //Отключить универсальный массив атрибутов вершин
    glDisableVertexAttribArray(0);

    glutSwapBuffers();
}


static void InitializeGlutCallbacks()
{
    glutDisplayFunc(RenderSceneCB);
    glutIdleFunc(RenderSceneCB);
}

static void CreateVertexBuffer()
{
    Vector3f Vertices[3];
    Vertices[0] = Vector3f(-1.0f, -1.0f, 0.0f);
    Vertices[1] = Vector3f(1.0f, -1.0f, 0.0f);
    Vertices[2] = Vector3f(0.0f, 1.0f, 0.0f);
        
    //Cоздание имен объектов буфера 
    // 1 - количество создаваемых буферных объектов 
    // 2 - ссылка на массив, который будет использоваться для хранения имен буферных объектов
    glGenBuffers(1, &VBO);  

    //Связка "Объект буфера вершин" с назначенным идентификатором (В нашем случае GL_ARRAY_BUFFER)
    // GL_ARRAY_BUFFER, который используется для привязки массива вершин.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    //   Передача пользовательских данных в текущий связанный буфер видеопамяти, где
    //   1 - тип VBO, 2 - рамзер VBO, 3 - данные для передачи, 4 - метод хранения VBO(GL_STATIC_DRAW)
    //  STATIC: Содержимое хранилища данных будет изменено один раз и использовано несколько раз
    //  DRAW: Приложение изменяет содержимое хранилища данных и использует его в качестве источника
    //  для команд рисования GL и спецификации изображения
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
    

}

static void AddShader(GLuint ShaderProgram, const char* pShaderText, GLenum ShaderType)
{
    //Создаем объект шейдера 
    GLuint ShaderObj = glCreateShader(ShaderType);//Объект шейдера 
    if (ShaderObj == 0) {
        fprintf(stderr, "Error creating shader type %d\n", ShaderType);
        exit(0);
    }
    
    //Компилируем исходный код шейдера объекта
    const GLchar* p[1];
    p[0] = pShaderText;//Данные исходного кода 
    GLint Lengths[1];
    Lengths[0] = strlen(pShaderText);//Длина соответствующего исходного кода 
    glShaderSource(ShaderObj, 1, p, Lengths);// Связываем исходный код шейдера с объектом шейдера
    glCompileShader(ShaderObj);//Компилируем объект шейдера

    //Убеждаемся, что компиляция шейдера прошла успешно 
    GLint success;
    //Возвращает параметр объекта шейдера в переменую-парматер. В нашем случае мы указали GL_COMPILE_STATUS, что
    //возратит GL_TRUE, если последняя операция компиляции шейдера была успешной, 
    //и GL_FALSE в противном случае, в переменную-параметр(success) 
    glGetShaderiv(ShaderObj, GL_COMPILE_STATUS, &success); 
    if (!success)//Если не успешно
    {
        GLchar InfoLog[1024];//длина кэша с длиной строк для значения, возращаемое журналом
        //возвращает скомпилированный результат шейдера 
        // 1 - Объект шейдера, 2 -  размер символьного буфера для хранения журнала 
        // возвращаемой информации, 3 - возвращает указатель на длину строки, возвращаемой в 
        // infoLog, 4 - указатель на массив символов, который используется для возврата 
        // информационного журнала.
        glGetShaderInfoLog(ShaderObj, 1024, NULL, InfoLog);
        fprintf(stderr, "Error compiling shader type %d: '%s'\n", ShaderType, InfoLog);
        exit(1);
    }

    glAttachShader(ShaderProgram, ShaderObj);//Присоединяет объект шейдера к программному шейдеру
}

static void CompileShaders()
{
    //создает пустую программу шейдера, возвращаемое значение - ненулевое целое число, 
    //если оно равно 0, произошла ошибка
    GLuint ShaderProgram = glCreateProgram();

    if (ShaderProgram == 0) {
        fprintf(stderr, "Error creating shader program\n");
        exit(1);
    }

    AddShader(ShaderProgram, pVS, GL_VERTEX_SHADER);//Тип шейдера - вершинный 
    AddShader(ShaderProgram, pFS, GL_FRAGMENT_SHADER);//Тип шейдера - фрагментный 

    GLint Success = 0;
    GLchar ErrorLog[1024] = { 0 };
   
    glLinkProgram(ShaderProgram);//Связываем программу шейдера

    //Возвращает параметр объекта шейдера в переменую-парматер. В нашем случае мы указали GL_LINK_STATUS, что
    //возратит GL_TRUE, если последняя операция компиляции шейдера была успешной, 
    //и GL_FALSE в противном случае, в переменную-параметр(Success) 
    glGetProgramiv(ShaderProgram, GL_LINK_STATUS, &Success);
    if (Success == 0)//Если false, то
    {
        //возвращает скомпилированный результат шейдера 
       // 1 - Объект шейдера, 2 -  размер символьного буфера для хранения журнала 
       // возвращаемой информации, 3 - возвращает указатель на длину строки, возвращаемой в 
       // infoLog, 4 - указатель на массив символов, который используется для возврата 
       // информационного журнала.
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Error linking shader program: '%s'\n", ErrorLog);
        exit(1);
    }
    glValidateProgram(ShaderProgram);//Проверка прогаммы шейдера
    glGetProgramiv(ShaderProgram, GL_VALIDATE_STATUS, &Success);
    if (!Success) {
        glGetProgramInfoLog(ShaderProgram, sizeof(ErrorLog), NULL, ErrorLog);
        fprintf(stderr, "Invalid shader program: '%s'\n", ErrorLog);
        exit(1);
    }

    glUseProgram(ShaderProgram);//Используем программу шейдера

    //Возвращает целое число, чтобы указать положение определенной равномерной переменной в программном объекте
    gWorldLocation = glGetUniformLocation(ShaderProgram, "gWorld");
    assert(gWorldLocation != 0xFFFFFFFF);//Сообщение об ошибке 
}
int main(int argc, char** argv)
{
    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);
    glutInitWindowSize(1024, 768);
    glutInitWindowPosition(100, 100);
    glutCreateWindow("Tutorial 06");
    
    InitializeGlutCallbacks();

    // Must be done after glut is initialized!
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);

    CreateVertexBuffer();

    CompileShaders();

    glutMainLoop();

    return 0;
}