#include "Engine.h"
#include "Addr.h"
#include "imgui/imgui.h"
#include "GameClass.h"
// 3C326F0

int width, height;

bool Engine::Worldtoscreen(Vector3* wtsvec, Vector3 position)
{
    if (!camera)
        camera = (Camera*)*(uintptr_t*)Addr::pCamera;
    if (!width || !height)
    {
        ImGuiIO& io = ImGui::GetIO();
        width = io.DisplaySize.x;
        height = io.DisplaySize.y;
    }
    Vector3 tvec{ camera->viewmatrix._14, camera->viewmatrix._24, camera->viewmatrix._34 };
    Vector3 rvec{ camera->viewmatrix._11, camera->viewmatrix._21, camera->viewmatrix._31 };
    Vector3 upvec{ camera->viewmatrix._12, camera->viewmatrix._22, camera->viewmatrix._32 };
    float w = tvec.Dot(position) + camera->viewmatrix._44;
    if (w < 0.098f)
        return false;
    float y = upvec.Dot(position) + camera->viewmatrix._42;
    float x = rvec.Dot(position) + camera->viewmatrix._41;
    wtsvec->x = (width / 2) * (1.f + x / w);
    wtsvec->y = (height / 2) * (1.f - y / w);
    return true;


}
