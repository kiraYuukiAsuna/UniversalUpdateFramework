#include <iostream>
#include "UpdateCore.h"

int main() {

    ApiRequest api("http://api.kirayuukiasuna.cloud", "GameApp1");

    auto [returnWrapper,content] = api.GetAppManifest("1.0.0");

    std::cout<<content<<"\n";

    return 0;
}
