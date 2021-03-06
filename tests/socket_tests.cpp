#include "websocketer/websocketer.hpp"

#include <catch2/catch.hpp>
#include <memory>
#include <string>

TEST_CASE("socket")
{
  namespace beast = boost::beast;
  namespace ws    = websocketer::asio;

  bool passed = false;

  std::string host("echo.websocket.org");
  std::string service("80");

  boost::asio::io_context     io;
  std::shared_ptr<ws::socket> socket = std::make_shared<ws::socket>(io);
  beast::flat_buffer          buffer;
  std::string                 data_to_send("hello world");

  socket->async_open(
      host, service,
      [&data_to_send, &buffer, &passed](const boost::system::error_code &ec,
                                        std::shared_ptr<ws::socket>      socket) {
        if (!ec)
        {
          socket->async_write(
              data_to_send, [&buffer, &passed](const boost::system::error_code &ec,
                                               std::shared_ptr<ws::socket> socket, std::size_t) {
                if (!ec)
                {
                  socket->async_read(
                      buffer, [&passed](const boost::system::error_code &ec,
                                        std::shared_ptr<ws::socket>      socket, std::size_t) {
                        if (!ec)
                        {
                          socket->async_close([&](const boost::system::error_code &ec,
                                                  std::shared_ptr<ws::socket>) {
                            if (!ec)
                            {
                              passed = true;
                            }
                          });
                        }
                      });
                }
              });
        }
      });

  io.run();

  REQUIRE(passed);
}
