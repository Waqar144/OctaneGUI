/**

MIT License

Copyright (c) 2022 Mitchell Davis <mdavisprog@gmail.com>

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.

*/

#include "Interface.h"
#include "OctaneUI/OctaneUI.h"

#include <fstream>
#include <string>

int main(int argc, char **argv)
{
	std::string Buffer;
	std::ifstream File;
	File.open("Overview.json");
	if (File.is_open())
	{
		File.seekg(0, std::ios::end);
		Buffer.resize(File.tellg());
		File.seekg(0, std::ios::beg);

		File.read(&Buffer[0], Buffer.size());
		File.close();
	}

	OctaneUI::Application Application;
	SFML::Initialize(Application);
	Application.Initialize("OctaneUI");
	Application.LoadFont("Roboto-Regular.ttf");
	
	OctaneUI::ControlList List;
	Application.GetMainWindow()->Load(Buffer.c_str(), List);

	List.ToMenuItem("File.Quit")->SetOnSelected([&](OctaneUI::MenuItem* Item) -> void
	{
		Application.Quit();
	});

	List.ToButton("OKBtn")->SetOnPressed([&]() -> void
	{
		printf("OK\n");
	});

	return Application.Run();
}
