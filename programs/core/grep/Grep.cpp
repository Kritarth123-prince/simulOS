#include <iostream>
#include <string>
#include <vector>

#include "../../../cmdunit/CMDunit.h"
#include "../../../filesystem/FileSystem.h"
#include "../../../shell/Shell.h"

/*
 *   grep <pattern> [path]
 *
 *   Searches file contents for a substring match, printing "path:line: text"
 *   for every matching line. If [path] points to a directory (or is
 *   omitted, defaulting to the current directory), the search recurses
 *   through every file underneath it.
 *
 *   Author: Kritarth Ranjan
 * */

class GrepCommand : public Command
{
  private:
    static void searchFile(FileObject *file, const std::string &pattern, bool &found_any)
    {
        std::string content = file->read();
        std::string line = "";
        int line_no = 1;

        auto checkLine = [&](const std::string &l, int ln) {
            if (l.find(pattern) != std::string::npos)
            {
                std::cout << file->getPath() << ":" << ln << ": " << l << std::endl;
                found_any = true;
            }
        };

        for (char c : content)
        {
            if (c == '\n')
            {
                checkLine(line, line_no);
                line.clear();
                line_no++;
            }
            else
            {
                line += c;
            }
        }
        // last line (or the whole content, if there was no newline at all)
        checkLine(line, line_no);
    }

    static void searchRecursive(FileSystemObject *object, const std::string &pattern, bool &found_any)
    {
        if (!object->isDir())
        {
            searchFile(static_cast<FileObject *>(object), pattern, found_any);
            return;
        }

        DirectoryObject *dir = static_cast<DirectoryObject *>(object);
        for (FileSystemObject *child : dir->childObjects)
        {
            searchRecursive(child, pattern, found_any);
        }
    }

  public:
    GrepCommand() : Command("grep", -1, "Search file contents: grep <pattern> [path]")
    {
        CommandRegistry::registerCommand("grep", this);
    }

    int operate(Shell &shell, std::vector<std::string> cmd_args) override
    {
        std::string pattern = cmd_args.at(0);
        std::string path = (cmd_args.size() > 1) ? cmd_args.at(1) : "";

        FileSystemObject *target = objectLocator(shell.getRootDir(), shell.getCurrentDir(), path);
        if (!target)
        {
            return -1; // Error already printed by objectLocator
        }

        bool found_any = false;
        searchRecursive(target, pattern, found_any);

        if (!found_any)
        {
            std::cout << "grep: no matches for '" << pattern << "'." << std::endl;
        }

        return 0;
    }
};

static GrepCommand grepCommandInstance;
