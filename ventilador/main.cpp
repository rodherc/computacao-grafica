#include <iostream>
#include <cmath>
//biblioteca para criar a cena
#include <vart/scene.h>
//biblioteca para criar a luz
#include <vart/light.h>
//biblioteca apra criar a camera
#include <vart/camera.h>
//biblioteca para criar a view
#include <vart/contrib/viewerglutogl.h>
//biblioteca para criar os objetos
#include <vart/meshobject.h>


using namespace std;
using namespace VART;

#define ANGULO_MAXIMO ((M_PI * 20)/ 180)

class MyIHClass : public VART::ViewerGlutOGL::IdleHandler{
    private:
        Transform* rotacao_helice;
        Transform* rotacao_proprio_eixo_helice;
        Transform* rotacao_grade;
        Transform* rotacao_eixo;
        Transform* rotacao_suporte_eixo;

        double angulo_rotacao_helice = 0;
        double angulo_rotacao_grade = 0;
        bool rodar_direita = true;
    public:
        MyIHClass(Transform* rotacao_helice,Transform* rotacao_proprio_eixo_helice,Transform* rotacao_grade,
                  Transform* rotacao_eixo,Transform* rotacao_suporte_eixo){

            this->rotacao_helice = rotacao_helice;
            this->rotacao_proprio_eixo_helice = rotacao_proprio_eixo_helice;
            this->rotacao_grade = rotacao_grade;
            this->rotacao_eixo = rotacao_eixo;
            this->rotacao_suporte_eixo = rotacao_suporte_eixo;
        }

        virtual void OnIdle(){
            rotacao_proprio_eixo_helice->MakeRotation(VART::Point4D(1,1.237193,0), VART::Point4D::X(),angulo_rotacao_helice);
            angulo_rotacao_helice += 0.2;

            //criando a rotacao dos objetos
            //todos com o mesmo algulos pos todos tem qeus estar sincronizados
            rotacao_grade->MakeYRotation(angulo_rotacao_grade);
            rotacao_helice->MakeYRotation(angulo_rotacao_grade);
            rotacao_eixo->MakeYRotation(angulo_rotacao_grade);
            rotacao_suporte_eixo->MakeYRotation(angulo_rotacao_grade);
            // posso rodar pra direita
            if(rodar_direita){
                angulo_rotacao_grade += 0.002;
                if(angulo_rotacao_grade >= ANGULO_MAXIMO){ // se angulo for maior ou igual ao maximo
                    rodar_direita = false;                 // entao vamor roda para a esquerda
                }
            }else{
                angulo_rotacao_grade -= 0.002;
                if(angulo_rotacao_grade <= -(ANGULO_MAXIMO)){
                    rodar_direita = true;
                    }
                }
                viewerPtr->PostRedisplay();
            }
};
int main(int argc, char* argv[]){
    //inicializa a view
    VART::ViewerGlutOGL::Init(&argc, argv);
    //cria a cena
    static VART::Scene cena;
    //cria a view
    static VART::ViewerGlutOGL view;
    // cria a camera
    Camera camera(Point4D(-5,0,3), Point4D::ORIGIN(), Point4D::Y());

    list<MeshObject*> lista_objs;
    //pegando os objetos do arquivo e inserindo na lista
    MeshObject::ReadFromOBJ("fan.obj", &lista_objs);
    //cada objeto da lista é inserido em uma posicao do vetor
    vector<MeshObject*> objs = {begin(lista_objs), end(lista_objs)};
    //objetos que eu quero
    MeshObject* helice;
    MeshObject* base;
    MeshObject* grade;
    MeshObject* eixo;
    MeshObject* suporte_eixo;

    for(int i = 0; i< int(objs.size()); i++){
        if(objs[i]->GetDescription() == "helice"){
            helice = objs[i];
        }else if(objs[i]->GetDescription() == "base"){
            base = objs[i];
        }else if(objs[i]->GetDescription() == "grade"){
            grade = objs[i];
        }else if(objs[i]->GetDescription() == "suporte-eixo"){
            suporte_eixo = objs[i];
        }else if(objs[i]->GetDescription() == "eixo"){
            eixo = objs[i];
        }
    }
        //BASE
    cena.AddObject(base);
        //HELICE
    Transform* rotacao_helice = new Transform();
    rotacao_helice->MakeIdentity();
    Transform* rotacao_proprio_eixo_helice = new Transform();;
    rotacao_proprio_eixo_helice->MakeIdentity();
    rotacao_proprio_eixo_helice->AddChild(*helice);
    rotacao_helice->AddChild(*rotacao_proprio_eixo_helice);
    cena.AddObject(rotacao_helice);
    
        //GRADE
    Transform* rotacao_grade = new Transform();;
    rotacao_grade->MakeIdentity();
    rotacao_grade->AddChild(*grade);
    cena.AddObject(rotacao_grade);
        //EIXO
    Transform* rotacao_eixo = new Transform();;
    rotacao_eixo->MakeIdentity();
    rotacao_eixo->AddChild(*eixo);
    cena.AddObject(rotacao_eixo);
        //SUPORTE EIXO
    Transform* rotacao_suporte_eixo = new Transform();;
    rotacao_suporte_eixo->MakeIdentity();
    rotacao_suporte_eixo->AddChild(*suporte_eixo);
    cena.AddObject(rotacao_suporte_eixo);

    MyIHClass idleHandler(rotacao_helice, rotacao_proprio_eixo_helice, rotacao_grade,
                   rotacao_eixo, rotacao_suporte_eixo);
    //adiociona a luz na cena
    cena.AddLight(Light::BRIGHT_AMBIENT());
    //adiciona camera na cena
    cena.AddCamera(&camera);

    // Set up the viewer
    view.SetTitle("Ventilador");
    view.SetScene(cena); // attach the scene
    view.SetIdleHandler(&idleHandler); // attach the idle handler to the viewer

    // Run application
    cena.DrawLightsOGL(); // Set OpenGL's lights' state
    ViewerGlutOGL::MainLoop(); // Enter main loop (event loop)

    return 0;
}