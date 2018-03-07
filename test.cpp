#include <FishEngine/FishEngine.hpp>
#include <FishEngine/GameApp.hpp>

#include <iostream>


#include <pybind11/pybind11.h>
using namespace pybind11;

#include <FishEditor/FishEditorInternal.hpp>

class TestApp : public FishEngine::GameApp
{
public:
	virtual void Init() override
	{
	}
	
	virtual void Update() override
	{
	}
};


int main(int argc, char** argv)
{
    try {
		FishEditor::Init();
		TestApp app;
		app.SetWindowSize(600, 400);
		app.Run();
    } catch (error_already_set& e) {
        std::cerr << ">>> Error! Uncaught exception:\n";
		std::cerr << e.what() << std::endl;
        PyErr_PrintEx(0);
        return 1;
    }
    return 0;
}
