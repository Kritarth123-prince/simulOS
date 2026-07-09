#include <iostream>
#include <string>
#include <vector>

#include "../../../cmdunit/CMDunit.h"
#include "../../../filesystem/FileSystem.h"
#include "../../../shell/Shell.h"

/*
 *   find <name> [path]
 *
 *   Recursively searches for files/directories whose name contains <name>
 *   as a substring, starting from [path] (defaults to the current
 *   directory), and prints the full path of every match.
 *
 *   Author: Kritarth Ranjan
 * */

class FindCommand : public Command
{
  private:
    static void searchRecursive(FileSystemObject *object, const std::string &name, bool &found_any)
    {
        if (object->getName().find(name) != std::string::npos)
        {
            std::cout << object->getPath() << std::endl;
            found_any = true;
        }

        if (!object->isDir()) return;

        DirectoryObject *dir = static_cast<DirectoryObject *>(object);
        for (FileSystemObject *child : dir->childObjects)
        {
            searchRecursive(child, name, found_any);
        }
    }

  public:
    FindCommand() : Command("find", -1, "Search for files/directories by name: find <name> [path]")
    {
        CommandRegistry::registerCommand("find", this);
    }

    int operate(Shell &shell, std::vector<std::string> cmd_args) override
    {
        std::string name = cmd_args.at(0);
        std::string path = (cmd_args.size() > 1) ? cmd_args.at(1) : "";

        FileSystemObject *target = objectLocator(shell.getRootDir(), shell.getCurrentDir(), path);
        if (!target)
        {
            return -1; // Error already printed by objectLocator
        }

        bool found_any = false;

        // Do not report the search root itself unless its own name matches;
        // start matching from the root's children so `find foo /` doesn't
        // always print "/" spuriously (root has an empty name).
        if (target->isDir())
        {
            DirectoryObject *dir = static_cast<DirectoryObject *>(target);
            for (FileSystemObject *child : dir->childObjects)
            {
                searchRecursive(child, name, found_any);
            }
        }
        else
        {
            searchRecursive(target, name, found_any);
        }

        if (!found_any)
        {
            std::cout << "find: no matches for '" << name << "'." << std::endl;
        }

        return 0;
    }
};

static FindCommand findCommandInstance;
