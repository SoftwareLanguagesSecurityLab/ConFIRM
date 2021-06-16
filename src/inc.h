#pragma once

#ifdef _WIN32
#ifdef EXPORTING_DLL
extern "C" __declspec(dllexport) void increment(int &i);
extern "C" __declspec(dllexport) int exp_null;
#else
extern "C" __declspec(dllimport) void increment(int &i);
extern "C" __declspec(dllimport) int exp_null;
#endif
#elif __linux__
extern "C" void increment(int &i);
extern "C" int exp_null;
#endif
