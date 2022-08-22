#include <PcapFileDevice.h>
#include <TcpReassembly.h>

uint64_t ms(const timeval &t) {
  return t.tv_sec * 1000 + t.tv_usec / 1000;
}
uint64_t ms(const timespec &t) {
  return t.tv_sec * 1000 + t.tv_nsec / 1000000;
}

enum Type : uint8_t {
  OPEN,
  CLOSE,
  DATA0,
  DATA1,
};
struct Item {
  uint64_t time;
  uint32_t id;
  uint32_t type : 2, size : 30;
};
static_assert(sizeof(Item) == 16);

struct Tcp {
  size_t _id{0};
  std::map<uint32_t, uint32_t> _ids;
  std::ofstream w;

  void _write(const Item &item) {
    w.write((const char *)&item, sizeof(item));
  }
  void _open(uint64_t time, uint32_t id) {
    _write({time, id, OPEN});
  }
  void _close(uint64_t time, uint32_t id) {
    _write({time, id, CLOSE});
  }
  void _data(uint64_t time, uint32_t id, int8_t side, std::string_view data) {
    _write({time, id, side == 0 ? DATA0 : DATA1, (uint32_t)data.size()});
    w.write(data.data(), data.size());
  }

  Tcp(const std::string &path) : w{path + ".tcp"} {
    pcpp::PcapFileReaderDevice dev{path};
    if (not dev.open()) abort();
    pcpp::TcpReassembly flow{
      +[](int8_t side, const pcpp::TcpStreamData &data, void *_tcp) {
        auto &tcp{*((Tcp *)_tcp)};
        auto &conn{data.getConnectionData()};
        if (data.isBytesMissing()) abort();
        tcp._data(ms(data.getTimeStamp()), tcp._ids.at(conn.flowKey), side, {(const char *)data.getData(), data.getDataLength()});
      },
      this,
      +[](const pcpp::ConnectionData &conn, void *_tcp) {
        auto &tcp{*((Tcp *)_tcp)};
        auto id{tcp._id++};
        tcp._ids[conn.flowKey] = id;
        tcp._open(ms(conn.startTime), id);
      },
      +[](const pcpp::ConnectionData &conn, pcpp::TcpReassembly::ConnectionEndReason, void *_tcp) {
        auto &tcp{*((Tcp *)_tcp)};
        tcp._close(ms(conn.endTime), tcp._ids.at(conn.flowKey));
      },
    };
    pcpp::RawPacket raw;
    uint64_t _time;
    while (dev.getNextPacket(raw)) {
      auto time{ms(raw.getPacketTimeStamp())};
      if (_time > time) abort();
      _time = time;
      flow.reassemblePacket(&raw);
    }
  }
};

int main(int argc, char **argv) {
  if (argc <= 1) {
    return 0;
  }
  for (int i{1}; i < argc; ++i) {
    Tcp{argv[i]};
  }
}
