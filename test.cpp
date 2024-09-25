#include <boost/filesystem.hpp>
#include <iostream>
int main()
{
    std::cout << boost::filesystem::exists("D:\\oci_images\\storage.conf") << std::endl;
}