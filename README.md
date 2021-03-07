# Requirements

## dependencies

For building clfParser you need some dependencies :
* conan
    * ```brew install conan``` if you are using brew
    * ```pip3 install conan``` if you are using pip
* cmake
    * ```brew install cmake``` if you are using brew
    * ```apt-get install cmake``` if you are using a debian like
    * ```yum install cmake``` if you are using redhat-like
    * try to have a recent cmake > 3.x (not tested on all cmake)
* a decent c++ compiler
    * clang that comes from XCode for MacOsX
    * g++ on Linux distros
        * ```apt-get install g++``` if you are using a debian like
        * ```yum install g++``` if you are using a redhat-like
* make
    * ```brew install make``` if you are using brew
    * ```apt-get install build-essential``` if you are using a debian like
    * ```yum install make``` if you are using a redhat-like
* docker
    * if you want to cheat and do not compile yourself this project
* A decent terminal
    * for displaying the program you need a decent terminal
        * iterm2 has been tested on MacOsX and works well
            * ```brew install iterm2```
        * Tilix has been tested on Linux and works well

## how to

the following commands needs to be done inside the project directory.

First we need to create a build directory :
```bash
mkdir -p build
cd build
```

After that we need to launch cmake :
```bash
cmake ..
```

This command can take quite some time because it will invoke **conan** the
npm like package manager for c++. So it can compile quite a lot of stuff...
When this step is finish you can build the project using make:
```bash
make -j
```

## I am not used to compile projects and want a simplier path

No worries, we have thought about that. We provide a docker image that
automagically build the entire project !

for using docker do :

```bash

```

# configuration

You can configure clfMonitor using a json parser. 
The code use nlohmann_json to parse and access to json data.

Here is a configurationExample : 

```json
{
  "batchMaxSizeNumber": 100,
  "batchMaxTimeMs": 100,
  "bufferSizeBytes": 4096,
  "consumerThreadsNumber": 4,
  "debugEnabled": true,
  "refreshTimeMs": 10000,
  "alertThresholdNumber" : 10
}
```

# Architecture

## Global architecture

```
   1 th                    1th                   x th
###########            ###########            ###########
#  File   #    ____\  #   Buffer #    ____\  #   clf   #
# Watcher #    ____    #Splitter #    ____    #  Parser #
###########        //  ###########        //  ###########
```

Here is the principal data flow into clfMonitor
* The FileWatcher thread take care of i/o :
    * looking for file changes
    * reading ios
    * tag buffer with timestamp and give them to the splitter
* The bufferSplitter thread take care of preparing lines for parser.
    * get buffers from fileWatcher
    * split buffers into single lines
    * call core **getDataFromSplitter** for each line
        * **getDataFromSplitter** will send the buffer to a thread pool for parsing data.
* clfParser parse entry and gather data
    * it is called in a thread pool of **consumerThreadsNumber** threads
    * all parsing is lockfree
    * data gathering is under lock (because ui access to these datas)

## Core
Core is like an orchestrator, it will call all the parts of this program to works well.0

## Parsing

For parsing we use a flex/bison parser. The grammar try to fit the common log format.

This parser is intended to parser only one line at a time.

## Ui

Ui code is present into *ui* code. It use imtui library (external/imtui) to display
stuff on the terminal.

### Limitations

When some part of the message are not handled by the grammar the message is dropped.
