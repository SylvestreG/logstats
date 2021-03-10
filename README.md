# Requirements

## dependencies

### MacOsX

You need this dependencies for building on MacOsX :

* conan
    * ```brew install conan``` if you are using brew
* cmake
    * ```brew install cmake``` if you are using brew
* a decent C compiler. Clang must comes with XCode, if you do not have XCode install it from the App Store.
* make
    * ```brew instal make```
* a good terminal like iterm2
    * ```brew instal --cask iterm2```

### Linux

For the moment I only try on ArchLinux. Here is an list of dependencies needed.

* conan, install pip3 and install it from pip after that
    * ```pip3 install conan```
* cmake
    * ```pacman -S cmake``` on ArchLinux
    * ```apt-get install cmake``` on debian-like
* make / g++
    * ```pacman -S make g++``` on ArchLinux
    * ```apt-get install build-essential g++``` on debian-like
* a good terminal like Tilix
    * ```pacman -S tilix``` on ArchLinux
    * ```apt-get install tilix``` on debian-like
* bison/flex/m4 can be necessary on debian to build on debian
    * ```apt-get install bison flex m4``` on debian-like

## how to build

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

This command can take quite some time because it will invoke **conan** the npm like package manager for c++. So it can
compile quite a lot of stuff... When this step is finish you can build the project using make:

```bash
make -j
```

### build errors on debian ###

On some linux you can have some build errors on m4/flex/bison. for that remove your build directory and restart the
build process, except replace :

```bash
cmake ..
```

by

```bash
cmake -DUSE_SYSTEM_BISON=ON ..
```

## What are the different binaries

all the binary are generated by the build phase. you will find them in the 
build/bin directory. 4 binaries should be
generated :

### clfMonitor

The clfMonitor. This program parse a clf log file and output interesting 
information about it.

### clfGenerator

The clfGenerator generator will generate in an input file *N* log lines 
every *T* interval of times. The behaviour of  this program is defined 
by its config file.

### scenarioPlayer

The scenarioPlayer will take an input file and will give each line to an
output file following a scenario define in its config files

### units

This is for developpement purpose. It some unit test of some part of the code.

### conf Files

You can find examples of config files in the conf folder in the project root dir.

# Architecture

## Timers

We use two boost::asio timers in core :
* _alertOffTimer
* _refreshTimer

```alertOffTimer``` is a tick that will comes every 50ms. Its aim is too look
to an end of an high traffic event.

```refreshTimer``` is use to refresh the current window of the monitoring 
datas.

## Data
std::map<uint64_t, boost::beast::http::verb> _orderedVerb;
std::map<uint64_t, boost::beast::http::status> _orderedStatus;
std::map<uint64_t, std::string> _orderedPath;

we split the data into two structure that has quite the same fields : 
- a monitoring window structure named ***MonitoringData***
- a monitoring global structure named ***GlobalMonitoringData***

the first one will be updated at each log line inside the current window and will be reset at the end
of the window.
the second one will be update on each log line.

Here is the data inside both structure.
```c++
  uint64_t _totalLines{0};          // nb parsed line
  uint64_t _totalValidLines{0};     // nb parser line with success
  uint64_t _totalSize{0};           // sum of each line http size

  std::unordered_map<boost::beast::http::verb, uint64_t> _verbMap;     //hashtable with verb as key, nb occurance as value
  std::unordered_map<boost::beast::http::status, uint64_t> _statusMap; //hashtable with status as key, nb occurance as value
  std::unordered_map<std::string, uint64_t> _pathMap;                  //hashtable with path as key, nb occurance as value
  std::array<uint64_t, 3> _versionArray;                               //array with nb with nb occurance of http version (0 => 1.0, 1 => 1.1 => unknow)
```

the current window also have theses map that will be filled at the end of the current window
```c++
  std::map<uint64_t, boost::beast::http::verb> _orderedVerb;
  std::map<uint64_t, boost::beast::http::status> _orderedStatus;
  std::map<uint64_t, std::string> _orderedPath;
```
The map is ordered by its key. It will allow us to print the verb/status/path in the order of the number of times we see it. 

## Global Thread architecture

```
   1 th                    1th                   x th
###########            ###########            ###########
#  File   #    ____\  #   Buffer #    ____\   #   clf   #
# Watcher #    ____    #Splitter #    ____    #  Parser #
###########        //  ###########        //  ###########
                                                  | |
                                                  | | set Data
                                                \| |//
                                               #########
                                               #       #
                                               #   Ui  #
                                               #       # 
                                               #########
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

Ui code is present into *ui* code. It use imtui library (external/imtui) to display stuff on the terminal.

### Limitations

When some part of the message are not handled by the grammar the message is dropped.

## Improvements

### Move threads to service.
if we want to be elastic we could move each thread (except the io one) to a *micro service* using 
a communication bus like ```zeromq```. It will allow us to give the job to local/remote workers.

### A tickless scheduler.
for the moment we use a tick(every 50ms) for looking to the end of alert period.
we could remove this tick and set a timer to then end of the alert period. (updating it if a new lines come before the trig)

### An api to get stats
The data could easily provide an api to fetching data. Something like grpc or 
an api rest. Doing that will allow us to remove the ui from clfMonitor.