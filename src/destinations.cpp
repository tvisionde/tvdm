#include "destination.h"
#include <filesystem>
#include <fstream>

SessionsList XSessions = SessionsList();
SessionsList WaylandSessions = SessionsList();

TSubMenu& populateXSessions()
{
    std::filesystem::directory_entry p("/usr/share/xsessions");
    TSubMenu& result = *new TSubMenu("X sessions", 0, hcNoContext);

    if (p.exists() && p.is_directory())
    {
        int i = 0;

        for (auto const& dir_entry : std::filesystem::directory_iterator{p})
        {
            if ((!dir_entry.is_directory()) &&
                (dir_entry.path().extension() == ".desktop"))
            {
                i += 1;
                std::ifstream input(dir_entry.path());

                std::string name, exec;

                // TODO: Add a proper reader. This is not how it should be
                // Assuming these .desktop files are properly made.
                for (std::string line; getline(input, line);)
                {
                    if (line.find("Name=") != -1)
                    {
                        line.erase(0, 5);
                        name = line;
                    }

                    if (line.find("Exec=") != -1)
                    {
                        line.erase(0, 5);
                        exec = line;
                        XSessions.push_back(line.c_str());
                    }
                }

                // += does not work
                result = result + *new TMenuItem(name.c_str(), i, kbNoKey, hcNoContext);
            }
        }
    }
    else
        result = result + *new TMenuItem("*Literally none*", 0, kbNoKey, hcNoContext);

    return result;
}

TSubMenu& populateWaylandSessions()
{
    std::filesystem::directory_entry p("/usr/share/wayland-sessions");
    TSubMenu& result = *new TSubMenu("Wayland sessions", 0, hcNoContext);

    if (p.exists() && p.is_directory())
    {
        int i = 0;

        for (auto const& dir_entry : std::filesystem::directory_iterator{p})
        {
            if ((!dir_entry.is_directory()) &&
                (dir_entry.path().extension() == ".desktop"))
            {
                i += 1;
                std::ifstream input(dir_entry.path());

                std::string name, exec;

                // TODO: Add a proper reader. This is not how it should be
                // Assuming these .desktop files are properly made.
                for (std::string line; getline(input, line);)
                {
                    if (line.find("Name=") != -1)
                    {
                        line.erase(0, 5);
                        name = line;
                    }

                    if (line.find("Exec=") != -1)
                    {
                        line.erase(0, 5);
                        exec = line;
                        WaylandSessions.push_back(line.c_str());
                    }
                }

                result = result + *new TMenuItem(name.c_str(), i, kbNoKey, hcNoContext);
            }
        }
    }
    else
        result = result + *new TMenuItem("*Literally none*", 0, kbNoKey, hcNoContext);
    
    return result;
}