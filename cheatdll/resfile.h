#pragma once

#include <map>
#include <vector>

struct RESFILE_entry
{
    int resfile_id;
    int offset;
    int size;
    void* data;
    int reference_count;
};

struct RESFILE_group
{
    unsigned int type;
    std::map<int, RESFILE_entry> entries;
};

class RESFILE
{
private:
    std::vector<RESFILE_group> groups;
    std::vector<FILE*> res_files;
public:
    void add_resource_file(char* filename);
    void* get_resource(unsigned int ext, int id, int* size);
};

void setResFileHooks();
