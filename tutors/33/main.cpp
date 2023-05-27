#include <math.h>
#include <GL/glew.h>
#include <GL/freeglut.h>

#include "engine_common.h"
#include "util.h"
#include "pipeline.h"
#include "camera.h"
#include "texture.h"
#include "lighting_technique.h"
#include "glut_backend.h"
#include "mesh.h"
#ifdef FREETYPE
#include "freetypeGL.h"
#endif

#define WINDOW_WIDTH  1280  
#define WINDOW_HEIGHT 1024

#define NUM_ROWS 50
#define NUM_COLS 20
#define NUM_INSTANCES NUM_ROWS * NUM_COLS


class Tutorial33 : public ICallbacks
{
public:

    // Конструктор класса Tutorial33
    Tutorial33()
    {
        // Устанавливаем указатели на нулевые значения
        m_pGameCamera = NULL;
        m_pEffect = NULL;
        m_pMesh = NULL;
        // Устанавливаем значения шкалы, параметров направленного света и матрицы проекции
        m_scale = 0.0f;
        m_directionalLight.Color = Vector3f(1.0f, 1.0f, 1.0f);
        m_directionalLight.AmbientIntensity = 0.55f;
        m_directionalLight.DiffuseIntensity = 0.9f;
        m_directionalLight.Direction = Vector3f(1.0f, 0.0, 0.0);
        m_persProjInfo.FOV = 60.0f;
        m_persProjInfo.Height = WINDOW_HEIGHT;
        m_persProjInfo.Width = WINDOW_WIDTH;
        m_persProjInfo.zNear = 1.0f;
        m_persProjInfo.zFar = 100.0f;

        // Устанавливаем значения счетчика кадров и FPS
        m_frameCount = 0;
        m_fps = 0.0f;
    }

    ~Tutorial33()
    {
        SAFE_DELETE(m_pEffect);
        SAFE_DELETE(m_pGameCamera);
        SAFE_DELETE(m_pMesh);
    }

    // Определяем функцию Init, которая инициализирует объекты и переменные для приложения.
    bool Init()
    {
        // Определяем позицию камеры - вектор Pos.
        Vector3f Pos(7.0f, 3.0f, 0.0f);
        // Определяем целевую точку камеры - вектор Target.
        Vector3f Target(0.0f, -0.2f, 1.0f);

        // Определяем вектор направления вверх относительно камеры - вектор Up.
        Vector3f Up(0.0, 1.0f, 0.0f);

        // Создаем новый объект Camera и инициализируем его параметрами Pos, Target и Up.
        m_pGameCamera = new Camera(WINDOW_WIDTH, WINDOW_HEIGHT, Pos, Target, Up);

        // Создаем новый объект LightingTechnique, который представляет собой технику освещения.
        m_pEffect = new LightingTechnique();

        // Инициализируем объект LightingTechnique. Если инициализация не удалась, выводим сообщение об ошибке и возвращаем false.
        if (!m_pEffect->Init()) {
            printf("Error initializing the lighting technique\n");
            return false;
        }

        // Активируем объект LightingTechnique.
        m_pEffect->Enable();

        // Устанавливаем текстурный юнит цвета на COLOR_TEXTURE_UNIT_INDEX.
        m_pEffect->SetColorTextureUnit(COLOR_TEXTURE_UNIT_INDEX);

        // Устанавливаем параметры направленного света.
        m_pEffect->SetDirectionalLight(m_directionalLight);

        // Устанавливаем интенсивность зеркального отражения материала.
        m_pEffect->SetMatSpecularIntensity(0.0f);

        // Устанавливаем степень зеркального отражения материала.
        m_pEffect->SetMatSpecularPower(0);

        // Задаем цвета для каждого из 4-х фрагментных шейдеров (для подсветки).
        m_pEffect->SetColor(0, Vector4f(1.0f, 0.5f, 0.5f, 0.0f));
        m_pEffect->SetColor(1, Vector4f(0.5f, 1.0f, 1.0f, 0.0f));
        m_pEffect->SetColor(2, Vector4f(1.0f, 0.5f, 1.0f, 0.0f));
        m_pEffect->SetColor(3, Vector4f(1.0f, 1.0f, 1.0f, 0.0f));

        // Создаем новый объект Mesh.
        m_pMesh = new Mesh();

        // Загружаем модель паука. Если загрузка не удалась, возвращаем false.
        if (!m_pMesh->LoadMesh("../../../Content/spider.obj")) {
            return false;
        }
#ifdef FREETYPE
        // Если определен макрос FREETYPE, то инициализируем шрифтовый рендерер, чтобы можно было выводить на экран текст.
        if (!m_fontRenderer.InitFontRenderer()) {
            return false;
        }
#endif
        // Инициализируем переменную m_time текущим временем.
        m_time = glutGet(GLUT_ELAPSED_TIME);

        // Вызываем функцию CalcPositions для расчета позиций объектов сцены.
        CalcPositions();

        // Возвращаем true, если все объекты и переменные были успешно инициализированы.
        return true;
    }

    void Run()
    {
        GLUTBackendRun(this);
    }


    // Метод для отрисовки сцены
    virtual void RenderSceneCB()
    {
        // Вычисляем количество кадров в секунду
        CalcFPS();
        // Увеличиваем масштаб сцены на 0.005f
        m_scale += 0.005f;

        // Обновляем матрицу вида камеры
        m_pGameCamera->OnRender();

        // Очищаем буфер экрана
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Включаем шейдер
        m_pEffect->Enable();

        // Устанавливаем позицию камеры в мировых координатах
        m_pEffect->SetEyeWorldPos(m_pGameCamera->GetPos());

        // Создаем объект Pipeline для установки параметров отображения
        Pipeline p;

        // Устанавливаем позицию камеры, цель, направление вверх
        p.SetCamera(m_pGameCamera->GetPos(), m_pGameCamera->GetTarget(), m_pGameCamera->GetUp());

        // Устанавливаем проекционную матрицу перспективы
        p.SetPerspectiveProj(m_persProjInfo);

        // Поворачиваем объект на 90 градусов по оси Y
        p.Rotate(0.0f, 90.0f, 0.0f);

        // Масштабируем объект
        p.Scale(0.005f, 0.005f, 0.005f);

        // Создаем массивы матриц преобразования для каждого экземпляра объекта
        Matrix4f WVPMatrics[NUM_INSTANCES];
        Matrix4f WorldMatrices[NUM_INSTANCES];

        // Вычисляем позицию каждого экземпляра объекта на основе его начальной позиции, скорости и масштаба
        for (unsigned int i = 0; i < NUM_INSTANCES; i++) {
            Vector3f Pos(m_positions[i]);
            Pos.y += sinf(m_scale) * m_velocity[i];
            Pos.x += cosf(m_scale) * m_velocity[i];
            p.WorldPos(Pos);
            // Получаем матрицы преобразования для каждого экземпляра объекта
            WVPMatrics[i] = p.GetWVPTrans().Transpose();
            WorldMatrices[i] = p.GetWorldTrans().Transpose();
        }

        // Отрисовываем объект на экране с использованием массивов матриц преобразования
        m_pMesh->Render(NUM_INSTANCES, WVPMatrics, WorldMatrices);

        // Отображаем количество кадров в секунду на экране
        RenderFPS();

        // Обмениваем буферы для отображения сцены на экране
        glutSwapBuffers();
    }

    // Метод для обновления сцены
    virtual void IdleCB()
    {
        // Вызываем метод для отрисовки сцены
        RenderSceneCB();
    }

    virtual void SpecialKeyboardCB(int Key, int x, int y)
    {
        m_pGameCamera->OnKeyboard(Key);
    }


    virtual void KeyboardCB(unsigned char Key, int x, int y)
    {
        switch (Key) {
        case 'q':
            glutLeaveMainLoop();
            break;
        }
    }


    virtual void PassiveMouseCB(int x, int y)
    {
        m_pGameCamera->OnMouse(x, y);
    }


    virtual void MouseCB(int Button, int State, int x, int y)
    {
    }


private:

    // Метод для вычисления количества кадров в секунду
    void CalcFPS()
    {
        // Увеличиваем счетчик кадров
        m_frameCount++;

        // Получаем текущее время
        int time = glutGet(GLUT_ELAPSED_TIME);

        // Если прошло более 1 секунды, вычисляем количество кадров в секунду
        if (time - m_time > 1000) {
            m_fps = (float)m_frameCount * 1000.0f / (time - m_time);
            m_time = time;
            m_frameCount = 0;
        }
    }

    // Метод для отображения количества кадров в секунду на экране
    void RenderFPS()
    {
        // Формируем строку с количеством кадров в секунду
        char text[32];
        SNPRINTF(text, sizeof(text), "FPS: %.2f", m_fps);

#ifdef FREETYPE
        // Отображаем текст с помощью шрифта FreeType
        m_fontRenderer.RenderText(10, 10, text);
#endif
    }

    // Метод для вычисления начальных позиций и скоростей объектов
    void CalcPositions()
    {
        for (unsigned int i = 0; i < NUM_ROWS; i++) {
            for (unsigned int j = 0; j < NUM_COLS; j++) {
                unsigned int Index = i * NUM_COLS + j;
                // Устанавливаем начальную позицию объекта
                m_positions[Index].x = (float)j;
                m_positions[Index].y = RandomFloat() * 5.0f;
                m_positions[Index].z = (float)i;
                // Устанавливаем скорость объекта
                m_velocity[Index] = RandomFloat();
                if (i & 1) {
                    m_velocity[Index] *= (-1.0f);
                }
            }
        }
    }

    // Переменные класса
    LightingTechnique* m_pEffect; // Шейдерная программа
    Camera* m_pGameCamera; // Камера
    float m_scale; // Масштаб сцены
    DirectionalLight m_directionalLight; // Направленный источник света
    Mesh* m_pMesh; // Модель объекта
    PersProjInfo m_persProjInfo; // Параметры проекции
#ifdef FREETYPE
    FontRenderer m_fontRenderer; // Рендерер шрифта
#endif
    int m_time; // Время
    int m_frameCount; // Счетчик кадров
    float m_fps; // Количество кадров в секунду
    Vector3f m_positions[NUM_INSTANCES]; // Массив позиций объектов
    float m_velocity[NUM_INSTANCES]; // Массив скоростей объектов
};


int main(int argc, char** argv)
{
    Magick::InitializeMagick(*argv);
    GLUTBackendInit(argc, argv);

    if (!GLUTBackendCreateWindow(WINDOW_WIDTH, WINDOW_HEIGHT, 32, false, "Tutorial 33")) {
        return 1;
    }

    srand;

    Tutorial33* pApp = new Tutorial33();

    if (!pApp->Init()) {
        return 1;
    }

    pApp->Run();

    delete pApp;

    return 0;
}