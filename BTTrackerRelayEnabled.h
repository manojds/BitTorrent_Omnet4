//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU Lesser General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
// 
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU Lesser General Public License for more details.
// 
// You should have received a copy of the GNU Lesser General Public License
// along with this program.  If not, see http://www.gnu.org/licenses/.
// 

#ifndef BTTRACKERRELAYENABLED_H_
#define BTTRACKERRELAYENABLED_H_
#include "BTTrackerBase.h"

class INET_API BTTrackerRelayEnabled: public BTTrackerBase {
public:
    BTTrackerRelayEnabled();
    virtual ~BTTrackerRelayEnabled();

    const string& relayInfoHash() const;

protected:
    virtual void initialize();

    void setRelayInfoHash(const string& infoHash);

private:
    string realyIfoHash_var;
};

#endif /* BTTRACKERRELAYENABLED_H_ */