#include <iostream>
//biblioteca pra criar a cena
#include<vart/scene.h>
//bilbioteca pra criar a luz
#include<vart/light.h>
//biblioteca pra crar a view
#include <vart/contrib/viewerglutogl.h>
//biblioteca pra criar o objeto
#include<vart/meshobject.h>

using namespace std;

class Cubo : public VART::MeshObject {
    public: Cubo();
};

Cubo::Cubo(){
    // vetor de coordenadas dos vertices do cubo
                    //x,y,z
    double coor [] = {0,0,0, //vertices de 1 a 8
                      1,0,0,
                      0,0,1,
                      1,0,1,
                      0,1,0,
                      1,1,0,
                      0,1,1,
                      1,1,1
                  };

    //ponteiro para o final do array
    double* fimArrayCoor = coor + sizeof(coor)/sizeof(double);

    // insere as coordenadas no mesh
    vertCoordVec.assign(coor,fimArrayCoor);

    //definir as faces visiveis(regra da mao direita)
    uint faces [] = {0,1,3,2,
                     5,4,6,7,
                     1,5,7,3,
                     4,0,2,6,
                     3,7,6,2, /// primeiro 3
                     4,5,1,0
                };
    uint* fimArrayFaces = faces + sizeof(faces)/sizeof(uint);

    // cria os quadrados
    VART::Mesh cubo;
    cubo.type = VART::Mesh::QUADS;
    // insere as faces no mesh
    cubo.indexVec.assign(faces,fimArrayFaces);
    // define o material do cubo
    cubo.material = VART::Material::PLASTIC_BLUE();
    //adiociona o cubo
    meshList.push_back(cubo);

    ComputeVertexNormals();
    ComputeBoundingBox();
    ComputeRecursiveBoundingBox();
}

int main(int argv, char** argc){
    // cria a cena, viewer e a camera
    VART::ViewerGlutOGL::Init(&argv,argc);
    static VART::Scene scene;
    static VART::ViewerGlutOGL viewer;
    VART::Camera camera(VART::Point4D(2,2,3), VART::Point4D::ORIGIN(), VART::Point4D::Y());

    Cubo c;
    // adicnona o cubo na cena
    scene.AddObject(&c);
    // adicnona a luz na cena
    scene.AddLight(VART::Light::BRIGHT_AMBIENT());
    // adiciona a camera na cena
    scene.AddCamera(&camera);

    viewer.SetTitle("CUBO");
    viewer.SetScene(scene);

    scene.DrawLightsOGL();
    VART::ViewerGlutOGL::MainLoop();

    return 0;
}