#ifndef __RIBBIT_REQUEST_DATA_H
#define __RIBBIT_REQUEST_DATA_H

#include <string>

struct RibbitRequestData
{
    RibbitRequestData() : Command(""), SubCommand(""), Product(""), Seqn(0), Flag("") { }
    RibbitRequestData(std::string cmd, std::string sub, std::string prod, unsigned long long seqn, std::string flag) : Command(cmd), SubCommand(sub), Product(prod), Seqn(seqn), Flag(flag) { }

    std::string Command;
    std::string SubCommand;
    std::string Product;
    unsigned long long Seqn;
    std::string Flag;
};

#endif