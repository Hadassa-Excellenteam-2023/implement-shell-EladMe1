#pragma once

#include <string>
#include <vector>

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

    std::vector<BackgroundProcess> backgroundProcesses;
};



