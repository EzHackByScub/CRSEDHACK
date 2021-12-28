#include "Engine.h"
#include "Addr.h"
#include "imgui/imgui.h"
#include "GameClass.h"
// 3C326F0

int width, height;
Camera* cam;
bool Engine::Worldtoscreen(Vector3* wtsvec, Vector3 position)
{
    if (!cam)
        cam = (Camera*)*(uintptr_t*)Addr::pCamera;
    if (!width || !height)
    {
        ImGuiIO& io = ImGui::GetIO();
        width = io.DisplaySize.x;
        height = io.DisplaySize.y;
    }
    Vector3 tvec{ cam->viewmatrix._14, cam->viewmatrix._24, cam->viewmatrix._34 };
    Vector3 rvec{ cam->viewmatrix._11, cam->viewmatrix._21, cam->viewmatrix._31 };
    Vector3 upvec{ cam->viewmatrix._12, cam->viewmatrix._22, cam->viewmatrix._32 };
    float w = tvec.Dot(position) + cam->viewmatrix._44;
    if (w < 0.098f)
        return false;
    float y = upvec.Dot(position) + cam->viewmatrix._42;
    float x = rvec.Dot(position) + cam->viewmatrix._41;
    wtsvec->x = (width / 2) * (1.f + x / w);
    wtsvec->y = (height / 2) * (1.f - y / w);
    return true;


}
