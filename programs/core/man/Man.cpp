#include <iostream>
#include <string>
#include <vector>

#include "../../../cmdunit/CMDunit.h"
#include "../../../shell/Shell.h"
#include "../help/Help.h"

/*
 *   man <command>
 *
 *   A familiar Unix-flavored alias for `help`. Under the hood, it reuses
 *   HelpProgram (see programs/core/help), which already pulls each
 *   Command's stored description straight out of the CommandRegistry --
 *   exactly what a manual page is supposed to do.
 *
 *   Author: Kritarth Ranjan
 * */

class ManCommand : public Command
{
  public:
    ManCommand() : Command("man", -1, "Alias for 'help': show the manual entry for one or more commands")
    {
        CommandRegistry::registerCommand("man", this);
    }

    int operate(Shell &shell, std::vector<std::string> cmd_args) override
    {
        HelpProgram program;
        return program.run(cmd_args);
    }
};

static ManCommand manCommandInstance;
