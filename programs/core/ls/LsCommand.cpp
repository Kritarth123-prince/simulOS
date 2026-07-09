#include "LsCommand.h"

#include <iostream>
#include <string>
#include <vector>

#include "../../../cmdunit/CMDunit.h"
#include "../../../filesystem/FileSystem.h"
#include "../../../shell/Shell.h"

LsCommand::LsCommand() : Command("ls", -1, "List directory contents. Flags: -l/-la for detailed view")
{
    CommandRegistry::registerCommand("ls", this);
};

int LsCommand::operate(Shell &shell, std::vector<std::string> cmd_args)
{
    LsApp *app = new LsApp();
    int code = app->run(shell, cmd_args);
    delete app;
    app = nullptr;
    return code;
}

int LsApp::run(const Shell &shell, std::vector<std::string> cmd_args)
{
    // Get current directory from shell (assuming shell has this capability)
    DirectoryObject *current_dir = shell.getCurrentDir();
    DirectoryObject *root_dir = shell.getRootDir();

    // Separate flags ("-l", "-la", "-a", etc.) from the actual path argument.
    // Any single argument beginning with '-' is treated as a flag; whatever
    // is left (there should be at most one remaining token) is the path.
    bool detailed = false;
    std::string path = "";
    bool path_set = false;

    for (const std::string &arg : cmd_args)
    {
        if (!arg.empty() && arg.at(0) == '-')
        {
            if (arg.find('l') != std::string::npos) detailed = true;
            continue;
        }
        path = arg;
        path_set = true;
    }

    if (!path_set) path = ""; // defaults to current directory

    // Find the target directory
    FileSystemObject *target = objectLocator(root_dir, current_dir, path);

    if (!target)
    {
        return -1; // Error already printed by objectLocator
    }

    if (!target->isDir())
    {
        FileSystemError("Cannot list contents of a file");
        return -1;
    }

    DirectoryObject *target_dir = static_cast<DirectoryObject *>(target);
    return detailed ? target_dir->displayDetailedContent() : target_dir->displayContent();
}

static LsCommand lsCommandInstance;
