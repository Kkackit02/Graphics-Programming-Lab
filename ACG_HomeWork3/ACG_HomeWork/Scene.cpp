#include "Scene.h"
#include <vector>

std::vector<Object*>& getScene()
{
    static std::vector<Object*> g_Scene_instance;
    return g_Scene_instance;
}