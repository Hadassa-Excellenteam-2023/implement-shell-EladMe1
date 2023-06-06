#include "shell.h"
#include <iostream>
#include <sstream>
#include <unistd.h>
#include <sys/types.h>
#include <sys/wait.h>


/**
 * Runs the shell program, allowing the user to input commands and execute them.
 * The shell supports running commands both in the foreground and background.
 */
void Shell::run() {
    std::string input;

    while (true) {
        removeFinishedProcesses();

        std::cout << "Shell> ";
        std::getline(std::cin, input);

        if (input == "exit") {
            break;
        }

        if (input == "myjobs") {
            showBackgroundProcesses();
            continue;
        }

        // Check if the command should be run in the background
        bool runInBackground = false;
        if (!input.empty() && input.back() == '&') {
            runInBackground = true;
            input.pop_back();
        }

        executeCommand(input, runInBackground);
    }
}

/**
 * Executes the given command in a child process.
 * If the command should be run in the background, it adds the background process
 * to the list of background processes and prints a message.
 * If the command should be run in the foreground, it waits for the child process to finish.
 * @param command The command to execute
 * @param runInBackground Flag indicating whether the command should be run in the background
 */
void Shell::executeCommand(const std::string& command, bool runInBackground) {
    pid_t pid = fork();
    if (pid == -1) {
        std::cerr << "Failed to create child process" << std::endl;
        return;
    } 
    
    else if (pid == 0) {
        // Child process
        std::vector<std::string> args = splitString(command, ' ');

        // Convert vector of strings to char* array
        std::vector<char*> cArgs;
        for (const auto& arg : args) {
            cArgs.push_back(const_cast<char*>(arg.c_str()));
        }
        cArgs.push_back(nullptr); // Add a null terminator at the end

        execvp(cArgs[0], cArgs.data());

        // execvp will only return if an error occurred
        std::cerr << "Failed to execute command: " << command << std::endl;
        exit(EXIT_FAILURE);
    } else {
        // Parent process
        if (runInBackground) {
            BackgroundProcess process;
            process.pid = pid;
            process.command = command;
            backgroundProcesses.push_back(process);
            std::cout << "Background process started: " << command << std::endl;
        } else {
            int status;
            waitpid(pid, &status, 0);
            if (status != 0) {
                std::cerr << "Command exited with non-zero status: " << command << std::endl;
            }
        }
    }
}

/**
 * Splits the given input string into a vector of strings based on the specified delimiter.
 * @param input The input string to split
 * @param delimiter The delimiter character
 * @return A vector of strings resulting from the split operation
 */
std::vector<std::string> Shell::splitString(const std::string& input, char delimiter) {
    std::vector<std::string> tokens;
    std::string token;
    std::istringstream tokenStream(input);
    while (std::getline(tokenStream, token, delimiter)) {
        tokens.push_back(token);
    }
    return tokens;
}

/**
 * Prints the list of background processes along with their PIDs and commands.
 */
void Shell::showBackgroundProcesses() const {
    std::cout << "Background processes:" << std::endl;
    for (const auto& process : backgroundProcesses) {
        std::cout << "PID: " << process.pid << ", Command: " << process.command << std::endl;
    }
}

/**
 * Removes the background processes that have finished executing from the list.
 */
void Shell::removeFinishedProcesses() {
    std::vector<BackgroundProcess>::iterator it = backgroundProcesses.begin();
    while (it != backgroundProcesses.end()) {
        pid_t result = waitpid(it->pid, nullptr, WNOHANG);
        if (result == -1) {
            // Error occurred while checking process status
            std::cerr << "Failed to check background process status: PID " << it->pid << std::endl;
            ++it;
        } else if (result == 0) {
            // Process is still running
            ++it;
        } else {
            // Process has finished
            it = backgroundProcesses.erase(it);
        }
    }
}


