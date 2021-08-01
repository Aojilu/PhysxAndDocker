#include "WebSocketppFarcade.h"
#pragma region server


    //typedef websocketpp::server<websocketpp::config::asio> server;

    //using websocketpp::lib::placeholders::_1;
    //using websocketpp::lib::placeholders::_2;
    //using websocketpp::lib::bind;

    //// pull out the type of messages sent by our config
    //typedef server::message_ptr message_ptr;

    //// Define a callback to handle incoming messages
    //void on_message(server* s, websocketpp::connection_hdl hdl, message_ptr msg) {
    //    std::cout << "on_message called with hdl: " << hdl.lock().get()
    //        << " and message: " << msg->get_payload()
    //        << std::endl;

    //    // check for a special command to instruct the server to stop listening so
    //    // it can be cleanly exited.
    //    if (msg->get_payload() == "stop-listening") {
    //        s->stop_listening();
    //        return;
    //    }

    //    try {
    //        s->send(hdl, msg->get_payload(), msg->get_opcode());

    //    }
    //    catch (websocketpp::exception const& e) {
    //        std::cout << "Echo failed because: "
    //            << "(" << e.what() << ")" << std::endl;
    //    }
    //    try {
    //        string command = msg->get_payload();
    //        if(_recieveMessage)_recieveMessage(command);
    //    }
    //    catch (exception e) {
    //        cout << "error=" << e.what() << endl;
    //    }
    //}

#pragma endregion

typedef websocketpp::client<websocketpp::config::asio_client> client;
#pragma region echoclient
function<void(string)> _recieveMessageFunc;
function<string()> _backMessageFunc;//データを送り返すときの処理


    //typedef websocketpp::client<websocketpp::config::asio_client> echoclient;

    using websocketpp::lib::placeholders::_1;
    using websocketpp::lib::placeholders::_2;
    using websocketpp::lib::bind;

    // pull out the type of messages sent by our config
    typedef websocketpp::config::asio_client::message_type::ptr message_ptr;

    // This message handler will be invoked once for each incoming message. It
    // prints the message and then sends a copy of the message back to the server.
    void on_message(client* c, websocketpp::connection_hdl hdl, message_ptr msg) {
        std::cout << "on_message called with hdl: " << hdl.lock().get()
            << " and message: " << msg->get_payload()
            << std::endl;


        string command = msg->get_payload();
        websocketpp::lib::error_code ec;
        //送り返し
        if (msg->get_payload() == "back") {
            if (!_backMessageFunc)return;
            //c->send(hdl, "re" + _backMessageFunc(), msg->get_opcode(), ec);
            c->send(hdl, _backMessageFunc(), msg->get_opcode(), ec);
            if (ec) {
                std::cout << "Echo failed because: " << ec.message() << std::endl;
            }
        }
        else {
            if (_recieveMessageFunc)_recieveMessageFunc(command);
        }
    }
#pragma endregion

#pragma region client

    function<void(string)>_replayMessageFunc;//帰ってきたデータの処理

    class connection_metadata {
    public:
        typedef websocketpp::lib::shared_ptr<connection_metadata> ptr;

        connection_metadata(int id, websocketpp::connection_hdl hdl, std::string uri)
            : m_id(id)
            , m_hdl(hdl)
            , m_status("Connecting")
            , m_uri(uri)
            , m_server("N/A")
        {}

        void on_open(client* c, websocketpp::connection_hdl hdl) {
            m_status = "Open";

            client::connection_ptr con = c->get_con_from_hdl(hdl);
            m_server = con->get_response_header("Server");
        }

        void on_fail(client* c, websocketpp::connection_hdl hdl) {
            m_status = "Failed";

            client::connection_ptr con = c->get_con_from_hdl(hdl);
            m_server = con->get_response_header("Server");
            m_error_reason = con->get_ec().message();
        }

        void on_close(client* c, websocketpp::connection_hdl hdl) {
            m_status = "Closed";
            client::connection_ptr con = c->get_con_from_hdl(hdl);
            std::stringstream s;
            s << "close code: " << con->get_remote_close_code() << " ("
                << websocketpp::close::status::get_string(con->get_remote_close_code())
                << "), close reason: " << con->get_remote_close_reason();
            m_error_reason = s.str();
        }

        void on_message(websocketpp::connection_hdl, client::message_ptr msg) {
            if (msg->get_opcode() == websocketpp::frame::opcode::text) {//この条件よくわからない
                m_messages.push_back("<< " + msg->get_payload());
                if (_replayMessageFunc)_replayMessageFunc(msg->get_payload());
            }
            else {
                m_messages.push_back("<< " + websocketpp::utility::to_hex(msg->get_payload()));
            }
        }

        websocketpp::connection_hdl get_hdl() const {
            return m_hdl;
        }

        int get_id() const {
            return m_id;
        }

        std::string get_status() const {
            return m_status;
        }

        void record_sent_message(std::string message) {
            m_messages.push_back(">> " + message);
        }

        friend std::ostream& operator<< (std::ostream& out, connection_metadata const& data);
    private:
        int m_id;
        websocketpp::connection_hdl m_hdl;
        std::string m_status;
        std::string m_uri;
        std::string m_server;
        std::string m_error_reason;
        std::vector<std::string> m_messages;
    };

    std::ostream& operator<< (std::ostream& out, connection_metadata const& data) {
        out << "> URI: " << data.m_uri << "\n"
            << "> Status: " << data.m_status << "\n"
            << "> Remote Server: " << (data.m_server.empty() ? "None Specified" : data.m_server) << "\n"
            << "> Error/close reason: " << (data.m_error_reason.empty() ? "N/A" : data.m_error_reason) << "\n";
        out << "> Messages Processed: (" << data.m_messages.size() << ") \n";

        std::vector<std::string>::const_iterator it;
        for (it = data.m_messages.begin(); it != data.m_messages.end(); ++it) {
            out << *it << "\n";
        }

        return out;
    }

    class websocket_endpoint {
    public:
        websocket_endpoint() : m_next_id(0) {
            m_endpoint.clear_access_channels(websocketpp::log::alevel::all);
            m_endpoint.clear_error_channels(websocketpp::log::elevel::all);

            m_endpoint.init_asio();
            m_endpoint.start_perpetual();
            
            m_thread = websocketpp::lib::make_shared<websocketpp::lib::thread>(&client::run, &m_endpoint);
        }

        ~websocket_endpoint() {
            m_endpoint.stop_perpetual();

            for (con_list::const_iterator it = m_connection_list.begin(); it != m_connection_list.end(); ++it) {
                if (it->second->get_status() != "Open") {
                    // Only close open connections
                    continue;
                }

                std::cout << "> Closing connection " << it->second->get_id() << std::endl;

                websocketpp::lib::error_code ec;
                m_endpoint.close(it->second->get_hdl(), websocketpp::close::status::going_away, "", ec);
                if (ec) {
                    std::cout << "> Error closing connection " << it->second->get_id() << ": "
                        << ec.message() << std::endl;
                }
            }

            m_thread->join();
        }

        int connect(std::string const& uri) {
            websocketpp::lib::error_code ec;

            client::connection_ptr con = m_endpoint.get_connection(uri, ec);

            if (ec) {
                std::cout << "> Connect initialization error: " << ec.message() << std::endl;
                return -1;
            }

            int new_id = m_next_id++;
            connection_metadata::ptr metadata_ptr = websocketpp::lib::make_shared<connection_metadata>(new_id, con->get_handle(), uri);
            m_connection_list[new_id] = metadata_ptr;

            con->set_open_handler(websocketpp::lib::bind(
                &connection_metadata::on_open,
                metadata_ptr,
                &m_endpoint,
                websocketpp::lib::placeholders::_1
            ));
            con->set_fail_handler(websocketpp::lib::bind(
                &connection_metadata::on_fail,
                metadata_ptr,
                &m_endpoint,
                websocketpp::lib::placeholders::_1
            ));
            con->set_close_handler(websocketpp::lib::bind(
                &connection_metadata::on_close,
                metadata_ptr,
                &m_endpoint,
                websocketpp::lib::placeholders::_1
            ));
            con->set_message_handler(websocketpp::lib::bind(
                &connection_metadata::on_message,
                metadata_ptr,
                websocketpp::lib::placeholders::_1,
                websocketpp::lib::placeholders::_2
            ));

            m_endpoint.connect(con);

            return new_id;
        }

        void close(int id, websocketpp::close::status::value code, std::string reason) {
            websocketpp::lib::error_code ec;

            con_list::iterator metadata_it = m_connection_list.find(id);
            if (metadata_it == m_connection_list.end()) {
                std::cout << "> No connection found with id " << id << std::endl;
                return;
            }

            m_endpoint.close(metadata_it->second->get_hdl(), code, reason, ec);
            if (ec) {
                std::cout << "> Error initiating close: " << ec.message() << std::endl;
            }
        }

        void send(int id, std::string message) {
            websocketpp::lib::error_code ec;

            con_list::iterator metadata_it = m_connection_list.find(id);
            if (metadata_it == m_connection_list.end()) {
                std::cout << "> No connection found with id " << id << std::endl;
                return;
            }

            m_endpoint.send(metadata_it->second->get_hdl(), message, websocketpp::frame::opcode::text, ec);
            if (ec) {
                std::cout << "> Error sending message: " << ec.message() << std::endl;
                return;
            }

            metadata_it->second->record_sent_message(message);
        }

        connection_metadata::ptr get_metadata(int id) const {
            con_list::const_iterator metadata_it = m_connection_list.find(id);
            if (metadata_it == m_connection_list.end()) {
                return connection_metadata::ptr();
            }
            else {
                return metadata_it->second;
            }
        }

        client m_endpoint;
    private:
        typedef std::map<int, connection_metadata::ptr> con_list;

        websocketpp::lib::shared_ptr<websocketpp::lib::thread> m_thread;

        con_list m_connection_list;
        int m_next_id;

        //client m_endpoint;
    };

#pragma endregion


    namespace WebSocketppFarcade {


        websocket_endpoint endpoint;


        //void Listen()
        //{

        //    //server echo_server;
        //    //server* echo_server_a = &echo_server;

        //    //try {

        //    //    // Set logging settings
        //    //    echo_server.set_access_channels(websocketpp::log::alevel::all);
        //    //    echo_server.clear_access_channels(websocketpp::log::alevel::frame_payload);

        //    //    // Initialize Asio
        //    //    echo_server.init_asio();

        //    //    // Register our message handler
        //    //    echo_server.set_message_handler(bind(&on_message, &echo_server, ::_1, ::_2));

        //    //    // Listen on port 9002
        //    //    echo_server.listen(9002);

        //    //    // Start the server accept loop
        //    //    echo_server.start_accept();

        //    //    // Start the ASIO io_service run loop
        //    //    echo_server.run();
        //    //}
        //    //catch (websocketpp::exception const& e) {
        //    //    std::cout << e.what() << std::endl;
        //    //}
        //    //catch (...) {
        //    //    std::cout << "other exception" << std::endl;
        //    //}
        //}

        //int maxid = -1;
        //サーバーで利用
        void Connect(string uri)
        {
            int id = endpoint.connect(uri);
            if (id != -1) {
                std::cout << "> Created connection with id " << id << std::endl;
                //if (id > maxid)maxid = id;
            }
        }

        //クライアントで利用
        void ConnectAndRecieve(string uri) {
            try {
                client c;
                // Set logging to be pretty verbose (everything except message payloads)
                c.set_access_channels(websocketpp::log::alevel::all);
                c.clear_access_channels(websocketpp::log::alevel::frame_payload);

                // Initialize ASIO
                c.init_asio();

                // Register our message handler
                c.set_message_handler(bind(&on_message, &c, ::_1, ::_2));

                websocketpp::lib::error_code ec;
                client::connection_ptr con = c.get_connection(uri, ec);
                if (ec) {
                    std::cout << "could not create connection because: " << ec.message() << std::endl;
                    return ;
                }

                // Note that connect here only requests a connection. No network messages are
                // exchanged until the event loop starts running in the next line.
                c.connect(con);
                // Start the ASIO io_service run loop
                // this will cause a single connection to be made to the server. c.run()
                // will exit when this connection is closed.
                c.run();
                
                
            }
            catch (websocketpp::exception const& e) {
                std::cout << e.what() << std::endl;
            }
        }

        void Send(string message,int id)
        {
            /*int id = 0;
            endpoint.send(id, message);
            cout << message << endl;*/
            //int id = 0;
            stringstream ss;
            ss << message;//idが2桁になると動かなくなるくそコードだが今回はこれで許して
            endpoint.send(0, to_string(id) + ss.str());
            //endpoint.send(0, ss.str());
            cout << message << endl;
        }

        void Close()
        {
            int id = 0;
            int close_code = websocketpp::close::status::normal;
            std::string reason;

            endpoint.close(id, close_code, reason);
        }

        void SetRecieveMessage(function<void(string)> func) {
            _recieveMessageFunc = func;
        }
        void SetReplayMessage(function<void(string)> func) {
            _replayMessageFunc = func;
        }

        void SetBackMessage(function<string()>func) {
            _backMessageFunc = func;
        }
    }