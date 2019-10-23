#include <iostream>
#include <cmath>

//bibioteca para criar a cena 
#include <vart/scene.h>
//biblioteca para crair luz
#include <vart/light.h>
//biblioteca para criar a viewer
#include <vart/contrib/viewerglutogl.h>
//biblioteca para criar os objetos
#include <vart/meshobject.h>

using namespace std;
using namespace VART;

#define NUMERO_DE_CADEIRAS  10
#define ANGULO_ENTRE_CADEIRAS ((2.0 * M_PI)/ NUMERO_DE_CADEIRAS)
#define RAIO 70

class MyIHClass : public ViewerGlutOGL::IdleHandler{
    private:
        double angulo_de_rotacao_roda = 0;
    public:
        Transform* rotacao_roda;
        vector<Transform*> translacao_cadeiras;

        virtual ~MyIHClass() {}
        virtual void OnIdle(){
            //definindo rotacao da roda
            rotacao_roda->MakeRotation(Point4D::X(), angulo_de_rotacao_roda); // (eixo da rotacao, angulo)
           //angulo para auxiliar no calculos da translacao
            double angulo_de_tranlacao_cadeira = 0;
            //a roda vai rodar no eixo x, as alteraçoes sao feitas somente no y e z 
            double y = 0, z = 0;

            for(int i = 0; i < NUMERO_DE_CADEIRAS; i++){
                //coordenadas polares
                y = RAIO * sin(angulo_de_tranlacao_cadeira + angulo_de_rotacao_roda);
                z = RAIO * -cos(angulo_de_tranlacao_cadeira + angulo_de_rotacao_roda);
                // translacao para as novas coordenadas
                translacao_cadeiras[i]->MakeTranslation(0,y,z);
                // incrementa o angulo para o calculo da proxima cadeira
                angulo_de_tranlacao_cadeira += ANGULO_ENTRE_CADEIRAS;
            }
            // variando o angulo de rotacao
            angulo_de_rotacao_roda+= 0.01;
            viewerPtr->PostRedisplay();
        }
};
int main(int argc, char* argv[]){
    // inicializando GLUT
    ViewerGlutOGL::Init(&argc, argv);
    //criando a cena
    static Scene cena;
    //criado a view
    static ViewerGlutOGL view;
    // criando a camera
    Camera camera(Point4D(200,60,0),Point4D::ORIGIN(),Point4D::Y());
    camera.SetFarPlaneDistance(300);

    // leitura dos objetos do arquivo.obj
    list<MeshObject*> lista_obj;
    //cria uma lista com os objetos do arquivo
    MeshObject::ReadFromOBJ("ferris-wheel.obj", &lista_obj);
    // cria um vetor para cada objeto da lista
    vector<MeshObject*> obj = {begin(lista_obj), end(lista_obj)};

    MeshObject* cadeira;
    MeshObject* suporte;
    MeshObject* roda;

    for(int i = 0; i < int(obj.size()); i++){
        if(obj[i]->GetDescription() == "chair"){
            cadeira = obj[i];//encontra o obj cadeira
            } 
        else if(obj[i]->GetDescription() == "support"){
            suporte = obj[i];//encontra o obj suporte
        } 
        else if(obj[i]->GetDescription() == "wheel") {
            roda = obj[i]; // encontra o obj roda
        }
    }

            //CADEIRAS
    // vetor para transformar todas as cadeiras
    vector<Transform*> trans_cadeiras;
    for (int i = 0; i < NUMERO_DE_CADEIRAS; i++)
    {
        // para cada cadeira faça
        Transform* t_cadeira = new Transform();
        t_cadeira->MakeIdentity();
        // adiciona a cadeira como filha da tranformacao
        t_cadeira->AddChild(*cadeira);
        // adiona a tranfomacao de cada cadeira no vetor de transformaçoes
        trans_cadeiras.push_back(t_cadeira);
        //aciona a cadeira na cena
        cena.AddObject(trans_cadeiras[i]);
    }
    
            //RODA
    // cria uma tranformaca para a roda
    Transform* trans_roda = new Transform();
    trans_roda->MakeIdentity();
    // adiciona a roda como filha da transformacao
    trans_roda->AddChild(*roda);
    // adiociona a roda na cena
    cena.AddObject(trans_roda);

            //SUPORTE
    //suporte nao tem nenhuma alteracao.
    cena.AddObject(suporte);

    // adiciona luz e a camera na cena
    cena.AddLight(Light::BRIGHT_AMBIENT());
    cena.AddCamera(&camera);

    // criacao e atribuicao dos valores do idle
    MyIHClass idle;
    idle.rotacao_roda = trans_roda;
    idle.translacao_cadeiras = trans_cadeiras;

    view.SetTitle("Roda Gigante");
    view.SetScene(cena); // attach the scene
    // setta o objeto idle como idlehandler
    view.SetIdleHandler(&idle);
    
    cena.DrawLightsOGL();
    ViewerGlutOGL::MainLoop();
    return 0;
}