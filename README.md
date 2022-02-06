# Crash it!

## But why?
First of all, crashing applications is fun!

Crash it! is a small windows application which can be used to force crash another 
application. This might be useful in scenarios where you want to test whether your 
application is capable of catching crashes or if an external program is capable of 
diagnosing the nature of the crash.

## Building
Crash it! requires [CMake](https://cmake.org/) to build.
After you installed CMake, run the following:
```powershell
mkdir build
cd build
cmake ..
cmake --build . --config Release
start Release # Open directory Release with crash-it.exe, optional
```

Happy crashing!

## Further notes
Crash it! has been built to observe the exit codes of processes under certain 
termination conditions. Instead of building an executable which crashes itself, 
this tool may be used to test other applications.

Feel free to add more ways of crashing and creating a PR.
