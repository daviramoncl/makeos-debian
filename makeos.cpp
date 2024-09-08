#include <iostream>
#include <filesystem>
#include <string>
#include <cstdlib>
#include <thread>
#include <chrono>
#include <curl/curl.h>  // Include libcurl for HTTP/HTTPS requests
#include <fstream>      // Include fstream for file operations

using namespace std;
namespace fs = std::filesystem;

string currentDir = fs::current_path().string(); // To track the current directory

// Function to simulate terminal input prompt
void showPrompt() {
    cout << "noonie-LenovoG475:" << currentDir << "/$ ";
}

// Function to handle 'help' command to display available commands
void helpCommand() {
    cout << "Available commands:\n";
    cout << "  help         - Show this help message\n";
    cout << "  mkdir <dir>  - Create a directory\n";
    cout << "  cd <dir>     - Change directory\n";
    cout << "  ls           - List files in the current directory\n";
    cout << "  clear        - Clear the terminal screen\n";
    cout << "  echo <text>  - Print the text to the terminal\n";
    cout << "  wget <url>   - Download a file from the web\n";
    cout << "  curl <url>   - Display the HTML of a web page\n";
    cout << "  sudo <cmd>   - Execute a command with elevated privileges\n";
    cout << "  apt <cmd>    - Perform package management actions\n";
    cout << "  gcc <file>   - Compile a C program\n";
    cout << "  g++ <file>   - Compile a C++ program\n";
    cout << "  cpp <file>   - Compile a C++ file with cpp\n";
    cout << "  ./<file>     - Execute a file\n";
    cout << "  touch <file> - Create an empty file or update timestamp\n";
    cout << "  nano <file>  - Edit a file with nano editor\n";
    cout << "  exit         - Exit MakeOS\n";
}

// Function to handle 'sudo' command
void sudoCommand(const string& cmd) {
    if (cmd.empty()) {
        cout << "sudo: missing command" << endl;
    } else {
        cout << "Executing with sudo: " << cmd << endl;
        system(cmd.c_str());  // Execute the command with elevated privileges
    }
}

// Function to handle 'apt' command
void aptCommand(const string& action) {
    if (action.empty()) {
        cout << "apt: missing action (install/remove/update)" << endl;
    } else {
        cout << "Performing apt action: " << action << endl;
        system(("apt " + action).c_str());  // Perform the apt action
    }
}

// Function to simulate compiling C or C++ files using gcc, g++, cpp
void compileCommand(const string& compiler, const string& fileName) {
    if (fileName.empty()) {
        cout << compiler << ": missing input file" << endl;
    } else {
        cout << "Compiling " << fileName << " using " << compiler << endl;
        // Here, you could actually check for the file's existence and run the compilation with system().
        cout << fileName << " successfully compiled." << endl;
    }
}

// Function to handle 'curl' command and print HTML content
size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void curlCommand(const string& url) {
    CURL* curl;
    CURLcode res;
    string readBuffer;
    
    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &readBuffer);
        
        // Perform the request
        res = curl_easy_perform(curl);
        if (res != CURLE_OK) {
            cerr << "curl: Error fetching URL: " << curl_easy_strerror(res) << endl;
        } else {
            // Output the HTML content
            cout << readBuffer << endl;
        }
        
        // Clean up
        curl_easy_cleanup(curl);
    }
}

// Function to handle 'wget' command and download file with its actual name
void wgetCommand(const string& url) {
    CURL* curl;
    CURLcode res;
    FILE* file;
    string filename;
    size_t pos;

    curl = curl_easy_init();
    if (curl) {
        // Extract filename from URL
        pos = url.find_last_of('/');
        if (pos != string::npos) {
            filename = url.substr(pos + 1);
        } else {
            filename = "downloaded_file";
        }
        
        // Set the URL
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        
        // Open a file to write the downloaded content
        file = fopen(filename.c_str(), "wb");
        if (file) {
            curl_easy_setopt(curl, CURLOPT_WRITEDATA, file);
            
            // Perform the download
            res = curl_easy_perform(curl);
            if (res == CURLE_OK) {
                cout << "File downloaded as " << filename << "." << endl;
            } else {
                cerr << "wget: Error downloading file: " << curl_easy_strerror(res) << endl;
            }
            fclose(file);
        } else {
            cerr << "wget: Failed to open file for writing." << endl;
        }
        
        curl_easy_cleanup(curl);
    }
}

// Function to handle 'touch' command to create a file or update its timestamp
void touchCommand(const string& fileName) {
    if (fileName.empty()) {
        cout << "touch: missing file name" << endl;
        return;
    }
    
    if (fs::exists(fileName)) {
        // File exists, just update timestamp
        fs::last_write_time(fileName, fs::file_time_type::clock::now());
        cout << "File timestamp updated: " << fileName << endl;
    } else {
        // Create a new empty file
        ofstream ofs(fileName);
        if (ofs) {
            cout << "File created: " << fileName << endl;
        } else {
            cerr << "touch: failed to create file: " << fileName << endl;
        }
    }
}

// Function to handle 'nano' command to edit a file
void nanoCommand(const string& fileName) {
    if (fileName.empty()) {
        cout << "nano: missing file name" << endl;
        return;
    }
    
    // Simulate opening a file with nano (assuming nano is installed on the system)
    string command = "nano " + fileName;
    int result = system(command.c_str());
    if (result != 0) {
        cout << "nano: failed to open file or nano is not installed." << endl;
    }
}

int main() {
    // Welcome Screen and loading simulation
    cout << "Welcome to MakeOS" << endl;
    for (int i = 10; i <= 100; ++i) {
        cout << "Loading... " << i << "%" << endl;
        this_thread::sleep_for(chrono::milliseconds(1000));  // Simulate 1% per second
    }

    cout << "MakeOS has successfully loaded!" << endl;

    // Start terminal loop
    string command;
    while (true) {
        showPrompt();
        getline(cin, command);  // Get user input

        // Handle 'help' command to display available commands
        if (command == "help") {
            helpCommand();
        }
        // Handle 'curl' command to display HTML content
        else if (command.find("curl ") == 0) {
            string url = command.substr(5);
            curlCommand(url);
        }
        // Handle 'wget' command to download a file
        else if (command.find("wget ") == 0) {
            string url = command.substr(5);
            wgetCommand(url);
        }
        // Handle 'mkdir' command to create a directory
        else if (command.find("mkdir ") == 0) {
            string dirName = command.substr(6);
            fs::create_directory(dirName);
            cout << "Directory created: " << dirName << endl;
        }
        // Handle 'cd' command to change directory
        else if (command.find("cd ") == 0) {
            string dirName = command.substr(3);
            if (dirName == "..") {
                currentDir = fs::path(currentDir).parent_path().string();
            } else {
                fs::path newPath = fs::path(currentDir) / dirName;
                if (fs::exists(newPath) && fs::is_directory(newPath)) {
                    currentDir = newPath.string();
                    cout << "Changed directory to: " << currentDir << endl;
                } else {
                    cout << "cd: no such file or directory: " << dirName << endl;
                }
            }
        }
        // Handle 'ls' command to list files
        else if (command == "ls") {
            for (const auto& entry : fs::directory_iterator(currentDir)) {
                cout << entry.path().filename().string() << endl;
            }
        }
        // Handle 'clear' command to clear the terminal screen
        else if (command == "clear") {
            system("clear");  // Clear the terminal screen
        }
        // Handle 'echo' command to print text
        else if (command.find("echo ") == 0) {
            string text = command.substr(5);
            cout << text << endl;
        }
        // Handle file execution with './'
        else if (command.find("./") == 0) {
            string fileName = command.substr(2);  // Get file name after './'
            system(("./" + fileName).c_str());
        }
        // Handle 'touch' command to create or update a file
        else if (command.find("touch") == 0) {
            string fileName = command.substr(6);  // Get file name
            touchCommand(fileName);
        }
        // Handle 'nano' command to edit a file
        else if (command.find("nano") == 0) {
            string fileName = command.substr(5);  // Get file name
            nanoCommand(fileName);
        }
        // Handle 'sudo' command to execute with elevated privileges
        else if (command.find("sudo ") == 0) {
            string cmd = command.substr(5);  // Get command after 'sudo '
            sudoCommand(cmd);
        }
        // Handle 'apt' command for package management
        else if (command.find("apt ") == 0) {
            string action = command.substr(4);  // Get action after 'apt '
            aptCommand(action);
        }
        // Handle 'gcc', 'g++', 'cpp' commands to compile files
        else if (command.find("gcc ") == 0 || command.find("g++ ") == 0 || command.find("cpp ") == 0) {
            size_t pos = command.find(' ');
            string compiler = command.substr(0, pos);
            string fileName = command.substr(pos + 1);
            compileCommand(compiler, fileName);
        }
        // Handle 'exit' command to exit MakeOS
        else if (command == "exit") {
            cout << "Exiting MakeOS..." << endl;
            break;
        }
        // Handle unknown commands
        else {
            cout << "Unknown command: " << command << endl;
        }
    }

    return 0;
}

