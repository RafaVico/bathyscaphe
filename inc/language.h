#ifndef LANGUAGE_H
#define LANGUAGE_H

struct lang_file
{
  std::string lang;
  std::string file;
  std::string author;
};

class language
{
  private:
    char nullstring;
    int id_language;
    std::vector<lang_file> language_list;
    std::vector<std::string> string_list;
    std::string read_name(std::string file_name);
    std::string read_author(std::string file_name);
  public:
    language();
    ~language();
    void read_languages();
    int languages_count();
    void set_language(int id);
    int language_id();
    char* language_name(int id);
    char* language_author(int id);
    char* get_string(int id);
};

#endif
