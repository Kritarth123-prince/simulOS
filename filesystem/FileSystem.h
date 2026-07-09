#ifndef FILESYSTEM_H
#define FILESYSTEM_H

#include <string>
#include <vector>

class FileSystemObject;
class DirectoryObject;
class FileObject;

class FileSystem
{
  private:
    DirectoryObject *root_dir{nullptr};

  public:
    FileSystem();
    ~FileSystem();

    DirectoryObject *getRootDir();
};

std::string pathBuilder(FileSystemObject *fsystemObject);
FileSystemObject *objectLocator(DirectoryObject *root_dir, DirectoryObject *current_dir, std::string path);
std::string getCurrentTime();
void FileSystemError(const std::string &err_message);

bool permissionStringValidator(const std::string &perms);

class FileSystemObject
{
  private:
    std::string name{""};
    DirectoryObject *parent{nullptr};
    std::string created_at{""};
    std::string updated_at{""};
    bool is_dir;
    std::string permissions{"rwx"};
    // ^-- simplified single-owner permission model: 3 chars, each either
    // the letter (r/w/x) or '-'. No user/group/other distinction, since
    // simulOS only ever has a single simulated user.

  protected:
    // ^-- as the constructor should be accessible in derived classes
    FileSystemObject(std::string inp_name, DirectoryObject *inp_parent, bool inp_is_dir);
  public:
    virtual ~FileSystemObject();
    int remove();

    std::string getName() const;
    DirectoryObject *getParent() const;

    std::string getPath();
    std::string getCreatedTime() const;
    std::string getUpdatedTime() const;

    int changeUpdatedTime();
    int rename(std::string new_name);
    bool isDir() const;
    virtual int displayContent() const = 0;
    virtual int getSize() const = 0;
    int displayInfo();

    std::string getPermissions() const;
    int setPermissions(std::string new_perms);
    bool hasPermission(char perm) const;
};

class DirectoryObject : public FileSystemObject
{
  private:
    DirectoryObject(std::string inp_name, DirectoryObject *inp_parent);

  public:
    std::vector<FileSystemObject *> childObjects{};
    ~DirectoryObject() override;

    static DirectoryObject *create(std::string name, DirectoryObject *parent);

    int addChildObject(FileSystemObject *childObject);
    int removeChildObject(FileSystemObject *childObject);
    FileSystemObject *getChild(std::string name);
    int displayContent() const override;
    int displayDetailedContent() const;
    int getSize() const override;
};

class FileObject : public FileSystemObject
{
  private:
    std::string content{""};
    FileObject(std::string inp_name, DirectoryObject *inp_parent);

  public:
    ~FileObject() override;

    static FileObject *create(std::string name, DirectoryObject *parent);

    int rewrite(std::string new_content);
    int addWrite(std::string new_content);
    std::string read() const;
    int displayContent() const override;
    int getSize() const override;
};

#endif
