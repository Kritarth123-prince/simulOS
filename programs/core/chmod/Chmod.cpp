#include <iostream>
#include <string>
#include <vector>

#include "../../../cmdunit/CMDunit.h"
#include "../../../filesystem/FileSystem.h"
#include "../../../shell/Shell.h"

/*
 *   chmod <mode> <path>
 *
 *   Sets the permission string of a FileSystemObject. simulOS has a single
 *   simulated user, so there is no owner/group/other split like real Unix
 *   chmod -- instead <mode> is a 3-character string, one flag per
 *   capability: read, write, execute (e.g. "rwx", "r--", "rw-"). Directories
 *   use 'x' to mean "may be entered with cd"; files currently ignore 'x'.
 *
 *   Author: Kritarth Ranjan
 * */

class ChmodCommand : public Command
{
  public:
    ChmodCommand() : Command("chmod", 2, "Change permissions of a file/directory: chmod <rwx-mode> <path>")
    {
        CommandRegistry::registerCommand("chmod", this);
    }

    int operate(Shell &shell, std::vector<std::string> cmd_args) override
    {
        std::string mode = cmd_args.at(0);
        std::string path = cmd_args.at(1);

        FileSystemObject *target = objectLocator(shell.getRootDir(), shell.getCurrentDir(), path);
        if (!target)
        {
            return -1; // Error already printed by objectLocator
        }

        int code = target->setPermissions(mode);
        if (code == 0)
        {
            std::cout << "chmod: '" << target->getName() << "' permissions set to '" << mode << "'." << std::endl;
        }
        return code;
    }
};

static ChmodCommand chmodCommandInstance;
