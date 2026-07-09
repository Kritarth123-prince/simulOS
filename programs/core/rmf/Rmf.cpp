#include <iostream>
#include <string>
#include <vector>

#include "../../../cmdunit/CMDunit.h"
#include "../../../filesystem/FileSystem.h"
#include "../../../shell/Shell.h"

/*
 *  Author: Kritarth Ranjan
 *  Reviewer: Kritarth Ranjan
 *  # Approved!
 *
 * */

class RmfCommand : public Command
{
  public:
    RmfCommand() : Command("rmf", 1, "Removes a directory")
    {
        CommandRegistry::registerCommand("rmf", this);
    }

    int operate(Shell &shell, std::vector<std::string> cmd_args) override
    {
        std::string dir_name = cmd_args.front();
        DirectoryObject *current_dir = shell.getCurrentDir();

        FileSystemObject *dir_to_remove = current_dir->getChild(dir_name);
        if (!dir_to_remove || !dir_to_remove->isDir())
        {
            std::cout << "Error: Directory not found." << std::endl;
            return -1;
        }

        int code = dir_to_remove->remove();
        if (code != 0)
        {
            return code; // error already printed by remove()/hasPermission checks
        }
        std::cout << "Directory '" << dir_name << "' removed." << std::endl;
        return 0;
    }
};

static RmfCommand rmfCommandInstance;
