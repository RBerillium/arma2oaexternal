#pragma once 
#include <string>
#include <iostream>
#include "kdmapper/Pdb.h"
#include "kdmapper/SymLoader.h"
#include "kdmapper/Pe.hpp"

struct pdb_t
{

    std::wstring executalbe_path;
    std::wstring symbol_path;

    Pdb::Prov prov;

    bool load(std::wstring path) 
    {
        try
        {
            this->executalbe_path = path;



            const auto pdbInfo = this->prov.getPdbInfo(this->executalbe_path.c_str());

            const auto url = std::wstring(Pdb::Prov::k_microsoftSymbolServerSecure)
                + L"/" + pdbInfo.pdbUrl();


            wchar_t temp_path[MAX_PATH];


            GetTempPathW(MAX_PATH, temp_path);
            std::wstring temp_path_string(temp_path);
            this->symbol_path = temp_path_string + L"Symbols\\";


            Pdb::WinInetFileDownloader downloader((this->symbol_path + pdbInfo.pdbPath()).c_str());
            const bool downloadStatus = Pdb::SymLoader::download(url.c_str(), downloader);
            if (!downloadStatus)
            {
                printf("[-]Unable to download the PDB");
                return false;
            }
        }
        catch (std::exception& ex)
        {
         
            std::cout << ex.what() << std::endl;
            return false;
       }
        return true;
    };
    bool unload() 
    {
        //return(_wremove(this->symbol_path.c_str()));
        return true;
    }; // mock


	uint64_t get_rva(std::wstring function_name)
	{
      
        prov.setSymPath(this->symbol_path.c_str());

        Pdb::Mod mod(this->executalbe_path.c_str());


        auto sym = mod.find(function_name.c_str());
        auto a = sym.query<Pdb::SymInfo::GetAddress>() - mod.base();

        return a;
	}
};