
## Thing, meet thing

ceformat is a C++11 header library crafted by a psychopath to serve
some kind of demented justice.

It just so happens that this library implements C++ iostream string
formatting with a constexpr format string parser. How this works at
all is anyone's guess.

## Building

All features support Clang 3.3+ with libc++ (SVN head). GCC (as of 4.7.3) is
not supported due to its lack of support for the use of values belonging to an
object being constructed in a constant expression (and probably for many many
other reasons).

Tests can be compiled using plash's standard project protocol:
http://komiga.com/pp-cpp

## License

ceformat carries the MIT license, which can be found both below
and in the `LICENSE` file.

```
Copyright (c) 2013-2014 Tim Howard

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
```
