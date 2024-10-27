#include <stdio.h>
#include <iostream>
#include <fstream>
#include <vector>
#include <string>
#include <filesystem>
#include <functional>

enum FILE_TYPE {
    IMAGE,
    TEXT
};

class item {
public:
    item(const std::string& path1, const std::string& title1, FILE_TYPE tp1) : path(path1), title(title1), file_type(tp1) {}
    std::string path;
    std::string title;
    FILE_TYPE file_type;
};

std::vector<std::string> pdfs;
std::vector<std::string> indexes;

bool ends_with(std::string const& value, std::string const& ending)
{
    if (ending.size() > value.size()) return false;
    return std::equal(ending.rbegin(), ending.rend(), value.rbegin());
}

std::string base_name(std::string const & path)
{
    return path.substr(path.find_last_of("/\\") + 1);
}

std::string read_file(std::filesystem::path path)
{
    std::ifstream f(path, std::ios::in | std::ios::binary);
    const auto sz = std::filesystem::file_size(path);
    std::string result(sz, '\0');
    f.read(result.data(), sz);
    return result;
}

std::string replace_all(std::string str, const std::string& from, const std::string& to)
{
    size_t start_pos = 0;
    while((start_pos = str.find(from, start_pos)) != std::string::npos) {
        str.replace(start_pos, from.length(), to);
        start_pos += to.length(); // Handles case where 'to' is a substring of 'from'
    }
    return str;
}

void gen(const std::string base_path, const std::vector<item>& items)
{
  if(items.empty())
  {
      return;
  }

  std::fstream ofile;
  ofile.open(base_path+"/index.htm", std::ios::out);
  if(!ofile.is_open()) {
      std::cout << "can't open output file" << std::endl;
      exit(1);
  }

  ofile << "<html>" << std::endl;
  ofile << "<head>" << std::endl;
  ofile << "<meta charset=\"utf-8\" />" << std::endl; 
  ofile << "</head>" << std::endl;
  ofile << "<body>" << std::endl;

  ofile << "<div>" << base_path << "</div>" << std::endl;

  ofile << "<table border=0>" << std::endl;
  bool first = true;
  std::string t1;
  std::string t2;
  for(const auto& i : items) {
      if(i.file_type != FILE_TYPE::IMAGE)
      {
          continue;
      }
      if(first)
      {
          ofile << " <tr>" << std::endl;
          t1 = i.title;
          t2 = "";
      }
      {
          ofile << "  <td style=\"vertical-align:bottom\">" << std::endl;
          ofile << "   <img src=\"" << i.path << "\" width=\"480\">" << std::endl;
          ofile << "  </td>" << std::endl;
      }
      if(!first)
      {
          ofile << " </tr>" << std::endl;
          t2 = i.title;

          ofile << " <tr>" << std::endl;
          ofile << "  <td>" << t1 << "</td>" << std::endl;
          ofile << "  <td>" << t2 << "</td>" << std::endl;
          ofile << " </tr>" << std::endl;
      }
      first = !first;
  }

  if(!first)
  {
      ofile << " </tr>" << std::endl;

      ofile << " <tr>" << std::endl;
      ofile << "  <td>" << t1 << "</td>" << std::endl;
      ofile << "  <td>" << "</td>" << std::endl;
      ofile << " </tr>" << std::endl;
  }

  ofile << "</table>" << std::endl;

  for(const auto& i : items) {
      if(i.file_type == FILE_TYPE::TEXT)
      {
          std::string s = read_file(i.path);
          s = replace_all(s, "\n", "<br>\n");
          ofile << "<div>" << s << "</div>" << std::endl;
      }
  }

  ofile << "</body>" << std::endl;
  ofile << "</html>" << std::endl;
  ofile.close();

//  std::string cmd = "htmldoc --webpage -f " + base_path + "/index.pdf " + base_path + "/index.htm";
  std::string cmd = "wkhtmltopdf --enable-local-file-access " + base_path + "/index.htm " + base_path + "/index.pdf";
  system(cmd.c_str());

  pdfs.push_back(base_path + "/index.pdf");
  indexes.push_back(base_path + "/index.htm");
}

bool starts_with(const std::string& s, const std::string& word)
{
    return s.rfind(word, 0) == 0;
}

void scan(const std::string& base_path)
{
    std::vector<std::string> folders;
    std::vector<item> items;

    for (const auto& entry : std::filesystem::directory_iterator(base_path))
    {
        std::cout << entry.path();
        if(entry.is_directory())
        {
            std::cout << " -- folder";
            if(!starts_with(base_name(entry.path()), "book"))
            {
              folders.push_back(entry.path());
            }
        }
        else if(ends_with(entry.path(), ".JPG") || ends_with(entry.path(), ".jpg") || ends_with(entry.path(), ".png") || ends_with(entry.path(), ".gif"))
        {
            items.push_back(item(entry.path(), base_name(entry.path()), FILE_TYPE::IMAGE));
            std::cout << " -- image";
        }
        else if(ends_with(entry.path(), ".txt"))
        {
            items.push_back(item(entry.path(), base_name(entry.path()), FILE_TYPE::TEXT));
            std::cout << " -- text";
        }
        else
        {
            std::cout << " -- skipping";
        }
        std::cout << std::endl;
    }

    std::sort(items.begin(), 
              items.end(), 
              [](const auto& a, const auto& b) { return a.title < b.title; });

    std::sort(folders.begin(), 
              folders.end(), 
              [](const auto& a, const auto& b) { return a < b; });
 
    for(const auto& f : folders)
    {
        scan(f);
    }

    gen(base_path, items);
}


int main(int argc, char *argv[])
{
   if(argc < 2) {
       std::cout << "Usage:" << std::endl;
       std::cout << "gendoc folder" << std::endl;
       exit(1);
   }

   std::string folder = argv[1];
   if(ends_with(folder, "/"))
   {
       folder.pop_back();
   }
   scan(folder);

   // combine all files on one
   std::string cmd = "pdfunite ";
   for(const auto& pdf : pdfs)
   {
       cmd += pdf;
       cmd += " ";
   }
   cmd += folder;
   cmd += "/" + base_name(folder) + "_.pdf";
   system(cmd.c_str());

   // cleanup
   for(const auto& f : pdfs)
   {
       std::remove(f.c_str());
   }
   for(const auto& f : indexes)
   {
       std::remove(f.c_str());
   }
}
