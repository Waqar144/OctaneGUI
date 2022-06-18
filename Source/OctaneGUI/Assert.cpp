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

#include "Assert.h"

#include <climits>
#include <cstdarg>
#include <cstdlib>
#include <string>

namespace OctaneGUI
{

bool AssertFunc(bool Condition, const char* Format, ...)
{
#if !NDEBUG
	if (!Condition)
	{
		va_list List;
		va_start(List, Format);

		std::string Buffer;
		Buffer.resize(SHRT_MAX);
		vsnprintf(Buffer.data(), Buffer.size(), Format, List);

		va_end(List);

		printf("Assertion failed: %s:%d\n", __FILE__, __LINE__);
		printf("%s\n", Buffer.c_str());
		std::abort();
	}
#endif

	return Condition;
}

}
