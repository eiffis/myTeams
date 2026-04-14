#include "Database.hpp"

bool storePrivateMessages(std::ofstream &out, std::map<std::string, std::vector<Reply>> &pms)
{
    if (out.bad())
        return false;
    size_t pmsnb = pms.size();
    out.write(reinterpret_cast<const char *>(&pmsnb), sizeof(size_t));
    for (auto pm : pms) {
        out.write(reinterpret_cast<const char *>(pm.first.data()), UNPARSED_UUUID * 2);
        storeVector(out, pm.second);
    }
    if (out.fail())
        return false;
    return true;
}

bool loadPrivateMessages(std::ifstream &in, std::map<std::string, std::vector<Reply>> &pms)
{
    if (in.bad())
        return false;
    size_t pmsnb = 0;
    in.read(reinterpret_cast<char *>(&pmsnb), sizeof(size_t));
    if (pmsnb) {
        for (int i = 0; i < pmsnb; i++) {
            std::string id;
            std::vector<Reply> replies;
            id.resize(UNPARSED_UUUID * 2);
            in.read(id.data(), UNPARSED_UUUID * 2);
            loadVector(in, replies);
            pms.emplace(std::move(id), std::move(replies));
        }
    }
    if (in.fail())
        return false;
    return true;
}
