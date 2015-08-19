// Copyright 2014 The Android Open Source Project
//
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//
//      http://www.apache.org/licenses/LICENSE-2.0
//
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.

#ifndef FIREWALLD_IPTABLES_H_
#define FIREWALLD_IPTABLES_H_

#include <stdint.h>

#include <set>
#include <string>
#include <utility>
#include <vector>

#include <base/macros.h>
#include <chromeos/errors/error.h>

#if defined(__ANDROID__)
# include "dbus_bindings/org.chromium.Firewalld.h"
#else
# include "firewalld/dbus_adaptor/org.chromium.Firewalld.h"
#endif  // __ANDROID__

namespace firewalld {

enum ProtocolEnum { kProtocolTcp, kProtocolUdp };

class IpTables : public org::chromium::FirewalldInterface {
 public:
  typedef std::pair<uint16_t, std::string> Hole;

  IpTables();
  ~IpTables();

  // D-Bus methods.
  bool PunchTcpHole(uint16_t in_port, const std::string& in_interface) override;
  bool PunchUdpHole(uint16_t in_port, const std::string& in_interface) override;
  bool PlugTcpHole(uint16_t in_port, const std::string& in_interface) override;
  bool PlugUdpHole(uint16_t in_port, const std::string& in_interface) override;

  bool RequestVpnSetup(const std::vector<std::string>& usernames,
                       const std::string& interface) override;
  bool RemoveVpnSetup(const std::vector<std::string>& usernames,
                      const std::string& interface) override;

  // Close all outstanding firewall holes.
  void PlugAllHoles();

 private:
  friend class IpTablesTest;
  FRIEND_TEST(IpTablesTest, ApplyVpnSetupAddSuccess);
  FRIEND_TEST(IpTablesTest, ApplyVpnSetupAddFailureInUsername);
  FRIEND_TEST(IpTablesTest, ApplyVpnSetupAddFailureInMasquerade);
  FRIEND_TEST(IpTablesTest, ApplyVpnSetupAddFailureInRuleForUserTraffic);
  FRIEND_TEST(IpTablesTest, ApplyVpnSetupRemoveSuccess);
  FRIEND_TEST(IpTablesTest, ApplyVpnSetupRemoveFailure);

  bool PunchHole(uint16_t port,
                 const std::string& interface,
                 std::set<Hole>* holes,
                 ProtocolEnum protocol);
  bool PlugHole(uint16_t port,
                const std::string& interface,
                std::set<Hole>* holes,
                ProtocolEnum protocol);

  bool AddAcceptRules(ProtocolEnum protocol,
                      uint16_t port,
                      const std::string& interface);
  bool DeleteAcceptRules(ProtocolEnum protocol,
                         uint16_t port,
                         const std::string& interface);

  virtual bool AddAcceptRule(const std::string& executable_path,
                             ProtocolEnum protocol,
                             uint16_t port,
                             const std::string& interface);
  virtual bool DeleteAcceptRule(const std::string& executable_path,
                                ProtocolEnum protocol,
                                uint16_t port,
                                const std::string& interface);

  bool ApplyVpnSetup(const std::vector<std::string>& usernames,
                     const std::string& interface,
                     bool add);

  virtual bool ApplyMasquerade(const std::string& interface, bool add);
  virtual bool ApplyMarkForUserTraffic(const std::string& user_name, bool add);
  virtual bool ApplyRuleForUserTraffic(bool add);

  int ExecvNonRoot(const std::vector<std::string>& argv,
                   uint64_t capmask);

  // Keep track of firewall holes to avoid adding redundant firewall rules.
  std::set<Hole> tcp_holes_;
  std::set<Hole> udp_holes_;

  // Tracks whether IPv6 filtering is enabled. If set to |true| (the default),
  // then it is required to be working. If |false|, then adding of IPv6 rules is
  // still attempted but not mandatory; however, if it is successful even once,
  // then it'll be changed to |true| and enforced thereafter.
  bool ip6_enabled_ = true;

  DISALLOW_COPY_AND_ASSIGN(IpTables);
};

}  // namespace firewalld

#endif  // FIREWALLD_IPTABLES_H_
