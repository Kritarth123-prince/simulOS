#ifndef CMDUNIT_H
#define CMDUNIT_H
#include <string>
#include <unordered_map>
#include <vector>

class FileSystem;
class FileSystemObject;
class DirectoryObject;
class FileObject;
class Shell;
class Command;

class CommandHandler
{
  private:
    Shell *shell{nullptr};

  public:
    CommandHandler(Shell *inp_shell);
    ~CommandHandler();

    int processCommand(std::string user_input);
    std::vector<std::string> parseCommand(const std::string &user_input);
    int executeCommand(std::vector<std::string> user_input_vec);
};

class Command
{
  protected:
    std::string key{""};
    int arg_num{};
    std::string description{""};

  public:
    Command(std::string inp_key, int inp_arg_num, std::string inp_description);
    std::string getKey() const;
    int getArgNum() const;
    std::string getDescription() const;
    virtual int operate(Shell &shell, std::vector<std::string> cmd_args) = 0;
};

class CommandRegistry
{
  private:
    static std::unordered_map<std::string, Command *> &commandMap();
    // ^-- Deliberately a function returning a reference to a function-local
    // static, NOT a plain static member. A plain static member has no
    // guaranteed initialization order relative to the static Command
    // instances (in each program's .cpp file) that call registerCommand()
    // during their own static initialization -- this caused a real crash
    // (SIGFPE inside std::unordered_map, from writing into a
    // not-yet-constructed map) depending on link order. A function-local
    // static is guaranteed to be constructed on first use, which fixes it.

  public:
    static bool registerCommand(std::string key, Command *cmd);
    static Command *getCommand(std::string key);
    static bool hasCommand(std::string key);

    static std::vector<std::string> getAllCommandNames();
};

void CommandError(const std::string &err_message);
void CommandNotFoundError(const std::string &err_message);

#endif
