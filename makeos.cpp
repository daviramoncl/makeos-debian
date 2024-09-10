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
    cout << "  rmdir <dir>  - Remove an empty directory\n";
    cout << "  rm <file>    - Remove a file\n";
    cout << "  cat <file>   - Display the content of a file\n";  // Add 'cat' to help menu
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

// Function to handle 'cat' command to display the content of a file
void catCommand(const string& fileName) {
    if (fileName.empty()) {
        cout << "cat: missing file name" << endl;
        return;
    }

    fs::path filePath = fs::path(currentDir) / fileName;
    if (fs::exists(filePath) && fs::is_regular_file(filePath)) {
        ifstream file(filePath);
        string line;
        while (getline(file, line)) {
            cout << line << endl;
        }
        file.close();
    } else {
        cout << "cat: no such file: " << fileName << endl;
    }
}

// Function to handle 'rmdir' command to remove an empty directory
void rmdirCommand(const string& dirName) {
    if (dirName.empty()) {
        cout << "rmdir: missing directory name" << endl;
        return;
    }

    fs::path dirPath = fs::path(currentDir) / dirName;
    if (fs::exists(dirPath) && fs::is_directory(dirPath)) {
        if (fs::is_empty(dirPath)) {
            fs::remove(dirPath);
            cout << "Directory removed: " << dirName << endl;
        } else {
            cout << "rmdir: directory not empty: " << dirName << endl;
        }
    } else {
        cout << "rmdir: no such directory: " << dirName << endl;
    }
}

// Function to handle 'rm' command to remove a file
void rmCommand(const string& fileName) {
    if (fileName.empty()) {
        cout << "rm: missing file name" << endl;
        return;
    }

    fs::path filePath = fs::path(currentDir) / fileName;
    if (fs::exists(filePath) && fs::is_regular_file(filePath)) {
        fs::remove(filePath);
        cout << "File removed: " << fileName << endl;
    } else {
        cout << "rm: no such file: " << fileName << endl;
    }
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

// Function to handle 'wget' command to download a file from a URL
void wgetCommand(const string& url) {
    CURL* curl;
    FILE* fp;
    CURLcode res;
    string fileName = url.substr(url.find_last_of("/") + 1); // Get the file name from the URL

    curl = curl_easy_init();
    if (curl) {
        fp = fopen(fileName.c_str(), "wb");
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, NULL);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, fp);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);
        fclose(fp);

        if (res == CURLE_OK) {
            cout << "File downloaded: " << fileName << endl;
        } else {
            cout << "wget: download failed" << endl;
        }
    }
}

// Function to handle 'curl' command to display HTML of a webpage
size_t write_callback(void* contents, size_t size, size_t nmemb, void* userp) {
    ((string*)userp)->append((char*)contents, size * nmemb);
    return size * nmemb;
}

void curlCommand(const string& url) {
    CURL* curl;
    CURLcode res;
    string response;

    curl = curl_easy_init();
    if (curl) {
        curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
        curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, write_callback);
        curl_easy_setopt(curl, CURLOPT_WRITEDATA, &response);
        res = curl_easy_perform(curl);
        curl_easy_cleanup(curl);

        if (res == CURLE_OK) {
            cout << response << endl;
        } else {
            cout << "curl: failed to fetch the webpage" << endl;
        }
    }
}

// Function to handle 'mkdir' command to create a directory
void mkdirCommand(const string& dirName) {
    if (dirName.empty()) {
        cout << "mkdir: missing directory name" << endl;
        return;
    }
    fs::create_directory(dirName);
    cout << "Directory created: " << dirName << endl;
}

// Function to handle 'clear' command to clear the terminal screen
void clearCommand() {
    cout << "\033[2J\033[1;1H"; // ANSI escape code to clear the screen
}

// Function to handle 'echo' command to display text
void echoCommand(const string& text) {
    cout << text << endl;
}

// Function to handle 'ls' command to list files in the current directory
void lsCommand() {
    for (const auto& entry : fs::directory_iterator(currentDir)) {
        cout << entry.path().filename().string() << " ";
    }
    cout << endl;
}

// Function to handle 'cd' command to change directory
void cdCommand(const string& dir) {
    fs::path newDir = (dir == "..") ? fs::path(currentDir).parent_path() : fs::path(currentDir) / dir;
    if (fs::exists(newDir) && fs::is_directory(newDir)) {
        currentDir = newDir.string();
        cout << "Changed directory to: " << currentDir << endl;
    } else {
        cout << "cd: no such directory: " << dir << endl;
    }
}

// Main function
int main() {
    // Welcome Screen and loading simulation
    cout << "Welcome to MakeOS" << endl;
    for (int i = 10; i <= 100; ++i) {
        cout << "Loading... " << i << "%" << endl;
        this_thread::sleep_for(chrono::milliseconds(500));  // Simulate 1% per second
    }

    cout << "MakeOS has successfully loaded!" << endl;

    // Start terminal loop
    string command;
    while (true) {
        showPrompt();
        getline(cin, command);  // Get user input

        if (command == "help") {
            helpCommand();
        } else if (command.find("cat ") == 0) {
            string fileName = command.substr(4);
            catCommand(fileName);
        } else if (command.find("rmdir ") == 0) {
            string dirName = command.substr(6);
            rmdirCommand(dirName);
        } else if (command.find("rm ")
