#include <iostream>
#include <string>
#include <vector>

#include "../../../cmdunit/CMDunit.h"
#include "../../../filesystem/FileSystem.h"
#include "../../../shell/Shell.h"

/*
 *   tree <path>
 *
 *   Recursively visualizes the directory structure starting at <path>,
 *   in the classic Unix "tree" branch style.
 *
 *   Author: Kritarth Ranjan
 * */

class TreeCommand : public Command
{
  private:
    static void printTree(FileSystemObject *object, const std::string &prefix, bool is_last, int &dir_count,
                            int &file_count)
    {
        std::cout << prefix << (is_last ? "\xE2\x94\x94\xE2\x94\x80\xE2\x94\x80 " : "\xE2\x94\x9C\xE2\x94\x80\xE2\x94\x80 ")
                   << object->getName() << (object->isDir() ? "/" : "") << std::endl;

        if (!object->isDir())
        {
            file_count++;
            return;
        }
        dir_count++;

        DirectoryObject *dir = static_cast<DirectoryObject *>(object);
        std::string child_prefix = prefix + (is_last ? "    " : "\xE2\x94\x82   ");

        for (size_t i = 0; i < dir->childObjects.size(); i++)
        {
            printTree(dir->childObjects.at(i), child_prefix, i == dir->childObjects.size() - 1, dir_count,
                      file_count);
        }
    }

  public:
    TreeCommand() : Command("tree", 1, "Recursively visualize a directory: tree <path>")
    {
        CommandRegistry::registerCommand("tree", this);
    }

    int operate(Shell &shell, std::vector<std::string> cmd_args) override
    {
        std::string path = cmd_args.front();

        FileSystemObject *target = objectLocator(shell.getRootDir(), shell.getCurrentDir(), path);
        if (!target)
        {
            return -1; // Error already printed by objectLocator
        }

        std::string root_label = target->getPath();
        std::cout << (root_label.empty() ? "/" : root_label) << std::endl;

        if (!target->isDir())
        {
            return 0;
        }

        DirectoryObject *dir = static_cast<DirectoryObject *>(target);
        int dir_count = 0;
        int file_count = 0;

        for (size_t i = 0; i < dir->childObjects.size(); i++)
        {
            printTree(dir->childObjects.at(i), "", i == dir->childObjects.size() - 1, dir_count, file_count);
        }

        std::cout << std::endl
                   << dir_count << " director" << (dir_count == 1 ? "y" : "ies") << ", " << file_count << " file"
                   << (file_count == 1 ? "" : "s") << std::endl;

        return 0;
    }
};

static TreeCommand treeCommandInstance;
