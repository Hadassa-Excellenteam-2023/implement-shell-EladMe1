#pragma once

#pragma once

#include <string>
#include <vector>
#include <cstdio> 
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <fcntl.h>
#include <sys/stat.h>


// Struct to hold information about a background process
struct BackgroundProcess {
    pid_t pid;
    std::string command;
};

class Shell {
public:
    void run();

private:
    void executeCommand(const std::string& command, bool runInBackground);
    std::vector<std::string> splitString(const std::string& input, char delimiter);
    void showBackgroundProcesses() const;
    void removeFinishedProcesses();
    void processRedirection(std::vector<std::string>& args, std::string& inputFile, std::string& outputFile);
    size_t findRedirection(const std::vector<std::string>& args, const std::string& redirectionOperator) const ;
    void handleInputRedirection(const std::string& inputFile);
    void handleOutputRedirection(const std::string& outputFile);
    void executeSingleCommand(const std::string& command);

    std::vector<BackgroundProcess> backgroundProcesses;
};


