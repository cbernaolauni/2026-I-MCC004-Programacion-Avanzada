#include <vtkActor.h>
#include <vtkCamera.h>
#include <vtkCubeSource.h>
#include <vtkNamedColors.h>
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkInteractorStyleTrackballCamera.h>
#include <vtkRenderer.h>
#include <vtkNew.h>
#include <vtkAutoInit.h>

VTK_MODULE_INIT(vtkRenderingOpenGL2)
VTK_MODULE_INIT(vtkInteractionStyle)

int main() {
    vtkNew<vtkNamedColors> colors;
    vtkNew<vtkCubeSource> cube;
    cube->SetXLength(2.0);
    cube->SetYLength(2.0);
    cube->SetZLength(2.0);

    vtkNew<vtkPolyDataMapper> mapper;
    mapper->SetInputConnection(cube->GetOutputPort());

    vtkNew<vtkActor> actor;
    actor->SetMapper(mapper);
    actor->GetProperty()->SetColor(colors->GetColor4d("Tomato").GetData());
    actor->GetProperty()->EdgeVisibilityOn();
    actor->RotateX(30.0);
    actor->RotateY(-45.0);

    vtkNew<vtkRenderer> renderer;
    renderer->AddActor(actor);
    renderer->SetBackground(colors->GetColor3d("SteelBlue").GetData());

    vtkNew<vtkRenderWindow> window;
    window->SetWindowName("Demo VTK - Cubo 3D");
    window->SetSize(800, 600);
    window->AddRenderer(renderer);

    vtkNew<vtkRenderWindowInteractor> interactor;
    interactor->SetRenderWindow(window);

    vtkNew<vtkInteractorStyleTrackballCamera> style;
    interactor->SetInteractorStyle(style);

    interactor->Initialize();
    window->Render();
    interactor->Start();

    return 0;
}