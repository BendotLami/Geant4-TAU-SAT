#include "FilePrinter.hh"

void FilePrinter::CloseFile()
{
    if(file.is_open())
        file.close();
    iOpenedFile = false;
}
void FilePrinter::OpenFile(std::string name)
{
    CloseFile();
    file.open(name, std::ios_base::app);
    file << "### NEW RUN RESULTS: ###" << std::endl;
    iOpenedFile = true;
}

FilePrinter::FilePrinter(std::string name)
{
    OpenFile(name);
    iOpenedFile = true;
}

FilePrinter::~FilePrinter()
{
    if(iOpenedFile)
        CloseFile();
}
