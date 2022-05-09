
#include <iostream>
#include "FileWatcher.h"
#include <filesystem>
#include <string>
#include <algorithm>
#include <ctype.h>
#include <cstring>
#include <vector>
#include <fstream>
#include <regex>
#include <chrono>
#include <thread>

namespace fs = std::filesystem;

using std::cout; using std::cin;
using std::endl; using std::string;
using std::filesystem::current_path;

std::vector<std::string> all_args;

bool hasEnding (std::string const &fullString, std::string const &ending) {
    if (fullString.length() >= ending.length()) {
        return (0 == fullString.compare (fullString.length() - ending.length(), ending.length(), ending));
    } else {
        return false;
    }
}

#if UNITEST_ENABLED == 1
int Collector_Test(int argc, char* argv[])
{
    cout << "Passed Arguments: " << argv[0] << '\n';
    cout << "Passed Arguments: " << argv[1] << '\n';
    cout << "Passed Arguments: " << argv[2] << '\n';
    cout << "Passed Arguments: " << argv[3] << '\n';

#else
int main(int argc, char* argv[])
{
#endif
    // Create a FileWatcher instance that will check the current folder for changes every 5 seconds


	if (argc > 1)
	{
	    all_args.assign(argv + 1, argv + argc);
	}

    cout << "Current working directory: " << current_path() << '\n';

	FileWatcher fw{argv[1], std::chrono::milliseconds(5000)};

    // Start monitoring a folder for changes and (in case of changes)
    // run a user provided lambda function
    fw.start([] (std::string path_to_watch, FileStatus status) -> void {
        // Process only regular files, all other file types are ignored
        if(!std::filesystem::is_regular_file(std::filesystem::path(path_to_watch)) && status != FileStatus::erased)
        {
            return;
        }

        if(status == FileStatus::created)
        {
        	// get file name from the string:
        	std::cout << fs::path(path_to_watch).stem() << '\n';
        	std::string DetectedFile = fs::path(path_to_watch).stem();

        	// Check for file name:
        	if(DetectedFile.rfind("core",0) != 0)
        	{
        		return;
        	}

        	if(hasEnding(path_to_watch, "lz4") == false)
        	{
        		return;
        	}


        	// find the last occurrence of '.'

        	size_t dotes = std::count(path_to_watch.begin(), path_to_watch.end(), '.');
        	if(dotes <= 1)
        	{
        		return;
        	}

        	// checking name of file contains hex values
        	bool HexFound = false;
        	std::istringstream iss(path_to_watch);
        	std::vector<std::string> tokens;
        	std::string token;
        	while (std::getline(iss, token, '.')) {
        		if (!token.empty())
        			tokens.push_back(token);
        	}
        	for(size_t i=0; i<tokens.size();i++)
        	{
        		if(std::all_of(tokens[i].begin(), tokens[i].end(), ::isxdigit))
        		{
        			HexFound = true;
        			break;
        		}
        	}

        	if(false == HexFound)
        	{
        		return;
        	}

        	// File created and has the trigger criteria:
        	std::cout << "File created: " << path_to_watch << '\n';

            // command to be executed
            std::string cmd("du -ha --time ");
            cmd.append(all_args[1]);
            std::ofstream fout("CollectedData.txt");
            FILE *stream = popen(cmd.c_str(), "r");
            string line;
            const int max_size = 256;
            char readbuf[max_size];
            if (stream)
            {

                while (fgets(readbuf, max_size, stream) != NULL)
                {
                	// print content to text file
                	fout <<  readbuf;
                }
                pclose(stream);
                // close the opened file.
                fout.close();

                // move the created file to Target folder
                cmd = "mv CollectedData.txt ";
                cmd.append(all_args[1]);
                popen(cmd.c_str(), "r");
            }

            // create Archive:
            cmd = "tar -czvf ";
            // todo to make the name of file unique based on current time
            cmd.append("TargetArch.tar.gz ");
            cmd.append(all_args[1]);
            popen(cmd.c_str(), "r");


            // Get current date Time:
            string DateTime;
            cmd = "date +";
            cmd.append("%d-%m-%y-%T");
            stream = popen(cmd.c_str(), "r");
            if(fgets(readbuf, max_size, stream) != NULL)
            {
            	DateTime.append(readbuf);
            	DateTime =std::regex_replace( DateTime, std::regex("\\r\\n|\\r|\\n"),"");
            }
            else
            {
            	DateTime = "01-01-22-00:00:00";
            }

            // Wait for 1 sec to make sure archieve is created
            std::this_thread::sleep_for(std::chrono::milliseconds(1000));

            //move and rename file to the target directory
            cmd = "mv TargetArch.tar.gz ";
            cmd.append(all_args[2]);
            cmd.append("/TargetArch_");
            cmd.append(DateTime);
            cmd.append(".tar.gz");
            
            popen(cmd.c_str(), "r");

#if UNITEST_ENABLED == 1
            cout << "Set global to true!!" <<endl;
            G_FunctionCalled = true;
#endif

        }
    });
}
