
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include "math_3d.h"

GLuint VBO;//Переменная для хранения указателя на буфер вершин

static void RenderSceneCB(void)
{

    /*  Аргумент функции glClear() определяет надлежащий буфер. В данном случае программа отчистит только цветовой 
     *  буфер, в котором хранится изображение, выводимое на экран. 
     * 
    */

    glClear(GL_COLOR_BUFFER_BIT);// Очищаем все пиксели установленным цветом 

    glEnableVertexAttribArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);//привязываем буфер, приготавливая его для отрисовки
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);//говорим конвейеру как воспринимать данные внутри буфера

    glDrawArrays(GL_TRIANGLES, 0, 3); //функция для отрисовки

    glDisableVertexAttribArray(0);//количество вершин для отрисовки.

    glutSwapBuffers();  //  Задерживает обновление экрана до окончания показа предыдущего буфера.
                        //  Также позволяет показывать буфер с самого начала целиком 
}

/*  Так как мы начали работать в оконной системе, большая часть действий с запущенной программой происходит через функции обратного вызова.
    GLUT берёт на себя взаимодействие с оконной системой и предоставляет нам несколько вариантов обратного вызова.
    Пока что мы используем только один, самый главный, который отрисовывает 1 кадр.Эта функция регулярно вызывается GLUT'ом                     */
static void InitializeGlutCallbacks()
{
    glutDisplayFunc(RenderSceneCB); //  Функция обратной связи. Когда GLUT определяет, какое содержимое окна нужно перерисовать,
                                    //  выполняется функция обратной связи, зарегистрированная с помощью glutDisplayFunc()
}

static void CreateVertexBuffer()
{
    Vector3f Vertices[3];// экзмепляр структуры Vector3f
    Vertices[0] = Vector3f(0.0f, 1.0f, 0.0f); //т. треуголньика
    Vertices[1] = Vector3f(1.0f, 0.0f, 0.0f);//т. треуголньика (x,y,z)
    Vertices[2] = Vector3f(0.0f, -1.0f, 0.0f);//т. треуголньика

    glGenBuffers(1, &VBO);//Функция для генерации объектов переменных типов.
    glBindBuffer(GL_ARRAY_BUFFER, VBO);//Привязывание объекта
    glBufferData(GL_ARRAY_BUFFER, sizeof(Vertices), Vertices, GL_STATIC_DRAW);
}


int main(int argc, char** argv)
{
    glutInit(&argc, argv);//Инициализация GLUT и обрабатывает любые параметры командной строки
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA);//настраиваются некоторые опции GLUT (двойная буферизация/модель цвета)
    glutInitWindowSize(1600, 900);//Определяет размер окна в пикселях 
    glutInitWindowPosition(10, 10);//Определяет положение левого верхнего угла окна
    glutCreateWindow("Lab1");//Создаёт окно с содержимым OpenGL. Она возвращает уникальный идентификатор нового окна
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);//устанавливает цвет, который будет использован во время очистки буфера кадра
    glColor3f(0.0, 0.0, 1.0);//устанавливает цвет, который будет использован для отрисовывания
    InitializeGlutCallbacks();

    //инициализируем GLEW и проверяем на ошибки
    GLenum res = glewInit();
    if (res != GLEW_OK) {
        fprintf(stderr, "Error: '%s'\n", glewGetErrorString(res));
        return 1;
    }

    CreateVertexBuffer();
   
    glutMainLoop(); //  Передаёт контроль GLUT'у, который теперь начнёт свой собственный цикл.
                    //  После этого будут показаны все созданные окна вместе с их содержимым.
                    //  Начнётся обработка событий, и сработает привязка функций обратной 
                    //  связи. После того как цикл обработки начнет работать, он никогда не 
                    //  прекратится!
    return 0;
}