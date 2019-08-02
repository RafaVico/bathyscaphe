#include <vector>
#include <string>
#include <fstream>
#include <dirent.h>
#include "../inc/language.h"

language::language()
{
  id_language=0;
  read_languages();
  nullstring=0;
}

language::~language()
{
}

std::string language::read_name(std::string file_name)
{
  std::ifstream file;
  std::string name;

  std::string filename="lang/"+file_name;
  file.open(filename.c_str());
  if(file)
  {
    char buffer[1024];
    std::string bfline;
    std::string code;
    while(file.getline(buffer,1024))
    {
      bfline=buffer;
      int i=bfline.find(" ",1);
      code=bfline.substr(0,i);
#ifdef PLATFORM_GP2X
      name=bfline.substr(i+1,bfline.size()-i-2);
#endif
#ifdef PLATFORM_WIN
      name=bfline.substr(i+1,bfline.size()-i-1);
#endif
      if(code.substr(0,1)=="@")
        if(code.substr(1)=="lang")
          break;
    }
  }
  file.close();

  return name;
}

std::string language::read_author(std::string file_name)
{
  std::ifstream file;
  std::string author;

  std::string filename="lang/"+file_name;
  file.open(filename.c_str());
  if(file)
  {
    char buffer[1024];
    std::string bfline;
    std::string code;
    while(file.getline(buffer,1024))
    {
      bfline=buffer;
      int i=bfline.find(" ",1);
      code=bfline.substr(0,i);
#ifdef PLATFORM_GP2X
      author=bfline.substr(i+1,bfline.size()-i-2);
#endif
#ifdef PLATFORM_WIN
      author=bfline.substr(i+1,bfline.size()-i-1);
#endif
      if(code.substr(0,1)=="@")
        if(code.substr(1)=="author")
          break;
    }
  }
  file.close();

  return author;
}

void language::read_languages()
{
  DIR *dp;
  struct dirent *dirp;

   if((dp=opendir("./lang")) != NULL)
   {
      while ((dirp=readdir(dp)) != NULL)
      {
        if(dirp->d_name!=std::string(".") && dirp->d_name!=std::string("..") && std::string(dirp->d_name).substr(std::string(dirp->d_name).size()-5)==".lang")
        {
          lang_file tmp;
          tmp.file=dirp->d_name;
          //tmp.lang=dirp->d_name;
          //std::string n=dirp->d_name;
          tmp.lang=read_name(dirp->d_name);
          tmp.author=read_author(dirp->d_name);
          language_list.push_back(tmp);
        }
      }
      closedir(dp);
   }
}

int language::languages_count()
{
  return language_list.size();
}

void language::set_language(int id)
{
  std::ifstream file;

  if(id>=0 && id<language_list.size())
  {
    std::string filename="lang/"+language_list[id].file;
    file.open(filename.c_str());
    if(file)
    {
      id_language=id;
      string_list.clear();

      char buffer[1024];
      std::string bfline;
      std::string code;
      std::string line;
      while(file.getline(buffer,1024))
      {
        bfline=buffer;
        int i=bfline.find(" ",1);
        code=bfline.substr(0,i);
#ifdef PLATFORM_GP2X
        line=bfline.substr(i+1,bfline.size()-i-2);
#endif
#ifdef PLATFORM_WIN
        line=bfline.substr(i+1,bfline.size()-i-1);
#endif
        if(code.substr(0,1)=="@")
        {
          if(code.substr(1)!="lang")
          {
            int id=atoi(code.substr(1).c_str());
            if(id>=string_list.size())
            {
              int sz=id+1-string_list.size();
              for(int f=0; f<sz; f++)
              {
                string_list.push_back("");
              }
            }
            string_list[id]=line;
          }
        }
      }
    }
    file.close();
  }
}

int language::language_id()
{
  return id_language;
}

char* language::language_name(int id)
{
  if(id>=0 && id <language_list.size())
    return (char*)language_list[id].lang.c_str();
  return &nullstring;
}

char* language::language_author(int id)
{
  if(id>=0 && id <language_list.size())
    return (char*)language_list[id].author.c_str();
  return &nullstring;
}

char* language::get_string(int id)
{
  if(id>=0 && id<string_list.size())
    return (char*)string_list[id].c_str();
  return &nullstring;
}
