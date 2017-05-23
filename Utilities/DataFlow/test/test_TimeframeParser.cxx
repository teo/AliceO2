#include "DataFlow/TimeframeParser.h"
#include "DataFlow/FakeTimeframeBuilder.h"
#include "Headers/DataHeader.h"
#include <FairMQParts.h>
#include <istream>
#include <cstdlib>

struct OneShotReadBuf : public std::streambuf
{
    OneShotReadBuf(char* s, std::size_t n)
    {
        setg(s, s, s + n);
    }
};

using DataHeader = o2::Header::DataHeader;

int
main(int argc, char **argv) {
  // Construct a dummy timeframe.
  // Stream it and get the parts
  FairMQParts parts;
  auto onAddParts = [](FairMQParts &p, char *buffer, size_t size) {
    LOG(INFO) << "Adding part to those to be sent.\n";
  };
  auto onSend = [](FairMQParts &p) {
    LOG(INFO) << "Everything OK. Sending parts\n";
  };

  // Prepare a test timeframe to be streamed
  auto zeroFiller = [](char *b, size_t s) {memset(b, 0, s);};
  std::vector<o2::DataFlow::FakeTimeframeSpec> specs = {
    {
      .origin = "TPC",
      .dataDescription = "CLUSTERS",
      .bufferFiller = zeroFiller,
      .bufferSize = 1000
    }
  };

  size_t testBufferSize;
  auto testBuffer = fakeTimeframeGenerator(specs, testBufferSize);

  OneShotReadBuf osrb(testBuffer.get(), testBufferSize);
  std::istream s(&osrb);

  try {
    o2::DataFlow::streamTimeframe(s, onAddParts, onSend);
  } catch(std::runtime_error &e) {
    LOG(ERROR) << e.what() << std::endl;
    exit(1);
  }
}