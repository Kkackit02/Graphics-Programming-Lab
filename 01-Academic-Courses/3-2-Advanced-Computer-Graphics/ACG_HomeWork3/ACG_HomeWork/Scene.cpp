#include "Scene.h"
#include <vector>

std::vector<Object*>& getScene()
{
    static std::vector<Object*> Scene_instance;
    return Scene_instance;
}