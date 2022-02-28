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

int main(int argc, char** argv)
{
	const char* Stream = 
	"{"
		"\"Font\": \"Roboto-Regular.ttf\", \"FontSize\": 24, \"Windows\": {\"Main\": {"
			"\"Title\": \"Hello\", \"Width\": 400, \"Height\": 300, \"Body\": {\"Controls\"["
				"{\"Type\": \"HorizontalContainer\", \"Expand\": \"Both\", \"Grow\": \"Center\", \"Controls\": ["
					"{\"ID\": \"Container\", \"Type\": \"VerticalContainer\", \"Expand\": \"Height\", \"Grow\": \"Center\", \"Controls\": ["
						"{\"Type\": \"HorizontalContainer\", \"Expand\": \"Width\", \"Grow\": \"Center\", \"Controls\": ["
							"{\"ID\": \"Button\", \"Type\": \"TextButton\", \"Text\": {\"Text\": \"Hello Friends\"}}"
						"]}"
					"}]"
				"}]"
			"]}"
		"}}"
	"}";

	OctaneUI::Application Application;
	Interface::Initialize(Application);

	std::unordered_map<std::string, OctaneUI::ControlList> WindowControls;
	Application.Initialize(Stream, WindowControls);

	const OctaneUI::ControlList& List = WindowControls["Main"];
	List.To<OctaneUI::Button>("Container.Button")->SetOnPressed([&]()
	{
		static bool DidClick = false;
		if (!DidClick)
		{
			List.To<OctaneUI::Container>("Container")->AddControl<OctaneUI::Text>()->SetText("Welcome to the program :^)!");
			DidClick = true;
		}
	});

	return Application.Run();
}