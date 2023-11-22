#include <iostream>
#include "UpdateLogic/FullpackageUpdate.h"
#include "UpdateLogic/DifferencePackageUpdate.h"

int main() {
    try {
        DifferencePackageUpdate update("http://api.kirayuukiasuna.cloud", "EditorDemo", "EditorDemo");
        update.execute();
    }catch (std::exception& e){
        std::cerr<<e.what()<<"\n";
    }

    return 0;
}


void test(){
//    ApiRequest api("http://192.168.0.114:5275", "GameApp1");

//    auto [returnWrapper1,content1] = api.DownloadCurrentFullPackage("appfullpackage_110");
//
//    auto [returnWrapper2,content2] = api.DownloadFullPackage("1.0.0","appfullpackage_100");
//
//    auto [returnWrapper3,content3] = api.DownloadCurrentDifferencePackage("appdiffpackage_110");
//
//    auto [returnWrapper4,content4] = api.DownloadDifferencePackage("1.1.0","appdiffpackage_110_2");
//
//    MD5 md5;
//    std::ifstream infile;

//    infile.open("appfullpackage_110",std::ios::binary);
//    md5.update(infile);
//    std::cout<<md5.toString()<<"\n";

//    infile.open("appfullpackage_100",std::ios::binary);
//    md5.update(infile);
//    std::cout<<md5.toString()<<"\n";

//    infile.open("appdiffpackage_110",std::ios::binary);
//    md5.update(infile);
//    std::cout<<md5.toString()<<"\n";

//    infile.open("appdiffpackage_110_2",std::ios::binary);
//    md5.update(infile);
//    std::cout<<md5.toString()<<"\n";
}
