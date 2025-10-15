#include "Scene.h"
#include <vector>

std::vector<Geometry*>& getScene()
{
    static std::vector<Geometry*> g_Scene_instance;
    return g_Scene_instance;
}